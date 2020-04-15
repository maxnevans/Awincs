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
		auto gPoint = anchorPoint;

		if (auto prnt = parent.lock())
			gPoint += parent.lock()->getGlobalAnchorPoint();

		return gPoint;
	}

	Component::Point Component::transformToLocalPoint(const Point& point) const
	{
		return point - anchorPoint;
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

	void Component::draw(Gdiplus::Graphics& gfx) const
	{
		for (const auto& component : children)
			component->draw(gfx);
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

		auto removed = children.erase(index);

		expect(removed != children.end());
	}
	Component::ShouldParentHandleEvent Component::handleEvent(const Event::Mouse::ButtonEvent& e)
	{
		return handleMouseEvent(e);
	}
	Component::ShouldParentHandleEvent Component::handleEvent(const Event::Mouse::WheelEvent& e)
	{
		return handleMouseEvent(e);
	}
	Component::ShouldParentHandleEvent Component::handleEvent(const Event::Mouse::Hover& e)
	{
		auto childHandler = getMouseEventComponentHandler(e);
		bool bothPresent = childHandler && hoveredChild;
		bool bothDoesNotPresent = !childHandler && !hoveredChild;

		/* Save prev child */
		auto hoveredChildPrev = hoveredChild;
		hoveredChild = childHandler;

		/* Hover on empty area (WindowController area) */
		if (bothDoesNotPresent)
			return true;

		/* Hover on same component */
		bool isTheSameComponent = &**childHandler == &**hoveredChildPrev;
		if (bothPresent && isTheSameComponent)
			return (*childHandler)->handleEvent(adaptMouseEventToHandler(e, *childHandler));

		/* Hovered child changed */

		/* Both are present */
		if (bothPresent)
		{
			/* Previous child should receive Event::Mouse::HoverEnd */
			(*hoveredChildPrev)->handleEvent(Event::Mouse::HoverEnd{});

			/* New child should receive Event::Mouse::HoverStart */
			return (*childHandler)->handleEvent(Event::Mouse::HoverStart{ adaptMouseEventToHandler(e, *childHandler) });
		}

		/* One is present */

		if (childHandler)
		{
			/* New child should receive Event::Mouse::HoverStart */
			return (*childHandler)->handleEvent(Event::Mouse::HoverStart{ adaptMouseEventToHandler(e, *childHandler) });
		}

		if (hoveredChildPrev)
		{
			/* Previous child should receive Event::Mouse::HoverEnd */
			(*hoveredChildPrev)->handleEvent(Event::Mouse::HoverEnd{});
			return true;
		}

		expect(false);
		/* Fallback default return: will never hit this return */
		return true;
	}
	Component::ShouldParentHandleEvent Component::handleEvent(const Event::Mouse::HoverStart& e)
	{
		return handleMouseEvent(e);
	}
	Component::ShouldParentHandleEvent Component::handleEvent(const Event::Mouse::HoverEnd& e)
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