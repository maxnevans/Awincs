#include "pch.h"
#include "ButtonComponent.h"

#include <DebugConsole.h>

#include "Geometry.h"

namespace Awincs
{
    ButtonComponent::ButtonComponent()
        :
        PanelComponent()
    {
        p_showText();
        p_setTextAlignment(DEFAULT_HORIZONTAL_ALIGNMENT, DEFAULT_VERTICAL_ALIGNMENT);
    }

    void ButtonComponent::setDimensions(const Dimensions& d)
    {
        PanelComponent::p_setDimensions(d);

        auto [horizontal, vertical] = p_getTextAlignment();

        Point point = p_getTextAnchorPoint();

        if (horizontal == gp::StringAlignment::StringAlignmentCenter)
            point.x = d.width / 2;

        if (vertical == gp::StringAlignment::StringAlignmentCenter)
            point.y = d.height / 2;

        p_setTextAnchorPoint(point);
    }

    void ButtonComponent::onClick(OnClickCallback cb)
    {
        onClickCallback = cb;
    }

    bool ButtonComponent::handleEvent(const ComponentEvent::Mouse::ButtonEvent& e)
    {
        bool shouldHandle = Component::handleEvent(e);
        expect(shouldHandle);

        if (e.buttonType == Event::Mouse::ButtonType::LEFT && e.action == Event::Mouse::ButtonAction::DOWN)
        {
            if (onClickCallback)
                onClickCallback(e.point);
        }

        DCONSOLE(L"Button component event handling: coordinates(" << e.point.x << L"," << e.point.y << L")\n");

        return true;
    }

    ButtonComponent::ShouldParentHandleEvent ButtonComponent::handleEvent(const Event::Keyboard::KeyEvent& e)
    {
        bool shouldHandle = Component::handleEvent(e);
        expect(shouldHandle);

        DCONSOLE(L"KeyEvent in button handler!\n");

        return true;
    }

    ButtonComponent::ShouldParentHandleEvent ButtonComponent::handleEvent(const Event::Mouse::HoverStart& e)
    {
        auto shouldHandle = Component::handleEvent(e);
        expect(shouldHandle);

        Component::redraw();

        DCONSOLE(L"ButtonComponent: HoverStart\n");
        return true;
    }

    ButtonComponent::ShouldParentHandleEvent ButtonComponent::handleEvent(const Event::Mouse::HoverEnd& e)
    {
        auto shouldHandle = Component::handleEvent(e);
        expect(shouldHandle);

        Component::redraw();

        DCONSOLE(L"ButtonComponent: HoverEnd\n");
        return true;
    }

    ButtonComponent::ShouldParentHandleEvent ButtonComponent::handleEvent(const Event::Mouse::Hover& e)
    {
        auto shouldHandle = Component::handleEvent(e);
        expect(shouldHandle);

        return true;
    }
}