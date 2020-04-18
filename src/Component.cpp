#include "pch.h"
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

	void Component::setParent(std::shared_ptr<Component> parent)
	{
		/* To avoid recursive call for redraw */
		redrawCallback = parent->redrawCallback;

		parent->addChild(shared_from_this());
		this->parent = parent;
	}

	void Component::unsetParent()
	{
		redrawCallback = nullptr;
		this->parent.reset();
	}

	void Component::redraw()
	{
		shouldRedraw = true;
		auto p = parent.lock();

		if (p)
			p->redraw();
	}

	bool Component::checkAffiliationIgnoreChildren(const Point& pt) const
	{
		return Geometry::IntRectangle::checkAffiliationIgnoreChildren(anchorPoint, dimensions, pt);
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

	bool Component::checkMousePoint(const Point& mousePoint) const
	{
		auto localMousePoint = mousePoint - getGlobalAnchorPoint();
		return checkAffiliationIgnoreChildren(localMousePoint);
	}

	void Component::closeWindow()
	{
		auto p = parent.lock();
		expect(p);
		p->closeWindow();
	}

	void Component::minimizeWindow()
	{
		auto p = parent.lock();
		expect(p);
		p->minimizeWindow();
	}

	void Component::maximizeWindow()
	{
		auto p = parent.lock();
		expect(p);
		p->maximizeWindow();
	}	

	const std::shared_ptr<Component>& Component::getParent() const
	{
		return parent.lock();
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
	void Component::p_draw(gp::Graphics& gfx)
	{
		bool hasBeenRedrawn = shouldRedraw;
		if (shouldRedraw)
		{
			shouldRedraw = false;
			draw(gfx);
		}
		
		for (const auto& child : children)
		{
			if (hasBeenRedrawn)
				child->shouldRedraw = true;

			child->p_draw(gfx);
		}
			
	}

	void Component::p_setRedrawCallback(RedrawCallback cb)
	{
		expect(!redrawCallback);
		redrawCallback = cb;
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
		bool isTheSameComponent = childHandler.get() == hoveredChildPrev.get();
		if (bothPresent && isTheSameComponent)
			return childHandler->handleEvent(adaptMouseEventToHandler(e, childHandler));

		/* Hovered child changed */

		/* Both are present */
		if (bothPresent)
		{
			/* Previous child should receive Event::Mouse::HoverEnd */
			hoveredChildPrev->handleEvent(Event::Mouse::HoverEnd{});

			/* New child should receive Event::Mouse::HoverStart */
			return childHandler->handleEvent(Event::Mouse::HoverStart{ adaptMouseEventToHandler(e, childHandler) });
		}

		/* One is present */

		if (childHandler)
		{
			/* New child should receive Event::Mouse::HoverStart */
			return childHandler->handleEvent(Event::Mouse::HoverStart{ adaptMouseEventToHandler(e, childHandler) });
		}

		if (hoveredChildPrev)
		{
			/* Previous child should receive Event::Mouse::HoverEnd */
			hoveredChildPrev->handleEvent(Event::Mouse::HoverEnd{});
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