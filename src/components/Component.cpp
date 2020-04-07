#include "Component.h"

namespace AWC
{
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

		return {x, y};
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

	void Component::handleEvent(const MouseEvent& e)
	{
		auto mouseEvent = static_cast<const ComponentEvent::MouseEvent&>(e);

		ComponentEvent::MouseEvent localMouseEvent = mouseEvent;
		localMouseEvent.point = transformToLocalPoint(mouseEvent.point);

		for (const auto& child : children)
		{
			if (child->checkAffiliation(localMouseEvent.point))
			{
				child->handleEvent(localMouseEvent);
				break;
			}
		}
	}

	void Component::handleEvent(const KeyEvent& e)
	{
		for(const auto & child : children)
			child->handleEvent(e);
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