#include "pch.h"
#include "InputComponent.h"

#include "WindowController.h"

namespace Awincs
{
    void InputComponent::onTextChange(OnInputEvent cb)
    {
        onTextChangeCallback = cb;
    }

    void InputComponent::setBackgroundColor(gp::ARGB color, ComponentState state)
    {
        p_setBackgroundColor(color, state);
    }

    void InputComponent::setTextColor(gp::ARGB color, ComponentState state)
    {
        p_setTextColor(color, state);
    }

    void InputComponent::setCursorColor(gp::ARGB color, ComponentState state)
    {
        p_setCursorColor(color, state);
    }

    void InputComponent::setOutlineColor(gp::ARGB color, ComponentState state)
    {
        p_setOutlineColor(color, state);
    }

    void InputComponent::setOutlineWidth(int px)
    {
        p_setOutlineWidth(px);
    }

    void InputComponent::setFontFamily(std::wstring fontFamily)
    {
        p_setFontFamily(fontFamily);
    }

    void InputComponent::setFontSize(int size)
    {
        p_setFontSize(size);
    }

    void InputComponent::setFontUnit(gp::Unit unit)
    {
        p_setFontUnit(unit);
    }

    void InputComponent::setBold(bool enabled)
    {
        p_setBold(enabled);
    }

    void InputComponent::setItalic(bool enabled)
    {
        p_setItalic(enabled);
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

        return false;
    }

    void InputComponent::draw(gp::Graphics& gfx) const
    {
        auto [width, height] = p_getDimensions();
        auto [x, y] = p_transformToGlobal(Point{0,0});

        gfx.FillRectangle(&gp::SolidBrush{ gp::Color{backgroundColors.at(p_getState())} }, gp::Rect{ x, y, width, height });

        if (outlineWidth > 0 && p_isFocused())
        {
            gfx.DrawRectangle(&gp::Pen{ gp::Color{outlineColors.at(p_getState())}, outlineWidth }, gp::Rect{ x, y, width, height });
        }

        gfx.DrawString(text.c_str(), static_cast<INT>(text.size()),
            &gp::Font{ fontFamily.c_str(), fontSize, fontStyle, fontUnit},
            gp::PointF{ static_cast<gp::REAL>(x), static_cast<gp::REAL>(y) },
            &gp::SolidBrush(gp::Color{ textColors.at(p_getState()) }));
    }
    void InputComponent::p_setBackgroundColor(gp::ARGB color, ComponentState state)
    {
        backgroundColors[state] = color;
    }
    void InputComponent::p_setTextColor(gp::ARGB color, ComponentState state)
    {
        textColors[state] = color;
    }
    void InputComponent::p_setCursorColor(gp::ARGB color, ComponentState state)
    {
        cursorColors[state] = color;
    }
    void InputComponent::p_setOutlineColor(gp::ARGB color, ComponentState state)
    {
        outlineColors[state] = color;
    }
    void InputComponent::p_setOutlineWidth(int px)
    {
        outlineWidth = static_cast<gp::REAL>(px);
    }
    void InputComponent::p_setFontFamily(std::wstring fontFamily)
    {
        this->fontFamily = fontFamily;
    }
    void InputComponent::p_setFontSize(int size)
    {
        fontSize = static_cast<gp::REAL>(size);
    }
    void InputComponent::p_setFontUnit(gp::Unit unit)
    {
        fontUnit = unit;
    }
    void InputComponent::p_setBold(bool enabled)
    {
        isBold = enabled;

        if (isItalic)
        {
            fontStyle = enabled ? gp::FontStyle::FontStyleBoldItalic : gp::FontStyle::FontStyleItalic;
        }
        else
        {
            fontStyle = enabled ? gp::FontStyle::FontStyleBold : gp::FontStyle::FontStyleRegular;
        }
    }

    void InputComponent::p_setItalic(bool enabled)
    {
        isItalic = enabled;

        if (isBold)
        {
            fontStyle = enabled ? gp::FontStyle::FontStyleBoldItalic : gp::FontStyle::FontStyleBold;
        }
        else
        {
            fontStyle = enabled ? gp::FontStyle::FontStyleItalic : gp::FontStyle::FontStyleRegular;
        }
    }
}