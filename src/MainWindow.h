#pragma once
#include <windows.h>
#include <string>
#include <map>
#include <list>
#include <memory>
#include "WindowMessageHandler.h"

namespace AWC
{
	class MainWindow
	{
	public:
		MainWindow(DWORD dwStyles, int x, int y, int nWidth, int nHeight);
		static void RegisterCls();

	protected:
		void setupHandler(UINT uMsg, std::shared_ptr<WindowMessageHandler> handler, bool clearPrevious = false);
		HINSTANCE getInstance();
		std::wstring getClassName();

	private:
		static LRESULT CALLBACK windowProc(_In_ HWND hwnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam);
		LRESULT wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	private:
		static constexpr const wchar_t* BASE_WINDOW_CLASS_NAME = L"Window";
		static std::wstring className;
		static HINSTANCE hInstance;
		
		std::map<UINT, std::list<std::shared_ptr<WindowMessageHandler>>> handlers;

		std::wstring windowTitle = L"";
		HWND hWnd = NULL;
	};
}