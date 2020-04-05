#pragma once
#include <utility>
#include <unordered_set>
#include <memory>
#include <windows.h>
#include <functional>

#include "../Physics.h"

namespace AWC
{
	class Component
		:
		public std::enable_shared_from_this<Component>
	{
	private:
		using Dimensions = Physics::IntDimensions2D;
		using Point = Physics::IntPoint2D;

	public:
		using ComponentCallback = std::function<void(Component&)>;

	public:
		void setDimensions(Dimensions dims);
		Dimensions getDimensions() const;
		void setAnchorPoint(Point point);
		Point getAnchorPoint() const;
		Point getGlobalAnchorPoint() const;
		virtual void setParent(std::shared_ptr<Component> parent);
		virtual void unsetParent();
		void foreachChildren(ComponentCallback cb);
		virtual void redraw();
		virtual bool shouldRedraw() const;
		virtual void draw(HDC hdc) const;

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