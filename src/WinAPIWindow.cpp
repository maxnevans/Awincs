#include <Windowsx.h>
#include <memory>

#include "../../DebugConsole/include/DebugConsole.h"
#include "base/DebugUntils.h"
#include "WinAPIWindow.h"
#include "WindowException.h"


namespace AWC
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

	WinAPIWindow::WinAPIWindow()
	{
		assert(registerer.registered);

		auto [x, y] = getAnchorPoint();
		auto [width, height] = getDimensions();

		CreateWindowW(WINDOW_CLASS_NAME.data(), windowTitle.c_str(), WS_OVERLAPPEDWINDOW,
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

	HWND WinAPIWindow::getHWND()
	{
		return this->hWnd;
	}

	void WinAPIWindow::setTitle(std::wstring title)
	{
		windowTitle = title;
		SetWindowTextW(hWnd, title.c_str());
	}

	std::wstring WinAPIWindow::getTitle() const
	{
		return windowTitle;
	}

	void WinAPIWindow::setAnchorPoint(Point point)
	{
		anchorPoint = point;
	}

	WinAPIWindow::Point WinAPIWindow::getAnchorPoint()
	{
		return anchorPoint;
	}

	void WinAPIWindow::setDimensions(Dimensions dims)
	{
		dimensions = dims;
	}

	WinAPIWindow::Dimensions WinAPIWindow::getDimensions()
	{
		return dimensions;
	}

	void WinAPIWindow::draw(DrawCallback cb)
	{
		drawQueue.push_back(cb);
	}

	void WinAPIWindow::registerEventCallback(EventCallback cb)
	{
		expect(!eventCallback);
		eventCallback = cb;
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
		case WM_ERASEBKGND: return TRUE;
		case WM_CLOSE: return wmClose(wParam, lParam);
		case WM_PAINT: return wmPaint(wParam, lParam);
		case WM_SIZING:
		case WM_MOVING: return wmSizingMoving(wParam, lParam);
		case WM_SIZE: return wmSize(wParam, lParam);
		case WM_MOVE: return wmMove(wParam, lParam);
		case WM_LBUTTONUP: return wmLButtonUp(wParam, lParam);
		}

		return DefWindowProcW(hWnd, uMsg, wParam, lParam);
	}

	LRESULT WinAPIWindow::wmLButtonUp(WPARAM wParam, LPARAM lParam)
	{
		DCONSOLE(L"Clicked at (" << GET_X_LPARAM(lParam) << L',' << GET_Y_LPARAM(lParam) << L");\n");

		if (eventCallback)
		{
			ComponentEvent::MouseEvent e = {};
			e.point = { GET_X_LPARAM(lParam) ,  GET_Y_LPARAM(lParam) };

			eventCallback(e);
		}

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

		auto [width, height] = getDimensions();
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

		setAnchorPoint({ pRect->left, pRect->top });
		setDimensions({ pRect->right - pRect->left, pRect->bottom - pRect->top });

		return TRUE;
	}

	LRESULT WinAPIWindow::wmSize(WPARAM wParam, LPARAM lParam)
	{
		setDimensions({ LOWORD(lParam), HIWORD(lParam) });
		return 0;
	}

	LRESULT WinAPIWindow::wmMove(WPARAM wParam, LPARAM lParam)
	{
		setAnchorPoint({ LOWORD(lParam), HIWORD(lParam) });
		return 0;
	}

}