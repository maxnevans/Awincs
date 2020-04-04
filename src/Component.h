#pragma once
#include <utility>
#include <unordered_set>
#include <memory>
#include <windows.h>
#include <functional>
#include "Drawable.h"

namespace AWC
{
	template<typename T>
	using Point2D = std::pair<T, T>;
	using IntPoint2D = Point2D<int>;
	static const int X = 0;
	static const int Y = 1;


	template<typename T>
	using Dimensions2D = std::pair<T, T>;
	using IntDimensions2D = Dimensions2D<int>;
	static const int WIDTH = 0;
	static const int HEIGHT = 1;
	

	class Component
		:
		public std::enable_shared_from_this<Component>,
		public Drawable
	{
	public:
		typedef std::function<void(Component&)> ComponentCallback;

	public:
		void setDimensions(IntDimensions2D dims);
		IntDimensions2D getDimensions() const;
		void setAnchorPoint(IntPoint2D point);
		IntPoint2D getAnchorPoint() const;
		IntPoint2D getGlobalAnchorPoint() const;
		virtual void setParent(std::shared_ptr<Component> parent);
		virtual void unsetParent();
		void foreachChildren(ComponentCallback cb);
		virtual void redraw() override;
		virtual bool shouldRedraw() const override;
		virtual void draw(HDC hdc) const override;

	protected:
		virtual void addChild(std::shared_ptr<Component> child);
		virtual void removeChild(std::shared_ptr<Component> child);

	private:
		static constexpr IntPoint2D DEFAULT_ANCHOR_POINT			= { 100, 200 };
		static constexpr IntDimensions2D DEFAULT_DIMENSIONS			= { 1280, 720 };

		IntPoint2D anchorPoint										= DEFAULT_ANCHOR_POINT;
		IntDimensions2D dimensions									= DEFAULT_DIMENSIONS;
		std::unordered_set<std::shared_ptr<Component>> children;
		std::weak_ptr<Component> parent;
		bool performRedraw = false;
	};
}