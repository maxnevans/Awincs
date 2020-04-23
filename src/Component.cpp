#include "pch.h"
#include "Component.h"

#include "WindowController.h"
#include "ComponentException.h"

namespace Awincs
{
    Component::Component()
        :
        Component({}, {})
    {
    }

    Component::Component(const Point& anchorPoint, const Dimensions& dims)
        :
        std::enable_shared_from_this<Component>()
    {
        this->anchorPoint = anchorPoint;
        this->dimensions = dims;

        p_onStateChange([this](ComponentState current, ComponentState next) {
            Component::redraw();
        });
        p_onFocusChange([this](bool isFocused) {
            Component::redraw();
        });
    }

    void Component::show()
    {
        throw ComponentException(L"Not implemented yet! Please use setParent and unsetParent instead!");
        //p_show();
    }

    void Component::hide()
    {
        throw ComponentException(L"Not implemented yet! Please use setParent and unsetParent instead!");
        //p_hide();
    }

    bool Component::isShown()
    {
        return p_isShown();
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
        if (dims != this->dimensions && onDimensionsChange)
            onDimensionsChange(this->dimensions, dims);

        this->dimensions = dims;
    }

    const Component::Dimensions& Component::p_getDimensions() const
    {
        return this->dimensions;
    }

    void Component::p_setAnchorPoint(const Component::Point& point)
    {
        if (this->anchorPoint != point && onAnchorPointChange)
            onAnchorPointChange(this->anchorPoint, point);

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

    void Component::p_onStateChange(OnStateChangeCallback cb)
    {
        onStateChangeCallback = cb;
    }

    void Component::p_onFocusChange(OnFocusChangeCallback cb)
    {
        onFocusChangeCallback = cb;
    }

    void Component::p_onWindowControllerChange(OnWindowControllerChange cb)
    {
        onWindowControllerChange = cb;
    }

    std::shared_ptr<WindowController> Component::p_getWindowController()
    {
        return windowController.lock();
    }

    void Component::p_setWindowController(const std::shared_ptr<WindowController>& wc)
    {
        if (windowController.lock() != wc)
        {
            for (auto& child : children)
                child->p_setWindowController(wc);

            if (onWindowControllerChange)
                onWindowControllerChange(wc);
        }
        windowController = wc;
    }

    void Component::p_setParent(const std::shared_ptr<Component>& parent)
    {
        /* To avoid recursive call for redraw */
        redrawCallback = parent->redrawCallback;

        parent->p_addChild(shared_from_this());
        this->parent = parent;

        p_setFocusComponentValue(parent->focusedComponent);
        p_setWindowController(parent->p_getWindowController());
    }

    void Component::p_unsetParent()
    {
        redrawCallback = nullptr;
        if (auto p = parent.lock())
            p->p_removeChild(shared_from_this());

        parent.reset();

        p_setFocusComponentValue(nullptr);
        p_setWindowController(nullptr);
    }

    void Component::p_onDimensionsChange(OnDimensionsChange cb)
    {
        onDimensionsChange = cb;
    }

    void Component::p_onAnchorPointChange(OnAnchorPointChange cb)
    {
        onAnchorPointChange = cb;
    }

    void Component::p_show()
    {
        m_isShown = true;
    }

    void Component::p_hide()
    {
        m_isShown = false;
    }

    bool Component::p_isShown()
    {
        return m_isShown;
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
        p_setParent(parent);
    }

    void Component::unsetParent()
    {
        p_unsetParent();
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
        auto parent = p_getParent();
        expect(parent);
        parent->closeWindow();
    }

    void Component::minimizeWindow()
    {
        auto parent = p_getParent();
        expect(parent);
        parent->minimizeWindow();
    }

    void Component::maximizeWindow()
    {
        auto parent = p_getParent();
        expect(parent);
        parent->maximizeWindow();
    }
    bool Component::isFocused() const
    {
        return p_isFocused();
    }
    std::shared_ptr<WindowController> Component::getWindowController() const
    {
        return windowController.lock();
    }
    gp::PointF Component::p_transformToGlobal(const gp::PointF& p) const
    {
        auto [x, y] = p_getGlobalAnchorPoint();
        return gp::PointF{ x + p.X, y + p.Y };
    }

    std::shared_ptr<Component> Component::p_getParent()
    {
        return parent.lock();
    }

    void Component::p_addChild(const std::shared_ptr<Component>& child)
    {
        children.push_back(child);
    }

    void Component::p_removeChild(const std::shared_ptr<Component>& child)
    {
        auto condition = [child](const std::shared_ptr<Component>& c) { return child.get() == c.get(); };
        auto index = std::find_if(children.begin(), children.end(), condition);

        expect(index != children.end());

        children.erase(index);
    }
    void Component::p_draw(gp::Graphics& gfx)
    {
        if (m_isShown)
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

        if (onFocusChangeCallback)
            onFocusChangeCallback(true);
    }

    void Component::p_unsetFocusFromThisComponent()
    {
        expect(focusedComponent);

        *focusedComponent = nullptr;

        if (onFocusChangeCallback)
            onFocusChangeCallback(false);
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
        if (onStateChangeCallback)
            onStateChangeCallback(this->state, state);

        this->state = state;
    }

    Component::ShouldParentHandleEvent Component::handleEvent(const Event::Mouse::ButtonEvent& e)
    {
        if (e.action == Event::Mouse::ButtonAction::DOWN)
        {
            p_setState(ComponentState::ACTIVE);
        }
        else if (e.action == Event::Mouse::ButtonAction::UP)
        {
            p_setState(ComponentState::HOVER);
        }

        return handleMouseEvent(e);
    }

    Component::ShouldParentHandleEvent Component::handleEvent(const Event::Mouse::WheelEvent& e)
    {
        return handleMouseEvent(e);
    }
    Component::ShouldParentHandleEvent Component::handleEvent(const Event::Mouse::Hover& event)
    {
        auto eAdapted = event;
        auto childHandler = getMouseEventComponentHandler(shared_from_this(), eAdapted);
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
            return childHandler->handleEvent(eAdapted);

        /* Hovered child changed */

        /* Both are present */
        if (bothPresent)
        {
            /* Previous child should receive Event::Mouse::HoverEnd */
            hoveredChildPrev->handleEvent(Event::Mouse::HoverEnd{});

            /* New child should receive Event::Mouse::HoverStart */
            return childHandler->handleEvent(Event::Mouse::HoverStart{ eAdapted });
        }

        /* One is present */

        if (childHandler)
        {
            /* New child should receive Event::Mouse::HoverStart */
            return childHandler->handleEvent(Event::Mouse::HoverStart{ eAdapted });
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
       p_setState(ComponentState::HOVER);
        return handleMouseEvent(e);
    }
    Component::ShouldParentHandleEvent Component::handleEvent(const Event::Mouse::HoverEnd& e)
    {
        p_setState(ComponentState::DEFAULT);
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