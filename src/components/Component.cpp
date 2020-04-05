#include "Component.h"

namespace AWC
{
	void Component::setDimensions(Component::Dimensions dims)
	{
		this->dimensions = dims;
	}

	Component::Dimensions Component::getDimensions() const
	{
		return this->dimensions;
	}

	void Component::setAnchorPoint(Component::Point point)
	{
		this->anchorPoint = point;
	}

	Component::Point Component::getAnchorPoint() const
	{
		return this->anchorPoint;
	}

	Component::Point Component::getGlobalAnchorPoint() const
	{
		int x = Physics::getX(anchorPoint);
		int y = Physics::getY(anchorPoint);

		if (auto prnt = parent.lock())
		{
			auto [globalX, globalY] = parent.lock()->getGlobalAnchorPoint();

			x += globalX;
			y += globalY;
		}

		return {x, y};
	}

	void Component::setParent(std::shared_ptr<Component> parent)
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
		for (auto& component : children)
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
		for (auto& component : children)
			component->draw(hdc);
	}

	void Component::addChild(std::shared_ptr<Component> child)
	{
		children.insert(child);
	}

	void Component::removeChild(std::shared_ptr<Component> child)
	{
		children.erase(child);
	}

}