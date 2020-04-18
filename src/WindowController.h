#pragma once

#include "pch.h"

#include "platform/WinAPIWindow.h"
#include "Component.h"
#include "event/EventManager.h"

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
		virtual Event::EventManager<Component>& getEventManager();
		
		virtual ShouldParentHandleEvent handleEvent(const Event::Window::MoveEvent&) override;
		virtual ShouldParentHandleEvent handleEvent(const Event::Window::ResizeEvent&) override;
		virtual ShouldParentHandleEvent handleEvent(const Event::Window::MinimizeEvent&) override;
		virtual ShouldParentHandleEvent handleEvent(const Event::Window::MaximizeEvent&) override;
		virtual ShouldParentHandleEvent handleEvent(const Event::Window::RestoreEvent&) override;

	protected:
		virtual void draw(Gdiplus::Graphics&) const override;

		void p_redraw();
		void p_setupDrawCallback(WinAPIWindow&);

		template<typename WindowEvent>
		inline ShouldParentHandleEvent p_handleWindowEvent(const WindowEvent& e)
		{
			auto shouldHandleEvent = Component::handleEvent(e);
			expect(shouldHandleEvent);
			p_redraw();
			return true;
		}

	private:
		static constexpr gp::ARGB DEFAULT_BACKGROUND_COLOR = 0xff005cc5;

		gp::ARGB backgroundColor = DEFAULT_BACKGROUND_COLOR;
		std::unique_ptr<WinAPIWindow> window;
		CaptureCallback moveCapture;
		Event::EventManager<Component> eventManager;
	};
}