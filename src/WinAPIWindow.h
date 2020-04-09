#pragma once

#include "pch.h"

#include "Geometry.h"
#include "components/ComponentEvent.h"
#include "ComponentEventHandler.h"
#include "WinAPIWindowDefaultEventHandler.h"

namespace Awincs
{
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
		WinAPIWindow();
		WinAPIWindow(ComponentEvent::ComponentEventHandler& handler);
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

		std::pair<std::set<ComponentEvent::ModificationKey>, std::set<ComponentEvent::MouseButtonType>> parseMouseKeyState(WORD keyState);

	private:
		static constexpr std::wstring_view DEFAULT_WINDOW_TITLE								= L"Default Window";
		static constexpr Dimensions DEFAULT_WINDOW_DIMENSIONS								= { 1280, 720 };
		static constexpr Point DEFAULT_WINDOW_ANCHOR_POINT									= {100, 100};
		static constexpr std::wstring_view WINDOW_CLASS_NAME								= L"WindowComponent";
		static constexpr COLORREF DEFAULT_WINDOW_BACKGROUND_COLOR							= RGB(0x10, 0x20, 0x30);
		static constexpr WinAPIWindowDefaultEventHandler DEFAULT_EVENT_HANDLER				= {};
		static WinAPIWindowRegisterer registerer;

		HINSTANCE hInstance															= GetModuleHandle(NULL);;
		HWND hWnd																	= NULL;
		std::wstring windowTitle													= DEFAULT_WINDOW_TITLE.data();
		Point anchorPoint															= DEFAULT_WINDOW_ANCHOR_POINT;
		Dimensions dimensions														= DEFAULT_WINDOW_DIMENSIONS;
		ComponentEvent::ComponentEventHandler& eventHandler;
		std::vector<DrawCallback> drawQueue;
	};
}