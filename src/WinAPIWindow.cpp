#include "pch.h"
#include "WinAPIWindow.h"

#include "../../DebugConsole/include/DebugConsole.h"
#include "base/DebugUntils.h"
#include "WindowException.h"


namespace Awincs
{

	/* Window auto registration in system */
	WinAPIWindow::WinAPIWindowRegisterer WinAPIWindow::registerer;

	WinAPIWindow::WinAPIWindowRegisterer::WinAPIWindowRegisterer()
	{
		assert(!registered);

		WNDCLASSEXW wndClass = {};

		wndClass.lpszClassName = WINDOW_CLASS_NAME.data();
		wndClass.cbSize = sizeof(WNDCLASSEX);
		wndClass.hInstance = GetModuleHandle(NULL);
		wndClass.lpfnWndProc = setupWindowProc;
		wndClass.style = CS_VREDRAW | CS_HREDRAW;
		wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		wndClass.hbrBackground = CreateSolidBrush(DEFAULT_WINDOW_BACKGROUND_COLOR);

		if (RegisterClassExW(&wndClass) == FALSE)
		{
			auto errorCode = std::to_wstring(GetLastError());

			throw WindowException(L"Failed to register class! Error code: " + errorCode);
		}

		registered = true;
	}

	WinAPIWindow::WinAPIWindow(WindowController& windowController, const Point& anchorPoint, const Dimensions& dims)
		:
		anchorPoint(anchorPoint),
		cachedUserDimensions(dims),
		dimensions(dims + DEFAULT_RESIZE_BORDER_WIDTH),
		windowController(windowController)
	{
		assert(registerer.registered);

		auto [x, y] = this->anchorPoint;
		auto [width, height] = this->dimensions;

		CreateWindowW(WINDOW_CLASS_NAME.data(), windowTitle.c_str(), WS_POPUP,
			x, y, width, height, NULL, NULL, hInstance, this);

		if (auto errorCode = GetLastError(); errorCode != ERROR_SUCCESS)
		{
			throw WindowException(L"Failed to create window! Error code: " + std::to_wstring(errorCode));
		}

		ShowWindow(hWnd, SW_HIDE);
		UpdateWindow(hWnd);
	}

	WinAPIWindow::~WinAPIWindow()
	{
		DestroyWindow(hWnd);
	}

	void WinAPIWindow::show()
	{
		ShowWindow(hWnd, SW_SHOW);
	}

	void WinAPIWindow::hide()
	{
		ShowWindow(hWnd, SW_HIDE);
	}

	void WinAPIWindow::minimize()
	{
		ShowWindow(hWnd, SW_MINIMIZE);
	}

	void WinAPIWindow::maximize()
	{
		ShowWindow(hWnd, SW_MAXIMIZE);
	}

	void WinAPIWindow::close()
	{
		SendMessage(hWnd, WM_CLOSE, NULL, NULL);
	}

	HWND WinAPIWindow::getHWND() const
	{
		return this->hWnd;
	}

	void WinAPIWindow::setTitle(const std::wstring& title)
	{
		windowTitle = title;
		SetWindowTextW(hWnd, title.c_str());
	}

	const std::wstring& WinAPIWindow::getTitle() const
	{
		return windowTitle;
	}

	void WinAPIWindow::setAnchorPoint(Point point)
	{
		anchorPoint = point;
		expect(SetWindowPos(hWnd, NULL, point.x, point.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER) == TRUE);
	}

	const WinAPIWindow::Point& WinAPIWindow::getAnchorPoint() const
	{
		return anchorPoint;
	}

	void WinAPIWindow::setDimensions(Dimensions dims)
	{
		// To fix Microsoft bug with WS_POPUP window when changing size
		dimensions = dims + resizeBorderWidth;
		cachedUserDimensions = dims;

		expect(SetWindowPos(hWnd, NULL, 0, 0, dims.width, dims.height, SWP_NOMOVE | SWP_NOZORDER) == TRUE);
	}

	const WinAPIWindow::Dimensions& WinAPIWindow::getDimensions() const
	{
		return cachedUserDimensions;
	}

	void WinAPIWindow::draw(DrawCallback cb)
	{
		drawQueue.push_back(cb);
		expect(InvalidateRect(hWnd, NULL, FALSE) == TRUE);
	}

	void WinAPIWindow::setResizeBorderWidth(int width)
	{
		expect(width >= 0);

		// To fix Microsoft bug with WS_POPUP window when changing size
		dimensions = cachedUserDimensions + width;

		resizeBorderWidth = width;
	}

	LRESULT WinAPIWindow::setupWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if (uMsg == WM_CREATE)
			throw WindowException(L"Failed to setup proxyWindowProc! WM_NCCREATE has not been called!");

		if (uMsg != WM_NCCREATE)
			return DefWindowProc(hWnd, uMsg, wParam, lParam);

		CREATESTRUCT* createStruct = reinterpret_cast<CREATESTRUCT*>(lParam);
		WinAPIWindow* const window = reinterpret_cast<WinAPIWindow* const>(createStruct->lpCreateParams);

		SetLastError(ERROR_SUCCESS);

		window->hWnd = hWnd;

		if (SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG>(window)) == FALSE && GetLastError() != ERROR_SUCCESS)
			throw WindowException(L"Failed to set GWLP_USERDATA when handling setupWindowProc");

		if (SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG>(WinAPIWindow::proxyWindowProc)) == FALSE && GetLastError() != ERROR_SUCCESS)
			throw WindowException(L"Failed to set GWLP_WNDPROC when handling setupWindowProc with proxyWindowProc");

		return window->windowProc(uMsg, wParam, lParam);
	}

	LRESULT WinAPIWindow::proxyWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		WinAPIWindow* const wnd = reinterpret_cast<WinAPIWindow* const>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

		return wnd->windowProc(uMsg, wParam, lParam);
	}

	LRESULT WinAPIWindow::windowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch (uMsg)
		{
		case WM_CREATE: return wmCreate(wParam, lParam);
		case WM_ERASEBKGND: return wmEraseBackground(wParam, lParam);
		case WM_CLOSE: return wmClose(wParam, lParam);
		case WM_PAINT: return wmPaint(wParam, lParam);
		case WM_SIZING:
		case WM_MOVING: return wmSizingMoving(wParam, lParam);
		case WM_SIZE: return wmSize(wParam, lParam);
		case WM_MOVE: return wmMove(wParam, lParam);
		case WM_CHAR: return wmChar(wParam, lParam);
		case WM_UNICHAR: return wmUnichar(wParam, lParam);
		case WM_MOUSEWHEEL: return wmMouseWheel(wParam, lParam);
		case WM_RBUTTONDOWN: return wmRButtonDown(wParam, lParam);
		case WM_RBUTTONUP: return wmRButtonUp(wParam, lParam);
		case WM_LBUTTONDOWN: return wmLButtonDown(wParam, lParam);
		case WM_LBUTTONUP: return wmLButtonUp(wParam, lParam);
		case WM_MBUTTONUP: return wmMButtonUp(wParam, lParam);
		case WM_MBUTTONDOWN: return wmMButtonDown(wParam, lParam);
		case WM_XBUTTONUP: return wmXButtonUp(wParam, lParam);
		case WM_XBUTTONDOWN: return wmXButtonDown(wParam, lParam);
		case WM_KEYUP: return wmKeyUp(wParam, lParam);
		case WM_KEYDOWN: return wmKeyDown(wParam, lParam);
		case WM_NCHITTEST: return wmNCHitTest(wParam, lParam);
		case WM_NCCALCSIZE: return wmNCCalcSize(wParam, lParam);
		}

		return DefWindowProcW(hWnd, uMsg, wParam, lParam);
	}

	LRESULT WinAPIWindow::wmLButtonUp(WPARAM wParam, LPARAM lParam)
	{
		DCONSOLE(L"Left button up: (" << GET_X_LPARAM(lParam) << L',' << GET_Y_LPARAM(lParam) << L")\n");

		return wmMouseButton(wParam, lParam, ComponentEvent::MouseButtonType::LEFT, ComponentEvent::MouseButtonAction::UP);
	}

	LRESULT WinAPIWindow::wmLButtonDown(WPARAM wParam, LPARAM lParam)
	{
		DCONSOLE(L"Left button down: (" << GET_X_LPARAM(lParam) << L',' << GET_Y_LPARAM(lParam) << L")\n");

		return wmMouseButton(wParam, lParam, ComponentEvent::MouseButtonType::LEFT, ComponentEvent::MouseButtonAction::DOWN);
	}

	LRESULT WinAPIWindow::wmRButtonUp(WPARAM wParam, LPARAM lParam)
	{
		DCONSOLE(L"Right button up: (" << GET_X_LPARAM(lParam) << L',' << GET_Y_LPARAM(lParam) << L")\n");

		return wmMouseButton(wParam, lParam, ComponentEvent::MouseButtonType::RIGHT, ComponentEvent::MouseButtonAction::UP);
	}

	LRESULT WinAPIWindow::wmRButtonDown(WPARAM wParam, LPARAM lParam)
	{
		DCONSOLE(L"Right button down: (" << GET_X_LPARAM(lParam) << L',' << GET_Y_LPARAM(lParam) << L")\n");

		return wmMouseButton(wParam, lParam, ComponentEvent::MouseButtonType::RIGHT, ComponentEvent::MouseButtonAction::DOWN);
	}

	LRESULT WinAPIWindow::wmMButtonUp(WPARAM wParam, LPARAM lParam)
	{
		return wmMouseButton(wParam, lParam, ComponentEvent::MouseButtonType::MIDDLE, ComponentEvent::MouseButtonAction::UP);
	}

	LRESULT WinAPIWindow::wmMButtonDown(WPARAM wParam, LPARAM lParam)
	{
		return wmMouseButton(wParam, lParam, ComponentEvent::MouseButtonType::MIDDLE, ComponentEvent::MouseButtonAction::DOWN);
	}

	LRESULT WinAPIWindow::wmXButtonUp(WPARAM wParam, LPARAM lParam)
	{
		ComponentEvent::MouseButtonType mb;
		auto xButton = GET_XBUTTON_WPARAM(wParam);

		if (xButton & XBUTTON1) mb = ComponentEvent::MouseButtonType::XBUTTON;
		if (xButton & XBUTTON2) mb = ComponentEvent::MouseButtonType::YBUTTON;

		wmMouseButton(wParam, lParam, mb, ComponentEvent::MouseButtonAction::UP);

		return TRUE;
	}

	LRESULT WinAPIWindow::wmXButtonDown(WPARAM wParam, LPARAM lParam)
	{
		ComponentEvent::MouseButtonType mb;
		auto xButton = GET_XBUTTON_WPARAM(wParam);

		if (xButton & XBUTTON1) mb = ComponentEvent::MouseButtonType::XBUTTON;
		if (xButton & XBUTTON2) mb = ComponentEvent::MouseButtonType::YBUTTON;

		wmMouseButton(wParam, lParam, mb, ComponentEvent::MouseButtonAction::DOWN);

		return TRUE;
	}

	LRESULT WinAPIWindow::wmMouseButton(WPARAM wParam, LPARAM lParam, ComponentEvent::MouseButtonType mbt, ComponentEvent::MouseButtonAction mba)
	{
		auto [modKeys, pressedMouseKeys] = parseMouseKeyState(GET_KEYSTATE_WPARAM(wParam));

		ComponentEvent::MouseButtonEvent e = {};
		e.point = { GET_X_LPARAM(lParam) ,  GET_Y_LPARAM(lParam) };
		e.buttonType = mbt;
		e.action = mba;
		e.modificationKeys = modKeys;
		e.pressedMouseButtons = pressedMouseKeys;

		windowController.handleEvent(e);

		return 0;
	}

	LRESULT WinAPIWindow::wmMouseWheel(WPARAM wParam, LPARAM lParam)
	{
		auto wheelDelta = GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;
		auto [modKeys, mouseButtons] = parseMouseKeyState(GET_KEYSTATE_WPARAM(wParam));

		ComponentEvent::MouseWheelEvent e = {};
		e.point = { GET_X_LPARAM(lParam) ,  GET_Y_LPARAM(lParam) };
		e.direction = wheelDelta > 0  ? ComponentEvent::ScrollDirection::UP: ComponentEvent::ScrollDirection::DOWN;
		e.amountScrolled = std::abs(wheelDelta);
		e.modificationKeys = modKeys;
		e.pressedMouseButtons = mouseButtons;

		windowController.handleEvent(e);

		return 0;
	}

	LRESULT WinAPIWindow::wmUnichar(WPARAM wParam, LPARAM lParam)
	{
		return DefWindowProc(hWnd, WM_UNICHAR, wParam, lParam);
	}

	LRESULT WinAPIWindow::wmChar(WPARAM wParam, LPARAM lParam)
	{
		ComponentEvent::InputEvent e = {};
		e.character = wParam;

		windowController.handleEvent(e);

		return 0;
	}

	LRESULT WinAPIWindow::wmKeyUp(WPARAM wParam, LPARAM lParam)
	{
		using namespace ComponentEvent;

		KeyEvent e = {};
		e.action = KeyEventAction::UP;
		e.keyCode = wParam;

		windowController.handleEvent(e);

		return 0;
	}

	LRESULT WinAPIWindow::wmKeyDown(WPARAM wParam, LPARAM lParam)
	{
		using namespace ComponentEvent;

		KeyEvent e = {};
		e.action = KeyEventAction::DOWN;
		e.keyCode = wParam;

		windowController.handleEvent(e);

		return 0;
	}

	LRESULT WinAPIWindow::wmClose(WPARAM wParam, LPARAM lParam)
	{
		PostQuitMessage(0);
		return 0;
	}

	LRESULT WinAPIWindow::wmPaint(WPARAM wParam, LPARAM lParam)
	{
		PAINTSTRUCT ps = {};
		HDC hdc = BeginPaint(hWnd, &ps);

		auto [width, height] = dimensions;

		HDC memHdc = CreateCompatibleDC(hdc);
		HBITMAP hBitmap = CreateCompatibleBitmap(hdc, width, height);
		SelectObject(memHdc, hBitmap);

		for (const auto& dc : drawQueue)
			dc(memHdc);

		drawQueue.clear();

		BitBlt(hdc, 0, 0, width, height, memHdc, 0, 0, SRCCOPY);
		EndPaint(hWnd, &ps);

		DeleteObject(memHdc);
		DeleteObject(hBitmap);

		return 0;
	}

	LRESULT WinAPIWindow::wmSizingMoving(WPARAM wParam, LPARAM lParam)
	{
		PRECT pRect = reinterpret_cast<PRECT>(lParam);

		anchorPoint = { pRect->left, pRect->top };
		dimensions = { pRect->right - pRect->left, pRect->bottom - pRect->top };

		// To fix Microsoft bug with WS_POPUP window when changing size
		cachedUserDimensions = dimensions - resizeBorderWidth;

		return TRUE;
	}

	LRESULT WinAPIWindow::wmSize(WPARAM wParam, LPARAM lParam)
	{
		dimensions = { LOWORD(lParam), HIWORD(lParam) };

		// To fix Microsoft bug with WS_POPUP window when changing size
		cachedUserDimensions = dimensions - resizeBorderWidth;

		return 0;
	}

	LRESULT WinAPIWindow::wmMove(WPARAM wParam, LPARAM lParam)
	{
		anchorPoint = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
		return 0;
	}

	LRESULT WinAPIWindow::wmEraseBackground(WPARAM wParam, LPARAM lParam)
	{
		return TRUE;
	}

	LRESULT WinAPIWindow::wmNCHitTest(WPARAM wParam, LPARAM lParam)
	{
		WindowController::Point p{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};

		if (resizeBorderWidth)
		{
			auto [width, height] = dimensions;
			auto [ax, ay] = anchorPoint;

			int x = p.x - ax;
			int y = p.y - ay;

			bool left = x < resizeBorderWidth;
			bool right = x >= (width - resizeBorderWidth);
			bool top = y < resizeBorderWidth;
			bool bottom = y >= (height - resizeBorderWidth);
			
			if (top && left) return HTTOPLEFT;
			if (top && right) return HTTOPRIGHT;
			if (bottom && left) return HTBOTTOMLEFT;
			if (bottom && right) return HTBOTTOMRIGHT;
			if (top) return HTTOP;
			if (bottom) return HTBOTTOM;
			if (left) return HTLEFT;
			if (right) return HTRIGHT;
		}

		if (windowController.testMoveCapture(p))
			return HTCAPTION;

		return HTCLIENT;
	}

	
	LRESULT WinAPIWindow::wmNCCalcSize(WPARAM wParam, LPARAM lParam)
	{
		// To fix Microsoft bug with WS_POPUP window when changing size
		if (wParam && resizeBorderWidth)
		{
			NCCALCSIZE_PARAMS* params = reinterpret_cast<NCCALCSIZE_PARAMS*>(lParam);
			params->rgrc[0].bottom += resizeBorderWidth;
			params->rgrc[0].right += resizeBorderWidth;
			return 0;
		}
		return DefWindowProc(hWnd, WM_NCCALCSIZE, wParam, lParam);
	}

	LRESULT WinAPIWindow::wmCreate(WPARAM wParam, LPARAM lParam)
	{
		// To fix Microsoft bug with WS_POPUP window when changing size

		CREATESTRUCT* windowInfo = reinterpret_cast<CREATESTRUCT*>(lParam);
		expect(MoveWindow(hWnd, windowInfo->x, windowInfo->y, windowInfo->cx - resizeBorderWidth, windowInfo->cy - resizeBorderWidth, TRUE));

		return DefWindowProc(hWnd, WM_CREATE, wParam, lParam);
	}

	std::pair<std::set<ComponentEvent::ModificationKey>, std::set<ComponentEvent::MouseButtonType>> WinAPIWindow::parseMouseKeyState(WORD keyState)
	{
		std::set<ComponentEvent::ModificationKey> modificationKeys;
		if (keyState & MK_CONTROL) modificationKeys.insert(ComponentEvent::ModificationKey::CTRL);
		if (keyState & MK_SHIFT) modificationKeys.insert(ComponentEvent::ModificationKey::SHIFT);


		std::set<ComponentEvent::MouseButtonType> pressedMouseButtons;
		if (keyState & MK_LBUTTON) pressedMouseButtons.insert(ComponentEvent::MouseButtonType::LEFT);
		if (keyState & MK_RBUTTON) pressedMouseButtons.insert(ComponentEvent::MouseButtonType::RIGHT);
		if (keyState & MK_MBUTTON) pressedMouseButtons.insert(ComponentEvent::MouseButtonType::MIDDLE);
		if (keyState & MK_XBUTTON1) pressedMouseButtons.insert(ComponentEvent::MouseButtonType::XBUTTON);
		if (keyState & MK_XBUTTON2) pressedMouseButtons.insert(ComponentEvent::MouseButtonType::YBUTTON);

		return { modificationKeys, pressedMouseButtons };
	}

	void WinAPIWindow::moveWindow(const Point& newAnchorPoint)
	{
		// To fix Microsoft bug with WS_POPUP window when changing size
		expect(MoveWindow(hWnd, newAnchorPoint.x, newAnchorPoint.y, dimensions.width - resizeBorderWidth, dimensions.height - resizeBorderWidth, TRUE));
	}

	void WinAPIWindow::redraw()
	{
		expect(RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE | RDW_NOERASE | RDW_INTERNALPAINT));
	}

}