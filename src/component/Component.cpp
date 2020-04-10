#include "../pch.h"
#include "Component.h"

namespace Awincs
{
	Component::Component(const Point& anchorPoint, const Dimensions& dims)
		:
		Component()
	{
		this->anchorPoint = anchorPoint;
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

	Component::Point Component::transformToLocalPoint(const Point& point) const
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
		auto p = parent.lock();

		if (p)
			p->redraw();
	}

	bool Component::shouldRedraw() const
	{
		return performRedraw;
	}

	bool Component::checkAffiliationIgnoreChildren(const Point& pt) const
	{
		return false;
	}

	bool Component::checkAffiliationDontIgnoreChildren(const Point& p) const
	{
		if (checkAffiliationIgnoreChildren(p))
		{
			for (const auto& child : children)
				if (child->checkAffiliationIgnoreChildren(transformToLocalPoint(p)))
					return false;

			return true;
		}

		return false;
	}

	void Component::closeWindow()
	{
		auto parent = getParent().lock();
		expect(parent);
		parent->closeWindow();
	}

	void Component::minimizeWindow()
	{
		auto parent = getParent().lock();
		expect(parent);
		parent->minimizeWindow();
	}

	void Component::maximizeWindow()
	{
		auto parent = getParent().lock();
		expect(parent);
		parent->maximizeWindow();
	}

	void Component::draw(HDC hdc) const
	{
		for (const auto& component : children)
			component->draw(hdc);
	}

	std::weak_ptr<Component> Component::getParent()
	{
		return parent;
	}

	void Component::addChild(const std::shared_ptr<Component>& child)
	{
		children.push_back(child);
	}
	void Component::removeChild(const std::shared_ptr<Component>& child)
	{
		auto index = std::find(children.begin(), children.end(), child);

		assert(index != children.end());

		children.erase(index);
	}
	Component::ShouldParentHandleEvent Component::handleEvent(const Event::Mouse::ButtonEvent& e)
	{
		return handleMouseEvent(e);
	}
	Component::ShouldParentHandleEvent Component::handleEvent(const Event::Mouse::WheelEvent& e)
	{
		return handleMouseEvent(e);
	}
	Component::ShouldParentHandleEvent Component::handleEvent(const Event::Keyboard::KeyEvent& e)
	{
		return handleNonMouseEvent(e);
	}
	Component::ShouldParentHandleEvent Component::handleEvent(const Event::Keyboard::InputEvent& e)
	{
		return handleNonMouseEvent(e);
	}
	Component::ShouldParentHandleEvent Component::handleEvent(const Event::Window::ResizeEvent& e)
	{
		return handleNonMouseEvent(e);
	}
	Component::ShouldParentHandleEvent Component::handleEvent(const Event::Window::MoveEvent& e)
	{
		return handleNonMouseEvent(e);
	}
	Component::ShouldParentHandleEvent Component::handleEvent(const Event::Window::MaximizeEvent& e)
	{
		return handleNonMouseEvent(e);
	}
	Component::ShouldParentHandleEvent Component::handleEvent(const Event::Window::MinimizeEvent& e)
	{
		return handleNonMouseEvent(e);
	}
	Component::ShouldParentHandleEvent Component::handleEvent(const Event::Window::CloseEvent& e)
	{
		return handleNonMouseEvent(e);
	}
	Component::ShouldParentHandleEvent Component::handleEvent(const Event::Window::RestoreEvent& e)
	{
		return handleNonMouseEvent(e);
	}
}