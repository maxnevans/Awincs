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

	void Component::setDimensions(const Dimensions& dims)
	{
		p_setDimensions(dims);
	}

	const Component::Dimensions& Component::getDimensions() const
	{
		return p_getDimensions();
	}

	void Component::setAnchorPoint(const Point& point)
	{
		p_setAnchorPoint(point);
	}

	const Component::Point& Component::getAnchorPoint() const
	{
		return p_getAnchorPoint();
	}

	Component::Point Component::getGlobalAnchorPoint() const
	{
		return p_getGlobalAnchorPoint();
	}

	Component::Point Component::transformToLocalPoint(const Point& point) const
	{
		return p_transformToLocalPoint(point);
	}

	void Component::p_setDimensions(const Component::Dimensions& dims)
	{
		this->dimensions = dims;
	}

	const Component::Dimensions& Component::p_getDimensions() const
	{
		return this->dimensions;
	}

	void Component::p_setAnchorPoint(const Component::Point& point)
	{
		this->anchorPoint = point;
	}

	const Component::Point& Component::p_getAnchorPoint() const
	{
		return this->anchorPoint;
	}

	Component::Point Component::p_getGlobalAnchorPoint() const
	{
		auto gPoint = anchorPoint;

		if (auto prnt = parent.lock())
			gPoint += parent.lock()->p_getGlobalAnchorPoint();

		return gPoint;
	}

	Component::Point Component::p_transformToLocalPoint(const Point& point) const
	{
		return point - anchorPoint;
	}

	bool Component::p_isFocused() const
	{
		if (focusedComponent && *focusedComponent)
			return (*focusedComponent).get() == this;

		return false;
	}

	std::shared_ptr<Component> Component::getFocusedComponent() const
	{
		return focusedComponent ? *focusedComponent : nullptr;
	}

	void Component::setFocusOnThisComponent()
	{
		p_setFocusOnThisComponent();
	}

	void Component::unsetFocusOnThisComponent()
	{
		p_unsetFocusFromThisComponent();
	}

	void Component::setParent(const std::shared_ptr<Component>& parent)
	{
		/* To avoid recursive call for redraw */
		redrawCallback = parent->redrawCallback;

		parent->addChild(shared_from_this());
		this->parent = parent;

		focusedComponent = parent->focusedComponent;
	}

	void Component::unsetParent()
	{
		redrawCallback = nullptr;
		this->parent.reset();

		focusedComponent = nullptr;
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
				if (child->checkAffiliationIgnoreChildren(p_transformToLocalPoint(p)))
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
	bool Component::isFocused() const
	{
		return p_isFocused();
	}
	gp::PointF Component::p_transformToGlobal(const gp::PointF& p) const
	{
		auto [x, y] = p_getGlobalAnchorPoint();
		return gp::PointF{ x + p.X, y + p.Y };
	}

	Component::Point Component::p_transformToGlobal(const Point& p) const
	{
		return p_getGlobalAnchorPoint() + p;
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

	void Component::p_setFocusOnThisComponent()
	{
		expect(focusedComponent);

		*focusedComponent = shared_from_this();
	}

	void Component::p_unsetFocusFromThisComponent()
	{
		expect(focusedComponent);
		*focusedComponent = nullptr;
	}

	void Component::p_setFocusComponentValue(std::shared_ptr<Component>* pComponent)
	{
		focusedComponent = pComponent;
	}

	ComponentState Component::p_getState() const
	{
		return state;
	}

	void Component::p_setState(ComponentState state)
	{
		this->state = state;
	}

	Component::ShouldParentHandleEvent Component::handleEvent(const Event::Mouse::ButtonEvent& e)
	{
		if (e.action == Event::Mouse::ButtonAction::DOWN)
		{
			state = ComponentState::ACTIVE;
			p_setFocusOnThisComponent();
		}
		else if (e.action == Event::Mouse::ButtonAction::UP)
		{
			state = ComponentState::HOVER;
		}

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
		state = ComponentState::HOVER;
		return handleMouseEvent(e);
	}
	Component::ShouldParentHandleEvent Component::handleEvent(const Event::Mouse::HoverEnd& e)
	{
		state = ComponentState::DEFAULT;
		return handleMouseEvent(e);
	}
	Component::ShouldParentHandleEvent Component::handleEvent(const Event::Keyboard::KeyEvent& e)
	{
		if (e.action == Event::Keyboard::KeyEventAction::DOWN && e.keyCode == VK_ESCAPE)
			p_unsetFocusFromThisComponent();

		return true;
	}
	Component::ShouldParentHandleEvent Component::handleEvent(const Event::Keyboard::InputEvent&)
	{
		/* Do nothing if this component is focused and got not keyboard event handler setup. */
		return true;
	}
	Component::ShouldParentHandleEvent Component::handleEvent(const Event::Window::ResizeEvent& e)
	{
		return handleWindowEvent(e);
	}
	Component::ShouldParentHandleEvent Component::handleEvent(const Event::Window::MoveEvent& e)
	{
		return handleWindowEvent(e);
	}
	Component::ShouldParentHandleEvent Component::handleEvent(const Event::Window::MaximizeEvent& e)
	{
		return handleWindowEvent(e);
	}
	Component::ShouldParentHandleEvent Component::handleEvent(const Event::Window::MinimizeEvent& e)
	{
		return handleWindowEvent(e);
	}
	Component::ShouldParentHandleEvent Component::handleEvent(const Event::Window::CloseEvent& e)
	{
		return handleWindowEvent(e);
	}
	Component::ShouldParentHandleEvent Component::handleEvent(const Event::Window::RestoreEvent& e)
	{
		return handleWindowEvent(e);
	}
}