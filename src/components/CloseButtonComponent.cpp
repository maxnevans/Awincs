#include "pch.h"

#include "CloseButtonComponent.h"

#include "WindowStateHandler.h"

namespace Awincs
{
    CloseButtonComponent::CloseButtonComponent()
    {
        p_setBackgroundColor(DEFAULT_BACKGROUND_COLOR, ComponentState::DEFAULT);
        p_setBackgroundColor(DEFAULT_HOVER_BACKGROUND_COLOR, ComponentState::HOVER);
        p_setBackgroundColor(DEFAULT_ACTIVE_BACKGROUND_COLOR, ComponentState::ACTIVE);
        
        p_setTextColor(DEFAULT_TEXT_COLOR, ComponentState::DEFAULT);
        p_setTextColor(DEFAULT_HOVER_TEXT_COLOR, ComponentState::HOVER);
        p_setTextColor(DEFAULT_ACTIVE_TEXT_COLOR, ComponentState::ACTIVE);
        
        p_setText(L"x");
        p_setFontSize(16);
        p_setFontFamily(L"Lucida Console");
        p_setBold();
    }

    CloseButtonComponent::ShouldParentHandleEvent CloseButtonComponent::handleEvent(const Event::Mouse::ButtonEvent& e)
    {
        auto shouldHandle = ButtonComponent::handleEvent(e);
        expect(shouldHandle);

        if (e.action == Event::Mouse::ButtonAction::DOWN && e.buttonType == Event::Mouse::ButtonType::LEFT)
        {
            auto parent = getParent().lock();
            expect(parent);
            parent->closeWindow();
        }

        return true;
    }
}
