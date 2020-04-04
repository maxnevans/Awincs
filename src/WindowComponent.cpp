#include <cassert>
#include "WindowComponent.h"
#include "WindowException.h"
#include "MouseClick.h"
#include "WindowClose.h"
#include "WindowPaint.h"
#include "WindowEraseBackground.h"
#include "WindowSizingMoving.h"

bool AWC::WindowComponent::hasBeenReigstered = false;
HINSTANCE AWC::WindowComponent::hInstance = GetModuleHandle(NULL);
std::map<UINT, std::list<std::shared_ptr<AWC::WindowMessageHandler>>> AWC::WindowComponent::handlers;

AWC::WindowComponent::WindowComponent()
{
	// class has not been registered yet
	assert(hasBeenReigstered);

	auto [x, y] = getAnchorPoint();
	auto [width, height] = getDimensions();

	hWnd = CreateWindowW(
		WINDOW_CLASS_NAME,
		windowTitle.c_str(),
		WS_OVERLAPPEDWINDOW,
		x, y, width, height,
		NULL, NULL, hInstance, this
	);

	if (auto errorCode = GetLastError(); errorCode != ERROR_SUCCESS)
	{
		throw WindowException(L"Failed to create window! Error code: " + std::to_wstring(errorCode));
	}

	ShowWindow(hWnd, SW_HIDE);
	UpdateWindow(hWnd);
}

AWC::WindowComponent::~WindowComponent()
{
	DestroyWindow(hWnd);
}

void AWC::WindowComponent::show()
{
	ShowWindow(hWnd, SW_SHOW);
}

void AWC::WindowComponent::hide()
{
	ShowWindow(hWnd, SW_HIDE);
}

void AWC::WindowComponent::registerClass()
{
	// class already has been registered
	assert(!hasBeenReigstered);

	WNDCLASSEXW wndClass = {};

	wndClass.lpszClassName = WINDOW_CLASS_NAME;
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.hInstance = hInstance;
	wndClass.lpfnWndProc = setupWindowProc;
	wndClass.style = CS_VREDRAW | CS_HREDRAW;
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = CreateSolidBrush(DEFAULT_WINDOW_BACKGROUND_COLOR);

	if (RegisterClassExW(&wndClass) == FALSE)
	{
		auto errorCode = std::to_wstring(GetLastError());

		throw WindowException(L"Failed to register class! Error code: " + errorCode);
	}

	setupWindowMessageHandlers();

	hasBeenReigstered = true;
}

HWND AWC::WindowComponent::getHWND()
{
	return this->hWnd;
}

void AWC::WindowComponent::foreachDrawable(DrawableCallback cb)
{
	foreachChildren(cb);
}

void AWC::WindowComponent::draw(HDC hdc) const
{
	InvalidateRect(hWnd, NULL, FALSE);
}

void AWC::WindowComponent::setTitle(std::wstring title)
{
	windowTitle = title;
	SetWindowTextW(hWnd, title.c_str());
}

std::wstring AWC::WindowComponent::getTitle() const
{
	return windowTitle;
}

LRESULT AWC::WindowComponent::setupWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_CREATE)
		throw WindowException(L"Failed to setup proxyWindowProc! WM_NCCREATE has not been called!");

	if (uMsg != WM_NCCREATE)
		return DefWindowProc(hWnd, uMsg, wParam, lParam);

	CREATESTRUCT* createStruct = reinterpret_cast<CREATESTRUCT*>(lParam);
	WindowComponent* const window = reinterpret_cast<WindowComponent* const>(createStruct->lpCreateParams);

	SetLastError(ERROR_SUCCESS);

	if (SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG>(window)) == FALSE && GetLastError() != ERROR_SUCCESS)
		throw WindowException(L"Failed to set GWLP_USERDATA when handling setupWindowProc");

	if (SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG>(WindowComponent::proxyWindowProc)) == FALSE && GetLastError() != ERROR_SUCCESS)
		throw WindowException(L"Failed to set GWLP_WNDPROC when handling setupWindowProc with proxyWindowProc");

	return window->windowProc(hWnd, uMsg, wParam, lParam);
}

LRESULT AWC::WindowComponent::proxyWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	WindowComponent* const wnd = reinterpret_cast<WindowComponent* const>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

	return wnd->windowProc(hWnd, uMsg, wParam, lParam);
}

LRESULT AWC::WindowComponent::windowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (auto handler = handlers.find(uMsg); handler != handlers.end())
	{
		auto handlersList = handler->second;
		LRESULT result = NULL;

		for (const auto& h : handlersList)
			result = (*h)(std::static_pointer_cast<WindowComponent>(shared_from_this()), result, wParam, lParam, uMsg);

		return result;
	}

	return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

void AWC::WindowComponent::setupWindowMessageHandlers()
{
	addWindowMessageHandler(std::shared_ptr<WindowMessageHandler>{ new MouseClick() });
	addWindowMessageHandler(std::shared_ptr<WindowMessageHandler>{ new WindowClose() });
	addWindowMessageHandler(std::shared_ptr<WindowMessageHandler>{ new WindowPaint() });
	addWindowMessageHandler(std::shared_ptr<WindowMessageHandler>{ new WindowEraseBackground() });
	addWindowMessageHandler(std::shared_ptr<WindowMessageHandler>{ new WindowSizingMoving() });
}

void AWC::WindowComponent::addWindowMessageHandler(std::shared_ptr<WindowMessageHandler> wmh)
{
	for (const auto& messageType : wmh->getMessageTypes())
	{
		auto handler = handlers[messageType];
		handler.push_back(wmh);
		handlers[messageType] = handler;
	}
}
