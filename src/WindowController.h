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
		virtual ShouldParentHandleEvent handleEvent(const ComponentEvent::Window::MinimizeEvent&) override;
		virtual ShouldParentHandleEvent handleEvent(const ComponentEvent::Window::MaximizeEvent&) override;
		virtual ShouldParentHandleEvent handleEvent(const ComponentEvent::Window::RestoreEvent&) override;

	protected:
		virtual void draw(HDC) const override;
		void p_redraw();

		template<typename WindowEvent>
		inline ShouldParentHandleEvent handleWindowEvent(const WindowEvent& e)
		{
			auto shouldHandleEvent = Component::handleEvent(e);
			expect(shouldHandleEvent);
			p_redraw();
			return true;
		}

	private:
		static constexpr COLORREF DEFAULT_BACKGROUND_COLOR = RGB(0x10, 0x20, 0x30);

		COLORREF backgroundColor = DEFAULT_BACKGROUND_COLOR;
		std::unique_ptr<WinAPIWindow> window;
		CaptureCallback moveCapture;
	};
}