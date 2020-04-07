#pragma once

#include <string>
#include <functional>
#include <vector>
#include <windows.h>

#include "Geometry.h"
#include "components/ComponentEvent.h"

namespace AWC
{
	class WinAPIWindow
	{
	private:

		using Point = Geometry::IntPoint2D;
		using Dimensions = Geometry::IntDimensions2D;
		using DrawCallback = std::function<void(HDC)>;
		using EventCallback = std::function<void(const ComponentEvent::Event&)>;

		class WinAPIWindowRegisterer
		{
		public:
			WinAPIWindowRegisterer();

		public:
			bool registered = false;
		};

	public:
		WinAPIWindow();
		~WinAPIWindow();
		WinAPIWindow(const WinAPIWindow&) = delete;
		WinAPIWindow& operator=(const WinAPIWindow&) = delete;

		void show();
		void hide();
		HWND getHWND();
		void setTitle(std::wstring title);
		std::wstring getTitle() const;
		void setAnchorPoint(Point point);
		Point getAnchorPoint();
		void setDimensions(Dimensions dims);
		Dimensions getDimensions();
		void draw(DrawCallback cb);
		void registerEventCallback(EventCallback cb);

	protected:
		/*Window Procedure Callbacks*/
		static LRESULT CALLBACK setupWindowProc(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam);
		static LRESULT CALLBACK proxyWindowProc(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam);
		LRESULT windowProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

	protected:

		/*Window Message Handlers*/
		LRESULT wmLButtonUp(WPARAM wParam, LPARAM lParam);
		LRESULT wmClose(WPARAM wParam, LPARAM lParam);
		LRESULT wmPaint(WPARAM wParam, LPARAM lParam);
		LRESULT wmSizingMoving(WPARAM wParam, LPARAM lParam);
		LRESULT wmSize(WPARAM wParam, LPARAM lParam);
		LRESULT wmMove(WPARAM wParam, LPARAM lParam);

	private:
		static constexpr std::wstring_view DEFAULT_WINDOW_TITLE		= L"Default Window";
		static constexpr Dimensions DEFAULT_WINDOW_DIMENSIONS		= { 1280, 720 };
		static constexpr Point DEFAULT_WINDOW_ANCHOR_POINT			= {100, 100};
		static constexpr std::wstring_view WINDOW_CLASS_NAME		= L"WindowComponent";
		static constexpr COLORREF DEFAULT_WINDOW_BACKGROUND_COLOR	= RGB(0x10, 0x20, 0x30);
		static WinAPIWindowRegisterer registerer;

		HINSTANCE hInstance											= GetModuleHandle(NULL);;
		HWND hWnd													= NULL;
		std::wstring windowTitle									= DEFAULT_WINDOW_TITLE.data();
		Point anchorPoint											= DEFAULT_WINDOW_ANCHOR_POINT;
		Dimensions dimensions										= DEFAULT_WINDOW_DIMENSIONS;
		EventCallback eventCallback									= nullptr;
		std::vector<DrawCallback> drawQueue;
	};
}