#include "pch.h"
#include "InputComponent.h"

namespace Awincs
{
    void InputComponent::onTextChange(OnInputEvent cb)
    {
        onTextChangeCallback = cb;
    }

    const std::wstring& InputComponent::getText() const
    {
        return text;
    }

    void InputComponent::setText(std::wstring text)
    {
        this->text = text;
    }

    InputComponent::ShouldParentHandleEvent InputComponent::handleEvent(const Event::Keyboard::InputEvent& e)
    {
        auto shouldHandle = Component::handleEvent(e);
        expect(shouldHandle);

        if (e.character == L'\b')
        {
            if (!text.empty())
                text.pop_back();
        }
        else
        {
            text += e.character;
        }

        if (onTextChangeCallback)
            onTextChangeCallback(std::static_pointer_cast<InputComponent>(shared_from_this()));
            
        Component::redraw();

        return true;
    }

    InputComponent::ShouldParentHandleEvent InputComponent::handleEvent(const Event::Mouse::ButtonEvent& e)
    {
        bool shouldParentHandle = Component::handleEvent(e);
        expect(shouldParentHandle);

        setFocusOnThisComponent();

        Component::redraw();

        return true;
    }

    void InputComponent::draw(gp::Graphics& gfx) const
    {
        auto [width, height] = p_getDimensions();
        auto [x, y] = p_transformToGlobal(Point{0,0});

        gfx.FillRectangle(&gp::SolidBrush{ gp::Color{DEFAULT_BACKGROUND_COLOR} }, gp::Rect{ x, y, width, height });

        if (p_isFocused())
        {
            gfx.DrawRectangle(&gp::Pen{ gp::Color{DEFAULT_OUTLINE_COLOR} }, gp::Rect{ x, y, width, height });
        }

        gfx.DrawString(text.c_str(), static_cast<INT>(text.size()),
            &gp::Font{ DEFAULT_TEXT_FONT_FAMILY, DEFAULT_TEXT_SIZE },
            gp::PointF{ static_cast<gp::REAL>(x), static_cast<gp::REAL>(y) },
            &gp::SolidBrush(gp::Color{ DEFAULT_TEXT_COLOR }));
    }
}