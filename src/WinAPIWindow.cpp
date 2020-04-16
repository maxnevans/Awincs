#include "pch.h"
#include "WinAPIWindow.h"

#include <DebugConsole.h>
#include <dwmapi.h>
#include <versionhelpers.h>
#include <uxtheme.h>

#include "base/DebugUntils.h"
#include "WindowException.h"

#ifndef WM_NCUAHDRAWCAPTION
#define WM_NCUAHDRAWCAPTION (0x00AE)
#endif
#ifndef WM_NCUAHDRAWFRAME
#define WM_NCUAHDRAWFRAME (0x00AF)
#endif


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
		wndClass.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);

		if (RegisterClassEx(&wndClass) == FALSE)
		{
			auto errorCode = std::to_wstring(GetLastError());

			throw WindowException(L"Failed to register class! Error code: " + errorCode);
		}

		registered = true;
	}

	WinAPIWindow::WinAPIWindowRegisterer::~WinAPIWindowRegisterer()
	{
		UnregisterClass(WINDOW_CLASS_NAME.data(), GetModuleHandle(NULL));
	}

	WinAPIWindow::WinAPIWindow(WindowController& windowController, const Point& anchorPoint, const Dimensions& dims)
		:
		anchorPoint(anchorPoint),
		dimensions({ dims, DEFAULT_MAXIMUM_DIMENSIONS, DEFAULT_MINIMUM_DIMENSIONS }),
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

	void WinAPIWindow::updateRegion()
	{
		/* Optimization puprose only: this is not required (we can just GetWindowInfo()
		   to retrieve again previous region. This field is static right here to do not mess with it
		   outside this function. */
		static RECT rgn = {};
		RECT oldRgn = rgn;

		/* Calculating new region */
		if (IsMaximized(hWnd)) {
			WINDOWINFO wi = {};
			wi.cbSize = sizeof(wi);
			GetWindowInfo(hWnd, &wi);

			/* For maximized windows, a region is needed to cut off the non-client
			   borders that hang over the edge of the screen */
			rgn = {
				wi.rcClient.left - wi.rcWindow.left, //left
				wi.rcClient.top - wi.rcWindow.top, // top
				wi.rcClient.right - wi.rcWindow.left, // right
				wi.rcClient.bottom - wi.rcWindow.top //bottom
			};
		}
		else if (!compositionEnabled) {
			/* For ordinary themed windows when composition is disabled, a region
			   is needed to remove the rounded top corners. Make it as large as
			   possible to avoid having to change it when the window is resized. */
			rgn = {
				0, 0, // top left
				32767, 32767 // right bottom
			};
		}
		else {
			/* Don't mess with the region when composition is enabled and the
			   window is not maximized, otherwise it will lose its shadow */
			rgn = {};
		}

		/* Avoid unnecessarily updating the region to avoid unnecessary redraws */
		if (EqualRect(&rgn, &oldRgn))
			return;

		/* Treat empty regions as NULL regions */
		RECT r = { 0 };
		if (EqualRect(&rgn, &r))
			SetWindowRgn(hWnd, NULL, TRUE);
		else
			SetWindowRgn(hWnd, CreateRectRgnIndirect(&rgn), TRUE);
	}

	bool WinAPIWindow::hasAutohideAppbar(UINT edge, const RECT& mon)
	{

		if (IsWindows8Point1OrGreater()) {
			APPBARDATA aB = { 0 };
			aB.cbSize = sizeof(APPBARDATA);
			aB.uEdge = edge;
			aB.rc = mon;

			return SHAppBarMessage(ABM_GETAUTOHIDEBAREX, &aB);
		}

		/* Before Windows 8.1, it was not possible to specify a monitor when
			checking for hidden appbars, so check only on the primary monitor */
		if (mon.left != 0 || mon.top != 0)
			return false;

		APPBARDATA aB = {};
		aB.cbSize = sizeof(aB);
		aB.uEdge = edge;
		return SHAppBarMessage(ABM_GETAUTOHIDEBAR, &aB);
	}

	void WinAPIWindow::setupDWM()
	{
		DwmIsCompositionEnabled(&reinterpret_cast<BOOL&>(compositionEnabled));

		if (compositionEnabled) {
			/* Deleting all frames */
			MARGINS m = { 0,0,0,0 };
			DWORD attr = DWMNCRP_ENABLED;
			DwmExtendFrameIntoClientArea(hWnd, &m);
			DwmSetWindowAttribute(hWnd, DWMWA_NCRENDERING_POLICY, &attr, sizeof(DWORD));
		}
	}

	void WinAPIWindow::processMessages()
	{
		MSG msg;
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	void WinAPIWindow::create()
	{
		expect(registerer.registered);

		auto [x, y] = this->anchorPoint;
		auto [width, height] = this->dimensions.normal;

		CreateWindowEx(WS_EX_APPWINDOW, WINDOW_CLASS_NAME.data(), 
			windowTitle.c_str(), WS_OVERLAPPEDWINDOW | WS_SIZEBOX,
			x, y, width, height, NULL, NULL, hInstance, this);

		if (auto errorCode = GetLastError(); errorCode != ERROR_SUCCESS)
			throw WindowException(L"Failed to create window! Error code: " + std::to_wstring(errorCode));

		setupDWM();
		updateRegion();
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

	void WinAPIWindow::setAnchorPoint(const Point& point)
	{
		p_smartMove(anchorPoint, point);
		anchorPoint = point;
	}

	const WinAPIWindow::Point& WinAPIWindow::getAnchorPoint() const
	{
		return anchorPoint;
	}

	void WinAPIWindow::setDimensions(const Dimensions& dims)
	{
		p_smartResize(dimensions.normal, dims);
		dimensions.normal = dims;
	}

	const WinAPIWindow::Dimensions& WinAPIWindow::getDimensions() const
	{
		return dimensions.normal;
	}

	void WinAPIWindow::redraw()
	{
		auto ret = InvalidateRect(hWnd, NULL, FALSE);
		expect(ret == TRUE);
	}

	void WinAPIWindow::setDrawCallback(DrawCallback cb)
	{
		drawCallback = std::move(cb);
	}

	void WinAPIWindow::setResizeBorderWidth(int width)
	{
		expect(width >= 0);
		resizeBorderWidth = width;
	}

	void WinAPIWindow::setMinDimensions(const Dimensions& dim)
	{
		dimensions.min = dim;

		// Apply dimensions
		p_update();
	}

	void WinAPIWindow::setMaxDimensions(const Dimensions& dim)
	{
		dimensions.max = dim;

		// Apply dimensions
		p_update();
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
		case WM_MOUSEMOVE: return wmMouseMove(wParam, lParam);
		case WM_NCMOUSEMOVE: return wmNCMouseMove(wParam, lParam);
		case WM_MOUSELEAVE: return wmMouseLeave(wParam, lParam);
		case WM_MOUSEACTIVATE: return wmMouseActivate(wParam, lParam);
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
		case WM_SETICON: 
		case WM_SETTEXT: return wmSetTextAndSetIcon(uMsg, wParam, lParam);
		case WM_THEMECHANGED: return wmThemeChanged(wParam, lParam);
		case WM_WINDOWPOSCHANGED: return wmWindowPosChanged(wParam, lParam);
		case WM_WINDOWPOSCHANGING: return wmWindowPosChanging(wParam, lParam);
		case WM_NCUAHDRAWCAPTION:
		case WM_NCUAHDRAWFRAME: return wmNCUAHDrawFrameAndCaption(wParam, lParam);
		case WM_NCPAINT: return wmNCPaint(wParam, lParam);
		case WM_NCACTIVATE: return wmNCActivate(wParam, lParam);
		case WM_DWMCOMPOSITIONCHANGED: return wmDWMCompositionChanged(wParam, lParam);
		}

		return DefWindowProc(hWnd, uMsg, wParam, lParam);
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
		ComponentEvent::Mouse::ButtonType mb = ComponentEvent::Mouse::ButtonType::UNKNOWN;
		auto xButton = GET_XBUTTON_WPARAM(wParam);

		if (xButton & XBUTTON1) mb = ComponentEvent::Mouse::ButtonType::XBUTTON;
		if (xButton & XBUTTON2) mb = ComponentEvent::Mouse::ButtonType::YBUTTON;
		expect(mb != ComponentEvent::Mouse::ButtonType::UNKNOWN);

		wmMouseButton(wParam, lParam, mb, ComponentEvent::Mouse::ButtonAction::UP);

		return TRUE;
	}

	LRESULT WinAPIWindow::wmXButtonDown(WPARAM wParam, LPARAM lParam)
	{
		ComponentEvent::Mouse::ButtonType mb = ComponentEvent::Mouse::ButtonType::UNKNOWN;
		auto xButton = GET_XBUTTON_WPARAM(wParam);

		if (xButton & XBUTTON1) mb = ComponentEvent::Mouse::ButtonType::XBUTTON;
		if (xButton & XBUTTON2) mb = ComponentEvent::Mouse::ButtonType::YBUTTON;
		expect(mb != ComponentEvent::Mouse::ButtonType::UNKNOWN);

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

	LRESULT WinAPIWindow::wmMouseMove(WPARAM wParam, LPARAM lParam)
	{
		auto [modKeys, mouseButtons] = parseMouseKeyState(GET_KEYSTATE_WPARAM(wParam));

		ComponentEvent::Mouse::Hover e = {};

		e.point = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
		e.modificationKeys = modKeys;
		e.pressedMouseButtons = mouseButtons;

		//DCONSOLE(L"WM_MOUSEMOVE: (" << e.point.x << L"," << e.point.y << L")\n");
		static_cast<ComponentEvent::Handler&>(windowController).handleEvent(e);
		return 0;
	}

	LRESULT WinAPIWindow::wmMouseActivate(WPARAM wParam, LPARAM lParam)
	{
		// TODO: Handle window activation by mouse
		return DefWindowProc(hWnd, WM_MOUSEACTIVATE, wParam, lParam);
	}

	LRESULT WinAPIWindow::wmMouseLeave(WPARAM wParam, LPARAM lParam)
	{
		static_cast<ComponentEvent::Handler&>(windowController).handleEvent(ComponentEvent::Mouse::HoverEnd{});
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

	LRESULT WinAPIWindow::wmNCMouseMove(WPARAM wParam, LPARAM lParam)
	{
		WORD leftButton = GetSystemMetrics(SM_SWAPBUTTON) ? MK_RBUTTON : MK_LBUTTON;
		WORD rightButton = leftButton == MK_LBUTTON ? MK_RBUTTON : MK_LBUTTON;

		WORD keyState = NULL;
		keyState = GetAsyncKeyState(VK_CONTROL) ? MK_CONTROL : NULL;
		keyState = GetAsyncKeyState(VK_SHIFT) ? MK_SHIFT : NULL;
		keyState = GetAsyncKeyState(VK_LBUTTON) ? leftButton : NULL;
		keyState = GetAsyncKeyState(VK_RBUTTON) ? rightButton : NULL;
		keyState = GetAsyncKeyState(VK_MBUTTON) ? MK_MBUTTON : NULL;
		keyState = GetAsyncKeyState(VK_XBUTTON1) ? MK_XBUTTON1 : NULL;
		keyState = GetAsyncKeyState(VK_XBUTTON2) ? MK_XBUTTON2 : NULL;

		auto [modKeys, mouseButtons] = parseMouseKeyState(keyState);

		ComponentEvent::Mouse::Hover e = {};

		Point mouseScreenPoint = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };

		e.point = mouseScreenPoint - anchorPoint;
		e.modificationKeys = modKeys;
		e.pressedMouseButtons = mouseButtons;

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

		BitBlt(memHdc, 0, 0, width, height, hdc, 0, 0, SRCCOPY);

		{
			/* Doc on Gdiplus::Graphics: should be deleted before device context */
			Gdiplus::Graphics gfx(memHdc);

			if (drawCallback)
				drawCallback(gfx);
		}

		BitBlt(hdc, 0, 0, width, height, memHdc, 0, 0, SRCCOPY);
		EndPaint(hWnd, &ps);

		DeleteObject(memHdc);
		DeleteObject(hBitmap);

		return 0;
	}

	LRESULT WinAPIWindow::wmSizing(WPARAM wParam, LPARAM lParam)
	{
		/*
			This message COULD be handled here. But you should NOT change WinAPIWindow::dimensions or WinAPIWindow::anchorPoint here.
			Note that WinAPIWindow::dimensions and WinAPIWindow::anchorPoint here is not updated yet.
			It is better to handle anything that is related to position, dimensions or window state inside
			WinAPIWindow::wmWindowPosChanged procedure;
		*/
		return DefWindowProc(hWnd, WM_SIZING, wParam, lParam);
	}

	LRESULT WinAPIWindow::wmMoving(WPARAM wParam, LPARAM lParam)
	{
		/*
			This message COULD be handled here. But you should NOT change WinAPIWindow::dimensions or WinAPIWindow::anchorPoint here.
			Note that WinAPIWindow::dimensions and WinAPIWindow::anchorPoint here is not updated yet.
			It is better to handle anything that is related to position, dimensions or window state inside
			WinAPIWindow::wmWindowPosChanged procedure;
		*/
		return DefWindowProc(hWnd, WM_MOVING, wParam, lParam);
	}

	LRESULT WinAPIWindow::wmSize(WPARAM wParam, LPARAM lParam)
	{
		/*
			This message will not be called due to WM_WINDOWPOSCHANGED is not calling DefWindowProc.
			Handle this inside wmWindowPosChanged.
		*/
		return DefWindowProc(hWnd, WM_SIZE, wParam, lParam);
	}

	LRESULT WinAPIWindow::wmMove(WPARAM wParam, LPARAM lParam)
	{
		/*
			This message will not be called due to WM_WINDOWPOSCHANGED is not calling DefWindowProc.
			Handle this inside wmWindowPosChanged.
		*/
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
			auto [width, height] = dimensions.normal;
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
		/* DefWindowProc must be called in both the maximized and non-maximized
		   cases, otherwise tile/cascade windows won't work */
		RECT* rect = reinterpret_cast<RECT*>(lParam);
		RECT nonClient = *reinterpret_cast<RECT*>(lParam);
		DefWindowProc(hWnd, WM_NCCALCSIZE, wParam, lParam);
		RECT client = *reinterpret_cast<RECT*>(lParam);

		if (IsMaximized(hWnd)) {
			WINDOWINFO wi = { sizeof wi }; // cbSize
			GetWindowInfo(hWnd, &wi);

			/* Maximized windows always have a non-client border that hangs over
			   the edge of the screen, so the size proposed by WM_NCCALCSIZE is
			   fine. Just adjust the top border to remove the window title. */
			*rect = RECT{
				client.left, // left
				nonClient.top + static_cast<LONG>(wi.cyWindowBorders), // top
				client.right, // right
				client.bottom, // bottom
			};

			HMONITOR mon = MonitorFromWindow(hWnd, MONITOR_DEFAULTTOPRIMARY);
			MONITORINFO mi = { sizeof(mi) }; // cbSize
			GetMonitorInfo(mon, &mi);

			/* If the client rectangle is the same as the monitor's rectangle,
			   the shell assumes that the window has gone fullscreen, so it removes
			   the topmost attribute from any auto-hide appbars, making them
			   inaccessible. To avoid this, reduce the size of the client area by
			   one pixel on a certain edge. The edge is chosen based on which side
			   of the monitor is likely to contain an auto-hide appbar, so the
			   missing client area is covered by it. */
			if (EqualRect(rect, &mi.rcMonitor)) {
				if (hasAutohideAppbar(ABE_BOTTOM, mi.rcMonitor))
					rect->bottom--;
				else if (hasAutohideAppbar(ABE_LEFT, mi.rcMonitor))
					rect->left++;
				else if (hasAutohideAppbar(ABE_TOP, mi.rcMonitor))
					rect->top++;
				else if (hasAutohideAppbar(ABE_RIGHT, mi.rcMonitor))
					rect->right--;
			}
		}
		else {
			/* For the non-maximized case, set the output RECT to what it was
			   before WM_NCCALCSIZE modified it. This will make the client size the
			   same as the non-client size. */
			*rect = nonClient;
		}

		return 0;
	}

	LRESULT WinAPIWindow::wmCreate(WPARAM wParam, LPARAM lParam)
	{
		return DefWindowProc(hWnd, WM_CREATE, wParam, lParam);
	}

	LRESULT WinAPIWindow::wmGetMinMaxInfo(WPARAM wParam, LPARAM lParam)
	{
		MINMAXINFO* minMax = reinterpret_cast<MINMAXINFO*>(lParam);

		if (dimensions.min.width) minMax->ptMinTrackSize.x = dimensions.min.width;
		if (dimensions.min.height) minMax->ptMinTrackSize.y = dimensions.min.height;
		if (dimensions.max.width) minMax->ptMaxTrackSize.x = dimensions.max.width;
		if (dimensions.max.height) minMax->ptMaxTrackSize.y = dimensions.max.height;

		return DefWindowProc(hWnd, WM_GETMINMAXINFO, wParam, lParam);
	}

	LRESULT WinAPIWindow::wmSetTextAndSetIcon(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		/* Disable painting while these messages are handled to prevent them
		   from drawing a window caption over the client area, but only when
		   composition and theming are disabled. These messages don't paint
		   when composition is enabled and blocking WM_NCUAHDRAWCAPTION should
		   be enough to prevent painting when theming is enabled. */
		if (compositionEnabled && themeEnabled)
		{
			LONG_PTR old_style = GetWindowLongPtr(hWnd, GWL_STYLE);

			/* Prevent Windows from drawing the default title bar by temporarily
			   toggling the WS_VISIBLE style. This is recommended in:
			   https://blogs.msdn.microsoft.com/wpfsdk/2008/09/08/custom-window-chrome-in-wpf/ */
			SetWindowLongPtr(hWnd, GWL_STYLE, old_style & ~WS_VISIBLE);
			LRESULT result = DefWindowProc(hWnd, uMsg, wParam, lParam);
			SetWindowLongPtr(hWnd, GWL_STYLE, old_style);

			return result;
		}
		
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	LRESULT WinAPIWindow::wmDWMCompositionChanged(WPARAM wParam, LPARAM lParam)
	{
		setupDWM();
		updateRegion();
		return 0;
	}

	LRESULT WinAPIWindow::wmNCActivate(WPARAM wParam, LPARAM lParam)
	{
		/* DefWindowProc won't repaint the window border if lParam (normally a
		   HRGN) is -1. This is recommended in:
		   https://blogs.msdn.microsoft.com/wpfsdk/2008/09/08/custom-window-chrome-in-wpf/ */
		return DefWindowProc(hWnd, WM_NCACTIVATE, wParam, -1);
	}

	LRESULT WinAPIWindow::wmNCPaint(WPARAM wParam, LPARAM lParam)
	{
		/* Only block WM_NCPAINT when composition is disabled. If it's blocked
		   when composition is enabled, the window shadow won't be drawn. */
		if (!compositionEnabled)
			return 0;

		return DefWindowProc(hWnd, WM_NCPAINT, wParam, lParam);
	}

	LRESULT WinAPIWindow::wmNCUAHDrawFrameAndCaption(WPARAM wParam, LPARAM lParam)
	{
		/* These undocumented messages are sent to draw themed window borders.
		   Block them to prevent drawing borders over the client area. */
		return 0;
	}

	LRESULT WinAPIWindow::wmThemeChanged(WPARAM wParam, LPARAM lParam)
	{
		themeEnabled = IsThemeActive();
		return DefWindowProc(hWnd, WM_THEMECHANGED, wParam, lParam);
	}

	LRESULT WinAPIWindow::wmWindowPosChanged(WPARAM wParam, LPARAM lParam)
	{
		/* WINDOWPOS contians entire window position and dimensions.
		   But we want a client because we process only client region. */
		RECT client = {};
		GetClientRect(hWnd, &client);

		/* Calculating new anchorPoint and dimensions */
		Dimensions newDimensions{ client.right , client.bottom};
		bool dimenionsChanged = dimensions.normal != newDimensions;

		WINDOWPOS* pos = reinterpret_cast<WINDOWPOS*>(lParam);

		Point newAnchorPoint = { pos->x, pos->y };
		bool anchorChanged = anchorPoint != newAnchorPoint;

		/* Window region correction */
		if (dimenionsChanged || (pos->flags & SWP_FRAMECHANGED))
			updateRegion();

		if (dimenionsChanged || anchorChanged)
		{
			/* Determine new current window state */
			WindowState wOldState = windowState;

			if (IsMaximized(hWnd))
				windowState = WindowState::MAXIMIZED;
			else if (IsMinimized(hWnd))
				windowState = WindowState::MINIMIZED;
			else 
				windowState = WindowState::NORMAL;

			/* Need to setup correct data before handling any event. */
			anchorPoint = newAnchorPoint;
			dimensions.normal = newDimensions;

			if (!handleWindowStateEvent(wOldState, windowState))
			{
				/* It is not a window state change. It's moving or/and resizing. */

				if (dimenionsChanged)
					windowController.handleEvent(ComponentEvent::Window::ResizeEvent{});

				if (anchorChanged)
					windowController.handleEvent(ComponentEvent::Window::MoveEvent{});
			}
		}

		/* 0 - processing this event */
		return 0;
	}

	LRESULT WinAPIWindow::wmWindowPosChanging(WPARAM wParam, LPARAM lParam)
	{
		/*
			This message COULD be handled here. But you should NOT change WinAPIWindow::dimensions or WinAPIWindow::anchorPoint here.
			Note that WinAPIWindow::dimensions and WinAPIWindow::anchorPoint here is not updated yet.
			It is better to handle anything that is related to position, dimensions or window state inside
			WinAPIWindow::wmWindowPosChanged procedure;
		*/
		return DefWindowProc(hWnd, WM_WINDOWPOSCHANGING, wParam, lParam);
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

	void WinAPIWindow::p_move(const Point& newAnchorPoint)
	{
		/* Just change anchor position */
		auto ret = SetWindowPos(hWnd, NULL, newAnchorPoint.x, newAnchorPoint.y, 0, 0,
			SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOREDRAW | SWP_FRAMECHANGED | SWP_NOSENDCHANGING);
		expect(ret);
	}

	void WinAPIWindow::p_smartMove(const Point& prevAp, const Point& ap)
	{
		if (ap != prevAp)
			p_move(ap);
	}

	void WinAPIWindow::p_resize(const Dimensions& d)
	{
		/* Just change dimensions */
		auto ret = SetWindowPos(hWnd, NULL, 0, 0, d.width, d.height,
			SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOREDRAW | SWP_FRAMECHANGED | SWP_NOSENDCHANGING);
		expect(ret);
	}

	void WinAPIWindow::p_smartResize(const Dimensions& prevD, const Dimensions& d)
	{
		if (d != prevD)
			p_resize(d);
	}

	void WinAPIWindow::p_update()
	{
		/* Just update window */
		auto ret = SetWindowPos(hWnd, NULL, 0, 0, 0, 0,
			SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOREDRAW | SWP_FRAMECHANGED | SWP_NOSENDCHANGING);
		expect(ret);
	}

	void WinAPIWindow::p_moveAndResize(const Point& an, const Dimensions& d)
	{
		/* Just move and resize window */
		auto ret = SetWindowPos(hWnd, NULL, an.x, an.y, d.width, d.height,
			SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOREDRAW | SWP_FRAMECHANGED | SWP_NOSENDCHANGING);
		expect(ret);
	}

	void WinAPIWindow::p_smartMoveAndResize(const Point& prevAn, const Point& an, const Dimensions& prevD, const Dimensions& d)
	{
		bool anChanged = an != prevAn;
		bool dChanged = d != prevD;
		if (anChanged && dChanged)
			return p_moveAndResize(an, d);
		
		if (anChanged)
			return p_move(an);
		
		if (dChanged)
			return p_resize(d);

		expect(false);
	}

	void WinAPIWindow::p_redraw()
	{
		auto ret = RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE | RDW_NOERASE | RDW_INTERNALPAINT);
		expect(ret);
	}

	bool WinAPIWindow::handleWindowStateEvent(WindowState prev, WindowState current)
	{
		if ((prev == WindowState::MAXIMIZED || prev == WindowState::MINIMIZED) && current == WindowState::NORMAL)
		{
			static_cast<ComponentEvent::Handler&>(windowController).handleEvent(ComponentEvent::Window::RestoreEvent{});
			return true;
		}
		else if (current == WindowState::MINIMIZED)
		{
			static_cast<ComponentEvent::Handler&>(windowController).handleEvent(ComponentEvent::Window::MinimizeEvent{});
			return true;
		}
		else if (current == WindowState::MAXIMIZED)
		{
			static_cast<ComponentEvent::Handler&>(windowController).handleEvent(ComponentEvent::Window::MaximizeEvent{});
			return true;
		}

		/* Could not handle this window state change. */
		return false;
	}
}