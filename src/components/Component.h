#pragma once
#include <utility>
#include <unordered_set>
#include <memory>
#include <windows.h>
#include <functional>

#include "../Geometry.h"
#include "ComponentEvent.h"

namespace AWC
{
	class Component
		:
		public std::enable_shared_from_this<Component>
	{
	protected:
		using Dimensions = Geometry::IntDimensions2D;
		using Point = Geometry::IntPoint2D;
		using MouseEvent = ComponentEvent::MouseEvent;
		using KeyEvent = ComponentEvent::KeyEvent;

	public:
		using ComponentCallback = std::function<void(Component&)>;

	public:
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
		virtual bool checkAffiliation(const Point& pt) const = 0;
		virtual void handleEvent(const MouseEvent& e);
		virtual void handleEvent(const KeyEvent& e);

	protected:
		virtual void addChild(std::shared_ptr<Component> child);
		virtual void removeChild(std::shared_ptr<Component> child);

	private:
		static constexpr Point DEFAULT_ANCHOR_POINT					= { 100, 200 };
		static constexpr Dimensions DEFAULT_DIMENSIONS				= { 1280, 720 };

		Point anchorPoint											= DEFAULT_ANCHOR_POINT;
		Dimensions dimensions										= DEFAULT_DIMENSIONS;
		bool performRedraw											= false;
		std::unordered_set<std::shared_ptr<Component>> children;
		std::weak_ptr<Component> parent;
	};
}