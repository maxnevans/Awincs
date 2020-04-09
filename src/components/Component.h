#pragma once

#include "../pch.h"

#include "../Geometry.h"
#include "../ComponentEventHandler.h"

namespace Awincs
{
	class Component
		:
		public std::enable_shared_from_this<Component>,
		public ComponentEvent::ComponentEventHandler
	{
	public:
		using Point = Geometry::IntPoint2D;
		using Dimensions = Geometry::IntDimensions2D;

	public:
		using ComponentCallback = std::function<void(Component&)>;
		using ShouldParentHandleEvent = ComponentEvent::ComponentEventHandler::ShouldParentHandleEvent;
		using MouseEvent = ComponentEvent::MouseEvent;
		using KeyboardEvent = ComponentEvent::KeyboardEvent;
		using MouseButtonEvent = ComponentEvent::MouseButtonEvent;
		using MouseWheelEvent = ComponentEvent::MouseWheelEvent;
		using KeyEvent = ComponentEvent::KeyEvent;
		using InputEvent = ComponentEvent::InputEvent;

	public:
		Component() = default;
		Component(Dimensions dims);
		void setDimensions(const Dimensions& dims);
		Dimensions getDimensions() const;
		void setAnchorPoint(const Point& point);
		Point getAnchorPoint() const;
		Point getGlobalAnchorPoint() const;
		Point transformToLocalPoint(const Point& point);
		virtual void setParent(const std::shared_ptr<Component>& parent);
		virtual void unsetParent();
		void foreachChildren(ComponentCallback cb);
		virtual void redraw();
		virtual bool shouldRedraw() const;
		virtual void draw(HDC hdc) const;
		virtual bool checkAffiliation(const Point& pt) const { return false; };

	protected:
		virtual void addChild(std::shared_ptr<Component> child);
		virtual void removeChild(std::shared_ptr<Component> child);
		virtual ShouldParentHandleEvent handleEvent(const MouseButtonEvent&);
		virtual ShouldParentHandleEvent handleEvent(const MouseWheelEvent&);
		virtual ShouldParentHandleEvent handleEvent(const KeyEvent&);
		virtual ShouldParentHandleEvent handleEvent(const InputEvent&);

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
				if (child->checkAffiliation(e.point))
					return child->handleEvent(eCopy);
			}
		}
		return true;
	}
	template<typename GNonMouseEvent>
	// requires std::is_convertable<GNonMouseEvent, ComponentEvent::Event>::value_type
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