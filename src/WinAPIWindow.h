#pragma once

#include "pch.h"

#include "Geometry.h"
#include "WindowController.h"
#include "component/event/Event.h"

namespace Awincs
{
	class WindowController;

	class WinAPIWindow
	{
	protected:
		using Point = Geometry::IntPoint2D;
		using Dimensions = Geometry::IntDimensions2D;
		using DrawCallback = std::function<void(HDC)>;

		struct WindowDimensions
		{
			Dimensions normal;
			Dimensions max;
			Dimensions min;
		};

		enum class WindowState
		{
			MAXIMIZED,
			MINIMIZED,
			NORMAL
		};

		class WinAPIWindowRegisterer
		{
		public:
			WinAPIWindowRegisterer();
			~WinAPIWindowRegisterer();

		public:
			bool registered = false;
		};

	public:
		WinAPIWindow(WindowController& window, const Point& anchorPoint, const Dimensions& dims);
		WinAPIWindow(WindowController& window);
		~WinAPIWindow();
		WinAPIWindow(const WinAPIWindow&) = delete;
		WinAPIWindow& operator=(const WinAPIWindow&) = delete;

		void create();
		void show();
		void hide();
		void minimize();
		void maximize();
		void close();
		HWND getHWND()const;
		void setTitle(const std::wstring& title);
		const std::wstring& getTitle() const;
		void setAnchorPoint(const Point& point);
		const Point& getAnchorPoint() const;
		void setDimensions(const Dimensions& dims);
		const Dimensions& getDimensions() const;
		void draw(DrawCallback cb);
		void setResizeBorderWidth(int width);
		void setMinDimensions(const Dimensions&);
		void setMaxDimensions(const Dimensions&);
		const Dimensions& getMinDimensions();
		const Dimensions& getMaxDimensions();

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
		LRESULT wmMouseButton(WPARAM wParam, LPARAM lParam, ComponentEvent::Mouse::ButtonType mbt, ComponentEvent::Mouse::ButtonAction mba);
		LRESULT wmMouseWheel(WPARAM wParam, LPARAM lParam);
		LRESULT wmUnichar(WPARAM wParam, LPARAM lParam);
		LRESULT wmChar(WPARAM wParam, LPARAM lParam);
		LRESULT wmKeyUp(WPARAM wParam, LPARAM lParam);
		LRESULT wmKeyDown(WPARAM wParam, LPARAM lParam);
		LRESULT wmClose(WPARAM wParam, LPARAM lParam);
		LRESULT wmPaint(WPARAM wParam, LPARAM lParam);
		LRESULT wmSizing(WPARAM wParam, LPARAM lParam);
		LRESULT wmMoving(WPARAM wParam, LPARAM lParam);
		LRESULT wmSize(WPARAM wParam, LPARAM lParam);
		LRESULT wmMove(WPARAM wParam, LPARAM lParam);
		LRESULT wmEraseBackground(WPARAM wParam, LPARAM lParam);
		LRESULT wmNCHitTest(WPARAM wParam, LPARAM lParam);
		LRESULT wmNCCalcSize(WPARAM wParam, LPARAM lParam);
		LRESULT wmCreate(WPARAM wParam, LPARAM lParam);
		LRESULT wmGetMinMaxInfo(WPARAM wParam, LPARAM lParam);
		LRESULT wmSetTextAndSetIcon(UINT uMsg, WPARAM wParam, LPARAM lParam);
		LRESULT wmDWMCompositionChanged(WPARAM wParam, LPARAM lParam);
		LRESULT wmNCActivate(WPARAM wParam, LPARAM lParam);
		LRESULT wmNCPaint(WPARAM wParam, LPARAM lParam);
		LRESULT wmNCUAHDrawFrameAndCaption(WPARAM wParam, LPARAM lParam);
		LRESULT wmThemeChanged(WPARAM wParam, LPARAM lParam);
		LRESULT wmWindowPosChanged(WPARAM wParam, LPARAM lParam);
		LRESULT wmWindowPosChanging(WPARAM wParam, LPARAM lParam);

		std::pair<std::set<ComponentEvent::Keyboard::ModificationKey>, std::set<ComponentEvent::Mouse::ButtonType>> parseMouseKeyState(WORD keyState);
		void p_move(const Point& ap);
		void p_smartMove(const Point& prevAp, const Point& ap);
		void p_resize(const Dimensions& d);
		void p_smartResize(const Dimensions& prevD, const Dimensions& d);
		void p_update();
		void p_moveAndResize(const Point& an, const Dimensions& d);
		void p_smartMoveAndResize(const Point& prevAn, const Point& an, const Dimensions& prevD, const Dimensions& d);
		void redraw();
		bool handleWindowStateEvent(WindowState prev, WindowState current);
		void updateRegion();
		bool hasAutohideAppbar(UINT edge, const RECT& mon);
		void setupDWM();

	private:
		static constexpr int DEFAULT_RESIZE_BORDER_WIDTH							= 5;
		static constexpr Dimensions DEFAULT_WINDOW_DIMENSIONS						= { 1280, 720 };
		static constexpr Dimensions DEFAULT_MAXIMUM_DIMENSIONS                      = { 0, 0 };
		static constexpr Dimensions DEFAULT_MINIMUM_DIMENSIONS						= { 500, 300 };
		static constexpr std::wstring_view DEFAULT_WINDOW_TITLE						= L"Default Window";
		static constexpr Point DEFAULT_WINDOW_ANCHOR_POINT							= { 100, 100 };
		static constexpr std::wstring_view WINDOW_CLASS_NAME						= L"WindowComponent";
		static constexpr COLORREF DEFAULT_WINDOW_BACKGROUND_COLOR					= RGB(0x10, 0x20, 0x30);
		static constexpr COLORREF DEFAUL_WINDOW_CHROMA_COLOR						= RGB(0xFF, 0x00, 0xFF);
		static constexpr WindowState DEFAULT_WINDOW_STATE							= WindowState::NORMAL;
		static WinAPIWindowRegisterer registerer;

		HINSTANCE hInstance															= GetModuleHandle(NULL);;
		HWND hWnd																	= NULL;
		std::wstring windowTitle													= DEFAULT_WINDOW_TITLE.data();
		Point anchorPoint															= DEFAULT_WINDOW_ANCHOR_POINT;
		WindowDimensions dimensions													= { DEFAULT_WINDOW_DIMENSIONS, 
																						DEFAULT_MAXIMUM_DIMENSIONS, 
																						DEFAULT_MINIMUM_DIMENSIONS };

		int resizeBorderWidth														= DEFAULT_RESIZE_BORDER_WIDTH;
		WindowState windowState														= DEFAULT_WINDOW_STATE;
		bool compositionEnabled														= false;
		bool themeEnabled															= false;
		WindowController& windowController;
		std::vector<DrawCallback> drawQueue;
	};
}