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
        virtual void setBackgroundColor(gp::ARGB color, ComponentState state = ComponentState::DEFAULT);
        virtual void setTextColor(gp::ARGB color, ComponentState state = ComponentState::DEFAULT);
        virtual void setCursorColor(gp::ARGB color, ComponentState state = ComponentState::DEFAULT);
        virtual void setOutlineColor(gp::ARGB color, ComponentState state = ComponentState::DEFAULT);
        virtual void setOutlineWidth(int px);
        virtual void setFontFamily(std::wstring fontFamily);
        virtual void setFontSize(int size);
        virtual void setFontUnit(gp::Unit unit);
        virtual void setBold(bool enabled = true);
        virtual void setItalic(bool enabled = true);

        /* Event handlers */
        virtual ShouldParentHandleEvent handleEvent(const Event::Keyboard::InputEvent&) override;
        virtual ShouldParentHandleEvent handleEvent(const Event::Mouse::ButtonEvent& e) override;

    protected:
        virtual void draw(gp::Graphics&) const override;
        void p_setBackgroundColor(gp::ARGB color, ComponentState state = ComponentState::DEFAULT);
        void p_setTextColor(gp::ARGB color, ComponentState state = ComponentState::DEFAULT);
        void p_setCursorColor(gp::ARGB color, ComponentState state = ComponentState::DEFAULT);
        void p_setOutlineColor(gp::ARGB color, ComponentState state = ComponentState::DEFAULT);
        void p_setOutlineWidth(int px);
        void p_setFontFamily(std::wstring fontFamily);
        void p_setFontSize(int size);
        void p_setFontUnit(gp::Unit unit);
        void p_setBold(bool enabled = true);
        void p_setItalic(bool enabled = true);

    private:
        static constexpr const wchar_t* DEFAULT_TEXT_FONT_FAMILY        = L"Tahoma";
        static constexpr gp::ARGB DEFAULT_OUTLINE_COLOR                 = makeARGB(45, 122, 186);
        static constexpr gp::ARGB DEFAULT_HOVER_OUTLINE_COLOR           = makeARGB(45, 122, 186);
        static constexpr gp::ARGB DEFAULT_ACTIVE_OUTLINE_COLOR          = makeARGB(45, 122, 186);
        static constexpr gp::ARGB DEFAULT_BACKGROUND_COLOR              = makeARGB(25, 50, 100);
        static constexpr gp::ARGB DEFAULT_HOVER_BACKGROUND_COLOR        = makeARGB(45, 122, 186);
        static constexpr gp::ARGB DEFAULT_ACTIVE_BACKGROUND_COLOR       = makeARGB(45, 122, 186);
        static constexpr gp::ARGB DEFAULT_CURSOR_COLOR                  = makeARGB(255, 255, 255);
        static constexpr gp::ARGB DEFAULT_HOVER_CURSOR_COLOR            = makeARGB(255, 255, 255);
        static constexpr gp::ARGB DEFAULT_ACTIVE_CURSOR_COLOR           = makeARGB(255, 255, 255);
        static constexpr gp::ARGB DEFAULT_TEXT_COLOR                    = makeARGB(170, 170, 170);
        static constexpr gp::ARGB DEFAULT_HOVER_TEXT_COLOR              = makeARGB(170, 170, 170);
        static constexpr gp::ARGB DEFAULT_ACTIVE_TEXT_COLOR             = makeARGB(170, 170, 170);
        static constexpr const gp::REAL DEFAULT_OUTLINE_WIDTH           = 1.f;
        static constexpr const gp::REAL DEFAULT_FONT_SIZE               = 14;
        static constexpr const gp::Unit DEFAULT_FONT_UNIT               = gp::Unit::UnitPixel;
        static constexpr const wchar_t* DEFAULT_FONT_FAMILY             = L"Tahoma";
        static constexpr const gp::FontStyle DEFAULT_FONT_STYLE         = gp::FontStyle::FontStyleRegular;

        std::map<ComponentState, gp::ARGB> outlineColors                = { {ComponentState::DEFAULT,   DEFAULT_OUTLINE_COLOR},
                                                                            {ComponentState::HOVER,     DEFAULT_HOVER_OUTLINE_COLOR}, 
                                                                            {ComponentState::ACTIVE,    DEFAULT_ACTIVE_OUTLINE_COLOR} };
        std::map<ComponentState, gp::ARGB> backgroundColors             = { {ComponentState::DEFAULT,   DEFAULT_BACKGROUND_COLOR},
                                                                            {ComponentState::HOVER,     DEFAULT_HOVER_BACKGROUND_COLOR},
                                                                            {ComponentState::ACTIVE,    DEFAULT_ACTIVE_BACKGROUND_COLOR} };
        std::map<ComponentState, gp::ARGB> cursorColors                 = { {ComponentState::DEFAULT,   DEFAULT_CURSOR_COLOR},
                                                                            {ComponentState::HOVER,     DEFAULT_HOVER_CURSOR_COLOR},
                                                                            {ComponentState::ACTIVE,    DEFAULT_ACTIVE_CURSOR_COLOR} };
        std::map<ComponentState, gp::ARGB> textColors                   = { {ComponentState::DEFAULT,   DEFAULT_TEXT_COLOR},
                                                                            {ComponentState::HOVER,     DEFAULT_HOVER_TEXT_COLOR},
                                                                            {ComponentState::ACTIVE,    DEFAULT_ACTIVE_TEXT_COLOR} };
        gp::REAL outlineWidth                                           = DEFAULT_OUTLINE_WIDTH;
        gp::Unit fontUnit                                               = DEFAULT_FONT_UNIT;
        gp::REAL fontSize                                               = DEFAULT_FONT_SIZE;
        std::wstring fontFamily                                         = DEFAULT_FONT_FAMILY;
        gp::FontStyle fontStyle                                         = DEFAULT_FONT_STYLE;
        bool isBold                                                     = false;
        bool isItalic                                                   = false;
        std::wstring text = L"";
        OnInputEvent onTextChangeCallback;
    };
}