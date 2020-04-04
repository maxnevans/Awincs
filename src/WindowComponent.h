#pragma once
#include <string>
#include <map>
#include <list>
#include <memory>
#include <functional>
#include <windows.h>
#include "Component.h"
#include "WindowMessageHandler.h"
#include "Drawable.h"

namespace AWC
{
	class WindowMessageHandler;

	class WindowComponent
		:
		public Component
	{
	public:
		typedef std::function<void(Drawable&)> DrawableCallback;

	public:
		WindowComponent();
		~WindowComponent();
		WindowComponent(const WindowComponent&) = delete;
		WindowComponent& operator=(const WindowComponent&) = delete;
		void show();
		void hide();
		static void registerClass();
		HWND getHWND();
		void foreachDrawable(DrawableCallback cb);
		virtual void draw(HDC hdc) const override;
		void setTitle(std::wstring title);
		std::wstring getTitle() const;

	protected:
		static LRESULT CALLBACK setupWindowProc(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam);
		static LRESULT CALLBACK proxyWindowProc(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam);
		LRESULT windowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	private:
		static void setupWindowMessageHandlers();
		static void addWindowMessageHandler(std::shared_ptr<WindowMessageHandler> wmh);

	private:
		static constexpr const wchar_t* WINDOW_CLASS_NAME									= L"WindowComponent";
		static constexpr COLORREF DEFAULT_WINDOW_BACKGROUND_COLOR							= RGB(0x10, 0x20, 0x30);
		static bool hasBeenReigstered;
		static HINSTANCE hInstance;
		static std::map<UINT, std::list<std::shared_ptr<WindowMessageHandler>>> handlers;

		HWND hWnd																			= NULL;
		std::wstring windowTitle															= L"";
	};
}