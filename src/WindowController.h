#pragma once

#include "pch.h"

#include "WinAPIWindow.h"
#include "component/Component.h"

namespace Awincs
{
	class WinAPIWindow;

	class WindowController
		:
		public Component
	{
	public:
		using CaptureCallback = std::function<bool(Point)>;

	public:
		WindowController() = default;
		WindowController(const Point& anchorPoint, const Dimensions& dims);
		WindowController(const WindowController&) = delete;
		WindowController& operator=(const WindowController&) = delete;

		void setTitle(const std::wstring& title);
		const std::wstring& getTitle() const;
		void setDimensions(const Dimensions& dims);
		void setMaxDimensions(const Dimensions& dims);
		void setMinDimensions(const Dimensions& dims);
		const Dimensions& getDimensions() const;
		const Dimensions& getMaxDimensions() const;
		const Dimensions& getMinDimensions() const;
		void setAnchorPoint(const Point&);
		const Point& getAnchorPoint() const;
		void show();
		void hide();
		void redraw();
		void setMoveCapture(CaptureCallback);
		bool testMoveCapture(const Point&) const;
		virtual bool checkAffiliationIgnoreChildren(const Point& p) const override;
		Point transformToLocalPoint(const Point& p) const;
		virtual void closeWindow() override;
		virtual void minimizeWindow() override;
		virtual void maximizeWindow() override;

		virtual ShouldParentHandleEvent handleEvent(const ComponentEvent::Window::MoveEvent&) override;
		virtual ShouldParentHandleEvent handleEvent(const ComponentEvent::Window::ResizeEvent&) override;

	protected:
		virtual void draw(HDC) const override;
		void p_redraw();

	private:
		static constexpr COLORREF DEFAULT_BACKGROUND_COLOR = RGB(10, 20, 30);
		static constexpr int BACKGROUND_FIX_ON_RESIZE = 100;

		COLORREF backgroundColor = DEFAULT_BACKGROUND_COLOR;
		std::unique_ptr<WinAPIWindow> window;
		CaptureCallback moveCapture;
	};
}