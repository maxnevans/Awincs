#pragma once

#include "pch.h"

#include "Geometry.h"
#include "WindowController.h"
#include "components/ComponentEvent.h"

namespace Awincs
{
	class WindowController;

	class WinAPIWindow
	{
	protected:
		using Point = Geometry::IntPoint2D;
		using Dimensions = Geometry::IntDimensions2D;
		using DrawCallback = std::function<void(HDC)>;

		class WinAPIWindowRegisterer
		{
		public:
			WinAPIWindowRegisterer();

		public:
			bool registered = false;
		};

	public:
		WinAPIWindow(WindowController& window, const Point& anchorPoint, const Dimensions& dims);
		~WinAPIWindow();
		WinAPIWindow(const WinAPIWindow&) = delete;
		WinAPIWindow& operator=(const WinAPIWindow&) = delete;

		void show();
		void hide();
		void minimize();
		void maximize();
		void close();
		HWND getHWND()const;
		void setTitle(const std::wstring& title);
		const std::wstring& getTitle() const;
		void setAnchorPoint(Point point);
		const Point& getAnchorPoint() const;
		void setDimensions(Dimensions dims);
		const Dimensions& getDimensions() const;
		void draw(DrawCallback cb);
		void setResizeBorderWidth(int width);

	protected:
		/*Window Procedure Callbacks*/
		static LRESULT CALLBACK setupWindowProc(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam);
		static LRESULT CALLBACK proxyWindowProc(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam);
		LRESULT windowProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

	protected:

		/*Window Message Handlers*/
		LRESULT wmLButtonUp(WPARAM wParam, LPARAM lParam);
		LRESULT wmLButtonDown(WPARAM wParam, LPARAM lParam);
		LRESULT wmRButtonUp(WPARAM wParam, LPARAM lParam);
		LRESULT wmRButtonDown(WPARAM wParam, LPARAM lParam);
		LRESULT wmMButtonUp(WPARAM wParam, LPARAM lParam);
		LRESULT wmMButtonDown(WPARAM wParam, LPARAM lParam);
		LRESULT wmXButtonUp(WPARAM wParam, LPARAM lParam);
		LRESULT wmXButtonDown(WPARAM wParam, LPARAM lParam);
		LRESULT wmMouseButton(WPARAM wParam, LPARAM lParam, ComponentEvent::MouseButtonType mbt, ComponentEvent::MouseButtonAction mba);
		LRESULT wmMouseWheel(WPARAM wParam, LPARAM lParam);
		LRESULT wmUnichar(WPARAM wParam, LPARAM lParam);
		LRESULT wmChar(WPARAM wParam, LPARAM lParam);
		LRESULT wmKeyUp(WPARAM wParam, LPARAM lParam);
		LRESULT wmKeyDown(WPARAM wParam, LPARAM lParam);
		LRESULT wmClose(WPARAM wParam, LPARAM lParam);
		LRESULT wmPaint(WPARAM wParam, LPARAM lParam);
		LRESULT wmSizingMoving(WPARAM wParam, LPARAM lParam);
		LRESULT wmSize(WPARAM wParam, LPARAM lParam);
		LRESULT wmMove(WPARAM wParam, LPARAM lParam);
		LRESULT wmEraseBackground(WPARAM wParam, LPARAM lParam);
		LRESULT wmNCHitTest(WPARAM wParam, LPARAM lParam);
		LRESULT wmNCCalcSize(WPARAM wParam, LPARAM lParam);
		LRESULT wmCreate(WPARAM wParam, LPARAM lParam);

		std::pair<std::set<ComponentEvent::ModificationKey>, std::set<ComponentEvent::MouseButtonType>> parseMouseKeyState(WORD keyState);
		void moveWindow(const Point& ap);
		void redraw();

	public:
		static constexpr std::wstring_view DEFAULT_WINDOW_TITLE = L"Default Window";
		static constexpr Dimensions DEFAULT_WINDOW_DIMENSIONS = { 1285, 725 };
		static constexpr Point DEFAULT_WINDOW_ANCHOR_POINT = { 100, 100 };
		static constexpr std::wstring_view WINDOW_CLASS_NAME = L"WindowComponent";
		static constexpr COLORREF DEFAULT_WINDOW_BACKGROUND_COLOR = RGB(0x10, 0x20, 0x30);
		static constexpr int DEFAULT_RESIZE_BORDER_WIDTH = 5;

	private:
		static WinAPIWindowRegisterer registerer;

		HINSTANCE hInstance															= GetModuleHandle(NULL);;
		HWND hWnd																	= NULL;
		std::wstring windowTitle													= DEFAULT_WINDOW_TITLE.data();
		Point anchorPoint															= DEFAULT_WINDOW_ANCHOR_POINT;
		Dimensions dimensions														= DEFAULT_WINDOW_DIMENSIONS;

		// To fix Microsoft bug with WS_POPUP window when changing size don't use this inside WinAPIWindow class
		Dimensions cachedUserDimensions												= DEFAULT_WINDOW_DIMENSIONS - DEFAULT_RESIZE_BORDER_WIDTH;

		int resizeBorderWidth														= DEFAULT_RESIZE_BORDER_WIDTH;
		WindowController& windowController;
		std::vector<DrawCallback> drawQueue;
	};
}