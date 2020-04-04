#include "MainWindow.h"
#include "WindowException.h"
#include <cassert>

std::wstring AWC::MainWindow::className = AWC::MainWindow::BASE_WINDOW_CLASS_NAME;
HINSTANCE AWC::MainWindow::hInstance = GetModuleHandle(NULL);

AWC::MainWindow::MainWindow()
{
	hWnd = CreateWindowW(
		getClassName().c_str(),
		windowTitle.c_str(),
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		100, 200, 1280, 720,
		NULL, NULL, getInstance(), this
	);

	if (auto errorCode = GetLastError(); errorCode != ERROR_SUCCESS)
	{
		throw WindowException(L"Failed to create MainWindow! Error code: " + std::to_wstring(errorCode));
	}

	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);
}

void AWC::MainWindow::RegisterCls()
{
	WNDCLASSEXW wndClass = {};

	wndClass.lpszClassName = className.c_str();
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.hInstance = hInstance;
	wndClass.lpfnWndProc = MainWindow::windowProc;
	wndClass.style = CS_VREDRAW | CS_HREDRAW;

	if (RegisterClassExW(&wndClass) == FALSE)
	{
		auto errorCode = std::to_wstring(GetLastError());

		throw WindowException(L"Failed to register class! Error code: " + errorCode);
	}
}

void AWC::MainWindow::setupHandler(UINT uMsg, std::shared_ptr<WindowMessageHandler> handler, bool clearPrevious)
{
	auto handlerList = handlers[uMsg];

	if (clearPrevious)
		handlerList.clear();

	handlerList.push_back(handler);

	handlers[uMsg] = handlerList;
}

HINSTANCE AWC::MainWindow::getInstance()
{
	return MainWindow::hInstance;
}

std::wstring AWC::MainWindow::getClassName()
{
	return MainWindow::className;
}

LRESULT CALLBACK AWC::MainWindow::windowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	
	return DefWindowProcW(hwnd, uMsg, wParam, lParam);
}

LRESULT AWC::MainWindow::wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (auto handler = handlers.find(uMsg); handler != handlers.end())
	{
		auto handlersList = handler->second;

		for (const auto& h : handlersList)
			(*h)(wParam, lParam);
	}

	return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}
