#pragma once

#include "../pch.h"

#include "../Geometry.h"
#include "event/Handler.h"
#include "event/Event.h"
#include "../WindowStateHandler.h"

namespace Awincs
{
	class Component
		:
		public std::enable_shared_from_this<Component>,
		public ComponentEvent::Handler,
		public WindowStateHandler
	{
	public:
		using Point = Geometry::IntPoint2D;
		using Dimensions = Geometry::IntDimensions2D;

	public:
		using ComponentCallback = std::function<void(Component&)>;
		using ShouldParentHandleEvent = ComponentEvent::Handler::ShouldParentHandleEvent;
		using MouseEvent = ComponentEvent::Mouse::Event;
		using KeyboardEvent = ComponentEvent::Keyboard::Event;
		using MouseButtonEvent = ComponentEvent::Mouse::ButtonEvent;
		using MouseWheelEvent = ComponentEvent::Mouse::WheelEvent;
		using KeyEvent = ComponentEvent::Keyboard::KeyEvent;
		using InputEvent = ComponentEvent::Keyboard::InputEvent;

	public:
		Component() = default;
		Component(const Point& anchorPoint, const Dimensions& dims);
		void setDimensions(const Dimensions& dims);
		Dimensions getDimensions() const;
		void setAnchorPoint(const Point& point);
		Point getAnchorPoint() const;
		Point getGlobalAnchorPoint() const;
		Point transformToLocalPoint(const Point& point) const;
		virtual void setParent(const std::shared_ptr<Component>& parent);
		virtual void unsetParent();
		void foreachChildren(ComponentCallback cb);
		virtual void redraw();
		virtual bool shouldRedraw() const;
		virtual bool checkAffiliationIgnoreChildren(const Point& pt) const;
		virtual bool checkAffiliationDontIgnoreChildren(const Point&) const;
		virtual void closeWindow() override;
		virtual void minimizeWindow() override;
		virtual void maximizeWindow() override;
		virtual ShouldParentHandleEvent handleEvent(const MouseButtonEvent&);
		virtual ShouldParentHandleEvent handleEvent(const MouseWheelEvent&);
		virtual ShouldParentHandleEvent handleEvent(const KeyEvent&);
		virtual ShouldParentHandleEvent handleEvent(const InputEvent&);

	protected:
		virtual void draw(HDC hdc) const;
		std::weak_ptr<Component> getParent();

	protected:
		virtual void addChild(std::shared_ptr<Component> child);
		virtual void removeChild(std::shared_ptr<Component> child);

	private:
	template<typename GMouseEvent>
	// requires std::is_convertable<GMouseEvent, ComponentEvent::MouseEvent>::value_type
	ShouldParentHandleEvent handleMouseEvent(const GMouseEvent& e)
	{
		if (!children.empty())
		{
			auto eCopy = e;
			for (const auto& child : children)
			{
				eCopy.point = child->transformToLocalPoint(e.point);
				if (child->checkAffiliationIgnoreChildren(e.point))
					return child->handleEvent(eCopy);
			}
		}
		return true;
	}
	template<typename GNonMouseEvent>
	// requires std::is_convertable<GNonMouseEvent, ComponentEvent::CoreEvent>::value_type
	ShouldParentHandleEvent handleNonMouseEvent(const GNonMouseEvent& e)
	{
		bool shouldComponentHandleEvent = true;

		for (const auto& child : children)
			shouldComponentHandleEvent = child->handleEvent(e) && shouldComponentHandleEvent;

		return shouldComponentHandleEvent;
	}

	private:
		static constexpr Point DEFAULT_ANCHOR_POINT					= { 0, 0 };
		static constexpr Dimensions DEFAULT_DIMENSIONS				= { 0, 0 };

		Point anchorPoint											= DEFAULT_ANCHOR_POINT;
		Dimensions dimensions										= DEFAULT_DIMENSIONS;
		bool performRedraw											= false;
		std::vector<std::shared_ptr<Component>> children;
		std::weak_ptr<Component> parent;
	};
}