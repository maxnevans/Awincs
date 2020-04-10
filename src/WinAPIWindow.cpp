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
		expect(!registered);

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
		cuDimensions({ dims, DEFAULT_MAXIMUM_DIMENSIONS - DEFAULT_RESIZE_BORDER_WIDTH, DEFAULT_MINIMUM_DIMENSIONS - DEFAULT_RESIZE_BORDER_WIDTH }),
		dimensions({ dims + DEFAULT_RESIZE_BORDER_WIDTH,  DEFAULT_MAXIMUM_DIMENSIONS, DEFAULT_MINIMUM_DIMENSIONS }),
		windowController(windowController)
	{
	}

	WinAPIWindow::WinAPIWindow(WindowController& window)
		:WinAPIWindow(window, DEFAULT_WINDOW_ANCHOR_POINT, DEFAULT_WINDOW_DIMENSIONS)
	{
	}

	WinAPIWindow::~WinAPIWindow()
	{
		DestroyWindow(hWnd);
	}

	void WinAPIWindow::create()
	{
		expect(registerer.registered);

		auto [x, y] = this->anchorPoint;
		auto [width, height] = this->dimensions.normal;

		CreateWindowW(WINDOW_CLASS_NAME.data(), windowTitle.c_str(), WS_POPUP,
			x, y, width, height, NULL, NULL, hInstance, this);

		if (auto errorCode = GetLastError(); errorCode != ERROR_SUCCESS)
		{
			throw WindowException(L"Failed to create window! Error code: " + std::to_wstring(errorCode));
		}

		ShowWindow(hWnd, SW_HIDE);
		UpdateWindow(hWnd);
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
		auto ret = SetWindowPos(hWnd, NULL, point.x, point.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		expect(ret == TRUE);
	}

	const WinAPIWindow::Point& WinAPIWindow::getAnchorPoint() const
	{
		return anchorPoint;
	}

	void WinAPIWindow::setDimensions(Dimensions dims)
	{
		// To fix Microsoft bug with WS_POPUP window when changing size
		dimensions.normal = dims + resizeBorderWidth;
		cuDimensions.normal = dims;
		auto ret = SetWindowPos(hWnd, NULL, 0, 0, dims.width, dims.height, SWP_NOMOVE | SWP_NOZORDER);
		expect(ret == TRUE);
	}

	const WinAPIWindow::Dimensions& WinAPIWindow::getDimensions() const
	{
		return cuDimensions.normal;
	}

	void WinAPIWindow::draw(DrawCallback cb)
	{
		drawQueue.emplace_back(std::move(cb));
		auto ret = InvalidateRect(hWnd, NULL, FALSE);
		expect(ret == TRUE);
	}

	void WinAPIWindow::setResizeBorderWidth(int width)
	{
		expect(width >= 0);

		// To fix Microsoft bug with WS_POPUP window when changing size
		dimensions.normal = cuDimensions.normal + width;

		resizeBorderWidth = width;
	}

	void WinAPIWindow::setMinDimensions(const Dimensions& dim)
	{
		// To fix Microsoft bug with WS_POPUP window when changing size
		dimensions.min = dim + resizeBorderWidth;
		cuDimensions.min = dim;

		// Apply dimensions
		moveWindow(anchorPoint);
	}

	void WinAPIWindow::setMaxDimensions(const Dimensions& dim)
	{
		// To fix Microsoft bug with WS_POPUP window when changing size
		dimensions.max = dim + resizeBorderWidth;
		cuDimensions.max = dim;

		// Apply dimensions
		moveWindow(anchorPoint);
	}

	const WinAPIWindow::Dimensions& WinAPIWindow::getMinDimensions()
	{
		return dimensions.min;
	}

	const WinAPIWindow::Dimensions& WinAPIWindow::getMaxDimensions()
	{
		return dimensions.max;
	}

	LRESULT CALLBACK WinAPIWindow::setupWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if (uMsg == WM_CREATE)
			throw WindowException(L"Failed to setup proxyWindowProc! WM_NCCREATE has not been called!");

		if (uMsg != WM_NCCREATE)
			return DefWindowProc(hWnd, uMsg, wParam, lParam);

		CREATESTRUCT* createStruct = reinterpret_cast<CREATESTRUCT*>(lParam);
		WinAPIWindow* const window = reinterpret_cast<WinAPIWindow* const>(createStruct->lpCreateParams);

		SetLastError(ERROR_SUCCESS);

		window->hWnd = hWnd;

		if (SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window)) == FALSE && GetLastError() != ERROR_SUCCESS)
			throw WindowException(L"Failed to set GWLP_USERDATA when handling setupWindowProc");

		if (SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WinAPIWindow::proxyWindowProc)) == FALSE && GetLastError() != ERROR_SUCCESS)
			throw WindowException(L"Failed to set GWLP_WNDPROC when handling setupWindowProc with proxyWindowProc");

		return window->windowProc(uMsg, wParam, lParam);
	}

	LRESULT CALLBACK WinAPIWindow::proxyWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
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
		case WM_SIZING: return wmSizing(wParam, lParam);
		case WM_MOVING: return wmMoving(wParam, lParam);
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
		case WM_GETMINMAXINFO: return wmGetMinMaxInfo(wParam, lParam);
		}

		return DefWindowProcW(hWnd, uMsg, wParam, lParam);
	}

	LRESULT WinAPIWindow::wmLButtonUp(WPARAM wParam, LPARAM lParam)
	{
		DCONSOLE(L"Left button up: (" << GET_X_LPARAM(lParam) << L',' << GET_Y_LPARAM(lParam) << L")\n");

		return wmMouseButton(wParam, lParam, ComponentEvent::Mouse::ButtonType::LEFT, ComponentEvent::Mouse::ButtonAction::UP);
	}

	LRESULT WinAPIWindow::wmLButtonDown(WPARAM wParam, LPARAM lParam)
	{
		DCONSOLE(L"Left button down: (" << GET_X_LPARAM(lParam) << L',' << GET_Y_LPARAM(lParam) << L")\n");

		return wmMouseButton(wParam, lParam, ComponentEvent::Mouse::ButtonType::LEFT, ComponentEvent::Mouse::ButtonAction::DOWN);
	}

	LRESULT WinAPIWindow::wmRButtonUp(WPARAM wParam, LPARAM lParam)
	{
		DCONSOLE(L"Right button up: (" << GET_X_LPARAM(lParam) << L',' << GET_Y_LPARAM(lParam) << L")\n");

		return wmMouseButton(wParam, lParam, ComponentEvent::Mouse::ButtonType::RIGHT, ComponentEvent::Mouse::ButtonAction::UP);
	}

	LRESULT WinAPIWindow::wmRButtonDown(WPARAM wParam, LPARAM lParam)
	{
		DCONSOLE(L"Right button down: (" << GET_X_LPARAM(lParam) << L',' << GET_Y_LPARAM(lParam) << L")\n");

		return wmMouseButton(wParam, lParam, ComponentEvent::Mouse::ButtonType::RIGHT, ComponentEvent::Mouse::ButtonAction::DOWN);
	}

	LRESULT WinAPIWindow::wmMButtonUp(WPARAM wParam, LPARAM lParam)
	{
		return wmMouseButton(wParam, lParam, ComponentEvent::Mouse::ButtonType::MIDDLE, ComponentEvent::Mouse::ButtonAction::UP);
	}

	LRESULT WinAPIWindow::wmMButtonDown(WPARAM wParam, LPARAM lParam)
	{
		return wmMouseButton(wParam, lParam, ComponentEvent::Mouse::ButtonType::MIDDLE, ComponentEvent::Mouse::ButtonAction::DOWN);
	}

	LRESULT WinAPIWindow::wmXButtonUp(WPARAM wParam, LPARAM lParam)
	{
		ComponentEvent::Mouse::ButtonType mb;
		auto xButton = GET_XBUTTON_WPARAM(wParam);

		if (xButton & XBUTTON1) mb = ComponentEvent::Mouse::ButtonType::XBUTTON;
		if (xButton & XBUTTON2) mb = ComponentEvent::Mouse::ButtonType::YBUTTON;

		wmMouseButton(wParam, lParam, mb, ComponentEvent::Mouse::ButtonAction::UP);

		return TRUE;
	}

	LRESULT WinAPIWindow::wmXButtonDown(WPARAM wParam, LPARAM lParam)
	{
		ComponentEvent::Mouse::ButtonType mb;
		auto xButton = GET_XBUTTON_WPARAM(wParam);

		if (xButton & XBUTTON1) mb = ComponentEvent::Mouse::ButtonType::XBUTTON;
		if (xButton & XBUTTON2) mb = ComponentEvent::Mouse::ButtonType::YBUTTON;

		wmMouseButton(wParam, lParam, mb, ComponentEvent::Mouse::ButtonAction::DOWN);

		return TRUE;
	}

	LRESULT WinAPIWindow::wmMouseButton(WPARAM wParam, LPARAM lParam, ComponentEvent::Mouse::ButtonType mbt, ComponentEvent::Mouse::ButtonAction mba)
	{
		auto [modKeys, pressedMouseKeys] = parseMouseKeyState(GET_KEYSTATE_WPARAM(wParam));

		ComponentEvent::Mouse::ButtonEvent e = {};
		e.point = { GET_X_LPARAM(lParam) ,  GET_Y_LPARAM(lParam) };
		e.buttonType = mbt;
		e.action = mba;
		e.modificationKeys = modKeys;
		e.pressedMouseButtons = pressedMouseKeys;

		static_cast<ComponentEvent::Handler&>(windowController).handleEvent(e);

		return 0;
	}

	LRESULT WinAPIWindow::wmMouseWheel(WPARAM wParam, LPARAM lParam)
	{
		auto wheelDelta = GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;
		auto [modKeys, mouseButtons] = parseMouseKeyState(GET_KEYSTATE_WPARAM(wParam));

		ComponentEvent::Mouse::WheelEvent e = {};
		e.point = { GET_X_LPARAM(lParam) ,  GET_Y_LPARAM(lParam) };
		e.direction = wheelDelta > 0  ? ComponentEvent::Mouse::ScrollDirection::UP: ComponentEvent::Mouse::ScrollDirection::DOWN;
		e.amountScrolled = std::abs(wheelDelta);
		e.modificationKeys = modKeys;
		e.pressedMouseButtons = mouseButtons;

		static_cast<ComponentEvent::Handler&>(windowController).handleEvent(e);

		return 0;
	}

	LRESULT WinAPIWindow::wmUnichar(WPARAM wParam, LPARAM lParam)
	{
		return DefWindowProc(hWnd, WM_UNICHAR, wParam, lParam);
	}

	LRESULT WinAPIWindow::wmChar(WPARAM wParam, LPARAM lParam)
	{
		ComponentEvent::Keyboard::InputEvent e = {};
		e.character = static_cast<wchar_t>(wParam);

		static_cast<ComponentEvent::Handler&>(windowController).handleEvent(e);

		return 0;
	}

	LRESULT WinAPIWindow::wmKeyUp(WPARAM wParam, LPARAM lParam)
	{
		ComponentEvent::Keyboard::KeyEvent e = {};
		e.action = ComponentEvent::Keyboard::KeyEventAction::UP;
		e.keyCode = static_cast<int>(wParam);

		static_cast<ComponentEvent::Handler&>(windowController).handleEvent(e);

		return 0;
	}

	LRESULT WinAPIWindow::wmKeyDown(WPARAM wParam, LPARAM lParam)
	{
		ComponentEvent::Keyboard::KeyEvent e = {};
		e.action = ComponentEvent::Keyboard::KeyEventAction::DOWN;
		e.keyCode = static_cast<int>(wParam);

		static_cast<ComponentEvent::Handler&>(windowController).handleEvent(e);

		return 0;
	}

	LRESULT WinAPIWindow::wmClose(WPARAM wParam, LPARAM lParam)
	{
		static_cast<ComponentEvent::Handler&>(windowController).handleEvent(ComponentEvent::Window::CloseEvent{});

		PostQuitMessage(0);
		return 0;
	}

	LRESULT WinAPIWindow::wmPaint(WPARAM wParam, LPARAM lParam)
	{
		PAINTSTRUCT ps = {};
		HDC hdc = BeginPaint(hWnd, &ps);

		auto [width, height] = dimensions.normal;

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

	LRESULT WinAPIWindow::wmSizing(WPARAM wParam, LPARAM lParam)
	{
		PRECT pRect = reinterpret_cast<PRECT>(lParam);

		anchorPoint = { pRect->left, pRect->top };
		
		// To fix Microsoft bug with WS_POPUP window when changing size
		cuDimensions.normal = { pRect->right - pRect->left, pRect->bottom - pRect->top };
		dimensions.normal = cuDimensions.normal + resizeBorderWidth;

		windowController.handleEvent(ComponentEvent::Window::ResizeEvent{});

		return TRUE;
	}

	LRESULT WinAPIWindow::wmMoving(WPARAM wParam, LPARAM lParam)
	{
		PRECT pRect = reinterpret_cast<PRECT>(lParam);

		anchorPoint = { pRect->left, pRect->top };

		// To fix Microsoft bug with WS_POPUP window when changing size
		cuDimensions.normal = { pRect->right - pRect->left, pRect->bottom - pRect->top };
		dimensions.normal = cuDimensions.normal + resizeBorderWidth;

		windowController.handleEvent(ComponentEvent::Window::MoveEvent{});

		return TRUE;
	}

	LRESULT WinAPIWindow::wmSize(WPARAM wParam, LPARAM lParam)
	{
		dimensions.normal = { LOWORD(lParam), HIWORD(lParam) };

		// To fix Microsoft bug with WS_POPUP window when changing size
		cuDimensions.normal = dimensions.normal - resizeBorderWidth;

		auto prevWindowState = windowState;
		changeWindowState(wParam);
		handleWindowStateEvent(prevWindowState, windowState);

		return 0;
	}

	LRESULT WinAPIWindow::wmMove(WPARAM wParam, LPARAM lParam)
	{
		anchorPoint = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };

		windowController.handleEvent(ComponentEvent::Window::MoveEvent{});

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
			auto [width, height] = dimensions.normal - resizeBorderWidth;
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
		auto ret = MoveWindow(hWnd, windowInfo->x, windowInfo->y, windowInfo->cx - resizeBorderWidth, windowInfo->cy - resizeBorderWidth, TRUE);
		expect(ret);

		return DefWindowProc(hWnd, WM_CREATE, wParam, lParam);
	}

	LRESULT WinAPIWindow::wmGetMinMaxInfo(WPARAM wParam, LPARAM lParam)
	{
		MINMAXINFO* minMax = reinterpret_cast<MINMAXINFO*>(lParam);

		if (dimensions.min.width != ZERO_DIMENSIONS.width) minMax->ptMinTrackSize.x = dimensions.min.width;
		if (dimensions.min.height != ZERO_DIMENSIONS.height) minMax->ptMinTrackSize.y = dimensions.min.height;
		if (dimensions.max.width != ZERO_DIMENSIONS.width) minMax->ptMaxTrackSize.x = dimensions.max.width;
		if (dimensions.max.height != ZERO_DIMENSIONS.height) minMax->ptMaxTrackSize.y = dimensions.max.height;

		return DefWindowProc(hWnd, WM_GETMINMAXINFO, wParam, lParam);
	}

	std::pair<std::set<ComponentEvent::Keyboard::ModificationKey>, std::set<ComponentEvent::Mouse::ButtonType>> WinAPIWindow::parseMouseKeyState(WORD keyState)
	{
		std::set<ComponentEvent::Keyboard::ModificationKey> modificationKeys;
		if (keyState & MK_CONTROL) modificationKeys.insert(ComponentEvent::Keyboard::ModificationKey::CTRL);
		if (keyState & MK_SHIFT) modificationKeys.insert(ComponentEvent::Keyboard::ModificationKey::SHIFT);


		std::set<ComponentEvent::Mouse::ButtonType> pressedMouseButtons;
		if (keyState & MK_LBUTTON) pressedMouseButtons.insert(ComponentEvent::Mouse::ButtonType::LEFT);
		if (keyState & MK_RBUTTON) pressedMouseButtons.insert(ComponentEvent::Mouse::ButtonType::RIGHT);
		if (keyState & MK_MBUTTON) pressedMouseButtons.insert(ComponentEvent::Mouse::ButtonType::MIDDLE);
		if (keyState & MK_XBUTTON1) pressedMouseButtons.insert(ComponentEvent::Mouse::ButtonType::XBUTTON);
		if (keyState & MK_XBUTTON2) pressedMouseButtons.insert(ComponentEvent::Mouse::ButtonType::YBUTTON);

		return { modificationKeys, pressedMouseButtons };
	}

	void WinAPIWindow::moveWindow(const Point& newAnchorPoint)
	{
		auto [width, height] = dimensions.normal;

		// To fix Microsoft bug with WS_POPUP window when changing size
		auto ret = MoveWindow(hWnd, newAnchorPoint.x, newAnchorPoint.y, width - resizeBorderWidth, height - resizeBorderWidth, TRUE);
		expect(ret);
	}

	void WinAPIWindow::redraw()
	{
		auto ret = RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE | RDW_NOERASE | RDW_INTERNALPAINT);
		expect(ret);
	}

	void WinAPIWindow::changeWindowState(WPARAM wParam)
	{
		switch (wParam)
		{
		case SIZE_MAXIMIZED: windowState = WindowState::MAXIMIZED; break;
		case SIZE_MINIMIZED: windowState = WindowState::MINIMIZED; break;
		case SIZE_RESTORED: windowState = WindowState::NORMAL; break;
		case SIZE_MAXHIDE: break;
		case SIZE_MAXSHOW: break;
		default: expect(false);
		}
	}

	void WinAPIWindow::handleWindowStateEvent(WindowState prev, WindowState current)
	{
		if ((prev == WindowState::MAXIMIZED || prev == WindowState::MINIMIZED) && current == WindowState::NORMAL)
			static_cast<ComponentEvent::Handler&>(windowController).handleEvent(ComponentEvent::Window::RestoreEvent{});
		else if (current == WindowState::NORMAL)
			static_cast<ComponentEvent::Handler&>(windowController).handleEvent(ComponentEvent::Window::ResizeEvent{});
		else if (current == WindowState::MINIMIZED)
			static_cast<ComponentEvent::Handler&>(windowController).handleEvent(ComponentEvent::Window::MinimizeEvent{});
		else if (current == WindowState::MAXIMIZED)
			static_cast<ComponentEvent::Handler&>(windowController).handleEvent(ComponentEvent::Window::MaximizeEvent{});
	}

}