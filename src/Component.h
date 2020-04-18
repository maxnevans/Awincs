#pragma once

#include "pch.h"

#include "Geometry.h"
#include "event/Handler.h"
#include "event/Event.h"
#include "WindowStateHandler.h"

namespace Awincs
{
	namespace
	{
		namespace Event = ComponentEvent;
		namespace gp = Gdiplus;

		static constexpr const int makeARGB(int a, int r, int g, int b)
		{
			return (((gp::ARGB)(b) << gp::Color::BlueShift) |
				((gp::ARGB)(g) << gp::Color::GreenShift) |
				((gp::ARGB)(r) << gp::Color::RedShift) |
				((gp::ARGB)(a) << gp::Color::AlphaShift));
		}
		static constexpr const int makeARGB(int r, int g, int b)
		{
			return makeARGB(0xff, r, g, b);
		}
	}

	class Component
		:
		public std::enable_shared_from_this<Component>,
		public ComponentEvent::Handler,
		public WindowStateHandler
	{
	public:
		using Point = Geometry::IntPoint2D;
		using Dimensions = Geometry::IntDimensions2D;
		using ShouldParentHandleEvent	= Event::Handler::ShouldParentHandleEvent;
		using RedrawCallback = std::function<void()>;

	public:
		Component() = default;
		Component(const Point& anchorPoint, const Dimensions& dims);
		void setDimensions(const Dimensions& dims);
		Dimensions getDimensions() const;
		void setAnchorPoint(const Point& point);
		Point getAnchorPoint() const;
		Point getGlobalAnchorPoint() const;
		Point transformToLocalPoint(const Point& point) const;
		virtual std::shared_ptr<Component> getFocusedComponent() const;
		virtual void setFocusOnThisComponent();
		virtual void setParent(const std::shared_ptr<Component>& parent);
		virtual void unsetParent();
		virtual void redraw();
		virtual bool checkAffiliationIgnoreChildren(const Point& pt) const;
		virtual bool checkAffiliationDontIgnoreChildren(const Point&) const;
		virtual void closeWindow() override;
		virtual void minimizeWindow() override;
		virtual void maximizeWindow() override;
		virtual ShouldParentHandleEvent handleEvent(const Event::Mouse::ButtonEvent&) override;
		virtual ShouldParentHandleEvent handleEvent(const Event::Mouse::WheelEvent&) override;
		virtual ShouldParentHandleEvent handleEvent(const Event::Mouse::Hover&) override;
		virtual ShouldParentHandleEvent handleEvent(const Event::Mouse::HoverStart&) override;
		virtual ShouldParentHandleEvent handleEvent(const Event::Mouse::HoverEnd&) override;
		virtual ShouldParentHandleEvent handleEvent(const Event::Keyboard::KeyEvent&) override;
		virtual ShouldParentHandleEvent handleEvent(const Event::Keyboard::InputEvent&) override;
		virtual ShouldParentHandleEvent handleEvent(const Event::Window::ResizeEvent&) override;
		virtual ShouldParentHandleEvent handleEvent(const Event::Window::MoveEvent&) override;
		virtual ShouldParentHandleEvent handleEvent(const Event::Window::MaximizeEvent&) override;
		virtual ShouldParentHandleEvent handleEvent(const Event::Window::MinimizeEvent&) override;
		virtual ShouldParentHandleEvent handleEvent(const Event::Window::CloseEvent&) override;
		virtual ShouldParentHandleEvent handleEvent(const Event::Window::RestoreEvent&) override;

	protected:
		virtual void draw(gp::Graphics&) const {}
		std::weak_ptr<Component> getParent();
		virtual void addChild(const std::shared_ptr<Component>& child);
		virtual void removeChild(const std::shared_ptr<Component>& child);
		virtual void p_draw(gp::Graphics& gfx);
		void p_setRedrawCallback(RedrawCallback cb);


	private:
		template<typename GMouseEvent>
		// requires std::is_convertable<GMouseEvent, Event::Mouse::Event>::value_type
		std::shared_ptr<Component> getMouseEventComponentHandler(const GMouseEvent& e)
		{
			if (!children.empty())
				for (const auto& child : children)
					if (child->checkAffiliationIgnoreChildren(e.point))
						return child;

			return nullptr;
		}

		template<typename GMouseEvent>
		// requires std::is_convertable<GMouseEvent, ComponentEvent::MouseEvent>::value_type
		ShouldParentHandleEvent handleMouseEvent(const GMouseEvent& e)
		{
			auto handler = getMouseEventComponentHandler(e);
			
			if (handler)
			{
				auto eAdapted = adaptMouseEventToHandler(e, handler);
				return handler->handleEvent(eAdapted);
			}

			return true;
		}

		template<typename GWindowEvent>
		// requires std::is_convertable<GWindowEvent, Event::Window::Event>::value_type
		ShouldParentHandleEvent handleWindowEvent(const GWindowEvent& e)
		{
			bool shouldComponentHandleEvent = true;

			for (const auto& child : children)
				shouldComponentHandleEvent = child->handleEvent(e) && shouldComponentHandleEvent;

			return shouldComponentHandleEvent;
		}

		template<typename GMouseEvent>
		GMouseEvent adaptMouseEventToHandler(const GMouseEvent& e, std::shared_ptr<Component>& handler)
		{
			auto eAdapted = e;
			eAdapted.point = handler->transformToLocalPoint(e.point);
			return eAdapted;
		}

	protected:
		static constexpr Point DEFAULT_ANCHOR_POINT					= { 0, 0 };
		static constexpr Dimensions DEFAULT_DIMENSIONS				= { 0, 0 };

		Point anchorPoint											= DEFAULT_ANCHOR_POINT;
		Dimensions dimensions										= DEFAULT_DIMENSIONS;
		bool shouldRedraw											= true;
		std::shared_ptr<Component> hoveredChild 					= nullptr;
		RedrawCallback redrawCallback								= nullptr;
		std::shared_ptr<Component>* focusedComponent                = nullptr;
		std::vector<std::shared_ptr<Component>> children;
		std::weak_ptr<Component> parent;
	};
}