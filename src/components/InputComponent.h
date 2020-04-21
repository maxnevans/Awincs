#pragma once

#include "pch.h"

#include "../Component.h"

namespace Awincs
{
    class InputComponent
        :
        public Component
    {
    public:
        using OnInputEvent = OnEventCallback<InputComponent>;

    public:
        virtual const std::wstring& getText() const;
        virtual void setText(std::wstring text);
        virtual void onTextChange(OnInputEvent cb);
        virtual ShouldParentHandleEvent handleEvent(const Event::Keyboard::InputEvent&) override;
        virtual ShouldParentHandleEvent handleEvent(const Event::Mouse::ButtonEvent& e) override;

    protected:
        virtual void draw(gp::Graphics&) const override;

    private:
        static constexpr gp::ARGB DEFAULT_BACKGROUND_COLOR              = makeARGB(25, 50, 100);
        static constexpr gp::ARGB DEFAULT_CURSOR_COLOR                  = makeARGB(255, 255, 255);
        static constexpr gp::ARGB DEFAULT_TEXT_COLOR                    = makeARGB(170, 170, 170);
        static constexpr const wchar_t* DEFAULT_TEXT_FONT_FAMILY        = L"Tahoma";
        static constexpr gp::REAL DEFAULT_TEXT_SIZE                     = 17;
        static constexpr gp::ARGB DEFAULT_OUTLINE_COLOR                 = makeARGB(45, 122, 186);

        std::wstring text = L"";
        OnInputEvent onTextChangeCallback;
    };
}