#include "Component.h"

void AWC::Component::setDimensions(IntDimensions2D dims)
{
	this->dimensions = dims;
}

AWC::IntDimensions2D AWC::Component::getDimensions() const
{
	return this->dimensions;
}

void AWC::Component::setAnchorPoint(IntPoint2D point)
{
	this->anchorPoint = point;
}

AWC::IntPoint2D AWC::Component::getAnchorPoint() const
{
	return this->anchorPoint;
}

AWC::IntPoint2D AWC::Component::getGlobalAnchorPoint() const
{
	IntPoint2D pt = {};

	int localX = std::get<X>(anchorPoint);
	int localY = std::get<Y>(anchorPoint);

	// If parent == nullptr than it's root window. it's anchor point is {0, 0}
	if (auto prnt = parent.lock())
	{
		auto [globalX, globalY] = parent.lock()->getGlobalAnchorPoint();

		pt = {localX + globalX, localY + globalY};
	}

	return pt;
}

void AWC::Component::setParent(std::shared_ptr<Component> parent)
{
	parent->addChild(shared_from_this());
	this->parent = parent;
}

void AWC::Component::unsetParent()
{
	this->parent.reset();
}

void AWC::Component::foreachChildren(ComponentCallback cb)
{
	for (auto& component : children)
		cb(*component);
}

void AWC::Component::redraw()
{
	performRedraw = true;
}

bool AWC::Component::shouldRedraw() const
{
	return performRedraw;
}

void AWC::Component::draw(HDC hdc) const
{
	for (auto& component : children)
		component->draw(hdc);
}

void AWC::Component::addChild(std::shared_ptr<Component> child)
{
	children.insert(child);
}

void AWC::Component::removeChild(std::shared_ptr<Component> child)
{
	children.erase(child);
}
