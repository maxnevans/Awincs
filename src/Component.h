#pragma once

#include "pch.h"

#include "Geometry.h"
#include "event/Handler.h"
#include "event/Event.h"
#include "WindowStateHandler.h"

namespace Awincs
{
    namespace
    {
        namespace Event = ComponentEvent;
        namespace gp = Gdiplus;

        static constexpr const int makeARGB(int a, int r, int g, int b)
        {
            return (((gp::ARGB)(b) << gp::Color::BlueShift) |
                ((gp::ARGB)(g) << gp::Color::GreenShift) |
                ((gp::ARGB)(r) << gp::Color::RedShift) |
                ((gp::ARGB)(a) << gp::Color::AlphaShift));
        }
        static constexpr const int makeARGB(int r, int g, int b)
        {
            return makeARGB(0xff, r, g, b);
        }
        static constexpr const int setARGBOpacity(int a, gp::ARGB color)
        {
            return color & ~(0xff << gp::Color::AlphaShift) & ((gp::ARGB)(a) << gp::Color::AlphaShift);
        }
        static constexpr const int getARGBOpacity(gp::ARGB color)
        {
            return (color & (0xff << gp::Color::AlphaShift)) >> gp::Color::AlphaShift;
        }
    }

    enum class ComponentState
    {
        DEFAULT,
        HOVER,
        ACTIVE
    };

    class WindowController;

    class Component
        :
        public std::enable_shared_from_this<Component>,
        public ComponentEvent::Handler,
        public WindowStateHandler
    {
    public:
        using Point = Geometry::IntPoint2D;
        using Dimensions = Geometry::IntDimensions2D;
        using ShouldParentHandleEvent    = Event::Handler::ShouldParentHandleEvent;
        using RedrawCallback = std::function<void()>;
        template<typename GComponent>
        using OnEventCallback = std::function<void(std::shared_ptr<GComponent>)>;
        using OnStateChangeCallback = std::function<void(ComponentState current, ComponentState next)>;
        using OnFocusChangeCallback = std::function<void(bool isFocused)>;
        using OnWindowControllerChange = std::function<void(std::shared_ptr<WindowController>)>;
        using OnDimensionsChange = std::function<void(const Dimensions& current, const Dimensions& next)>;
        using OnAnchorPointChange = std::function<void(const Point& current, const Point& next)>;

    public:
        Component();
        Component(const Point& anchorPoint, const Dimensions& dims);
        virtual void setDimensions(const Dimensions& dims);
        virtual const Dimensions& getDimensions() const;
        virtual void setAnchorPoint(const Point& point);
        virtual const Point& getAnchorPoint() const;
        Point getGlobalAnchorPoint() const;
        Point transformToLocalPoint(const Point& point) const;
        virtual std::shared_ptr<Component> getFocusedComponent() const;
        virtual void setFocusOnThisComponent();
        virtual void unsetFocusOnThisComponent();
        virtual void setParent(const std::shared_ptr<Component>& parent);
        virtual void unsetParent();
        virtual void redraw();
        virtual bool checkAffiliationIgnoreChildren(const Point& pt) const;
        virtual bool checkAffiliationDontIgnoreChildren(const Point&) const;
        virtual void closeWindow() override;
        virtual void minimizeWindow() override;
        virtual void maximizeWindow() override;
        virtual bool isFocused() const;
        virtual std::shared_ptr<WindowController> getWindowController() const;
        virtual ShouldParentHandleEvent handleEvent(const Event::Mouse::ButtonEvent&) override;
        virtual ShouldParentHandleEvent handleEvent(const Event::Mouse::WheelEvent&) override;
        virtual ShouldParentHandleEvent handleEvent(const Event::Mouse::Hover&) override;
        virtual ShouldParentHandleEvent handleEvent(const Event::Mouse::HoverStart&) override;
        virtual ShouldParentHandleEvent handleEvent(const Event::Mouse::HoverEnd&) override;
        virtual ShouldParentHandleEvent handleEvent(const Event::Keyboard::KeyEvent&) override;
        virtual ShouldParentHandleEvent handleEvent(const Event::Keyboard::InputEvent&) override;
        virtual ShouldParentHandleEvent handleEvent(const Event::Window::ResizeEvent&) override;
        virtual ShouldParentHandleEvent handleEvent(const Event::Window::MoveEvent&) override;
        virtual ShouldParentHandleEvent handleEvent(const Event::Window::MaximizeEvent&) override;
        virtual ShouldParentHandleEvent handleEvent(const Event::Window::MinimizeEvent&) override;
        virtual ShouldParentHandleEvent handleEvent(const Event::Window::CloseEvent&) override;
        virtual ShouldParentHandleEvent handleEvent(const Event::Window::RestoreEvent&) override;

    protected:
        virtual void draw(gp::Graphics&) const {}
        virtual gp::PointF p_transformToGlobal(const gp::PointF& p) const;
        template<typename TPoint>
        Geometry::Point2D<TPoint> p_transformToGlobal(const Geometry::Point2D<TPoint>& p) const
        {
            auto pp = p;
            auto g = p_getGlobalAnchorPoint();
            pp.x += static_cast<TPoint>(g.x);
            pp.y += static_cast<TPoint>(g.y);
            return  pp;
        }
        std::shared_ptr<Component> getParent();
        virtual void addChild(const std::shared_ptr<Component>& child);
        virtual void removeChild(const std::shared_ptr<Component>& child);
        virtual void p_draw(gp::Graphics& gfx);
        void p_setRedrawCallback(RedrawCallback cb);
        void p_setFocusOnThisComponent();
        void p_unsetFocusFromThisComponent();
        void p_setFocusComponentValue(std::shared_ptr<Component>*);
        ComponentState p_getState() const;
        void p_setState(ComponentState state);
        virtual void p_setDimensions(const Dimensions& dims);
        virtual const Dimensions& p_getDimensions() const;
        virtual void p_setAnchorPoint(const Point& point);
        virtual const Point& p_getAnchorPoint() const;
        Point p_getGlobalAnchorPoint() const;
        Point p_transformToLocalPoint(const Point& point) const;
        bool p_isFocused() const;
        void p_onStateChange(OnStateChangeCallback cb);
        void p_onFocusChange(OnFocusChangeCallback cb);
        void p_onWindowControllerChange(OnWindowControllerChange cb);
        virtual std::shared_ptr<WindowController> p_getWindowController();
        virtual void p_setWindowController(const std::shared_ptr<WindowController>&);
        void p_setParent(const std::shared_ptr<Component>& parent);
        void p_unsetParent();
        void p_onDimensionsChange(OnDimensionsChange cb);
        void p_onAnchorPointChange(OnAnchorPointChange cb);

    private:
        template<typename GMouseEvent>
        // requires std::is_convertable<GMouseEvent, Event::Mouse::Event>::value_type
        std::shared_ptr<Component> getMouseEventComponentHandler(const std::shared_ptr<Component>& root, GMouseEvent& e) const
        {
            if (!root->children.empty())
                for (const auto& child : root->children)
                    if (child->checkAffiliationIgnoreChildren(e.point))
                    {
                        adaptMouseEventToHandler(e, child);
                        return child;
                    }

            return nullptr;
        }

        template<typename GMouseEvent>
        // requires std::is_convertable<GMouseEvent, ComponentEvent::MouseEvent>::value_type
        ShouldParentHandleEvent handleMouseEvent(const GMouseEvent& e)
        {
            auto eAdapted = e;
            auto handler = getMouseEventComponentHandler(shared_from_this(), eAdapted);
            
            if (handler)
                return handler->handleEvent(eAdapted);

            return true;
        }

        template<typename GWindowEvent>
        // requires std::is_convertable<GWindowEvent, Event::Window::Event>::value_type
        ShouldParentHandleEvent handleWindowEvent(const GWindowEvent& e)
        {
            bool shouldComponentHandleEvent = true;

            for (const auto& child : children)
                shouldComponentHandleEvent = child->handleEvent(e) && shouldComponentHandleEvent;

            return shouldComponentHandleEvent;
        }

        template<typename GMouseEvent>
        GMouseEvent adaptMouseEventToHandler(GMouseEvent& e, const std::shared_ptr<Component>& handler) const
        {
            e.point = handler->p_transformToLocalPoint(e.point);
            return e;
        }

    private:
        static constexpr Point DEFAULT_ANCHOR_POINT                     = { 0, 0 };
        static constexpr Dimensions DEFAULT_DIMENSIONS                  = { 0, 0 };

        Point anchorPoint                                               = DEFAULT_ANCHOR_POINT;
        Dimensions dimensions                                           = DEFAULT_DIMENSIONS;
        bool shouldRedraw                                               = true;
        std::shared_ptr<Component> hoveredChild                         = nullptr;
        RedrawCallback redrawCallback                                   = nullptr;
        std::shared_ptr<Component>* focusedComponent                    = nullptr;
        ComponentState state                                            = ComponentState::DEFAULT;
        OnStateChangeCallback onStateChangeCallback                     = nullptr;
        OnFocusChangeCallback onFocusChangeCallback                     = nullptr;
        OnWindowControllerChange onWindowControllerChange               = nullptr;
        OnDimensionsChange onDimensionsChange                           = nullptr;
        OnAnchorPointChange onAnchorPointChange                         = nullptr;
        std::vector<std::shared_ptr<Component>> children;
        std::weak_ptr<Component> parent;
        std::weak_ptr<WindowController> windowController;
    };
}