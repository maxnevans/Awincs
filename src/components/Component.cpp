#include "../pch.h"
#include "Component.h"

namespace Awincs
{
	Component::Component(Dimensions dims)
		:
		Component()
	{
		this->dimensions = dims;
	}

	void Component::setDimensions(const Component::Dimensions& dims)
	{
		this->dimensions = dims;
	}

	Component::Dimensions Component::getDimensions() const
	{
		return this->dimensions;
	}

	void Component::setAnchorPoint(const Component::Point& point)
	{
		this->anchorPoint = point;
	}

	Component::Point Component::getAnchorPoint() const
	{
		return this->anchorPoint;
	}

	Component::Point Component::getGlobalAnchorPoint() const
	{
		auto [x, y] = getAnchorPoint();

		if (auto prnt = parent.lock())
		{
			auto [globalX, globalY] = parent.lock()->getGlobalAnchorPoint();

			x += globalX;
			y += globalY;
		}

		return { x, y };
	}

	Component::Point Component::transformToLocalPoint(const Point& point)
	{
		return {
			point.x - anchorPoint.x,
			point.y - anchorPoint.y
		};
	}

	void Component::setParent(const std::shared_ptr<Component>& parent)
	{
		parent->addChild(shared_from_this());
		this->parent = parent;
	}

	void Component::unsetParent()
	{
		this->parent.reset();
	}

	void Component::foreachChildren(ComponentCallback cb)
	{
		for (const auto& component : children)
			cb(*component);
	}

	void Component::redraw()
	{
		performRedraw = true;
	}

	bool Component::shouldRedraw() const
	{
		return performRedraw;
	}

	void Component::draw(HDC hdc) const
	{
		for (const auto& component : children)
			component->draw(hdc);
	}

	void Component::addChild(std::shared_ptr<Component> child)
	{
		children.push_back(child);
	}
	void Component::removeChild(std::shared_ptr<Component> child)
	{
		auto index = std::find(children.begin(), children.end(), child);

		assert(index != children.end());

		children.erase(index);
	}
	Component::ShouldParentHandleEvent Component::handleEvent(const MouseButtonEvent& e)
	{
		return handleMouseEvent(e);
	}
	Component::ShouldParentHandleEvent Component::handleEvent(const MouseWheelEvent& e)
	{
		return handleMouseEvent(e);
	}
	Component::ShouldParentHandleEvent Component::handleEvent(const KeyEvent& e)
	{
		return handleNonMouseEvent(e);
	}
	Component::ShouldParentHandleEvent Component::handleEvent(const InputEvent& e)
	{
		return handleNonMouseEvent(e);
	}
}