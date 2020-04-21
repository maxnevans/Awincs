#pragma once

#include "pch.h"

#include "ButtonComponent.h"

namespace Awincs
{
    class CloseButtonComponent
        :
        public ButtonComponent
    {
    public:
        CloseButtonComponent();
        virtual ShouldParentHandleEvent handleEvent(const Event::Mouse::ButtonEvent&) override;

    protected:
        static constexpr gp::ARGB DEFAULT_BACKGROUND_COLOR                      = makeARGB(0x00, 0x00, 0x00, 0x00);
        static constexpr gp::ARGB DEFAULT_HOVER_BACKGROUND_COLOR                = makeARGB(0x00, 0x00, 0x00, 0x00);
        static constexpr gp::ARGB DEFAULT_ACTIVE_BACKGROUND_COLOR               = makeARGB(0xff, 207, 81, 43);
        static constexpr gp::ARGB DEFAULT_TEXT_COLOR                            = makeARGB(0xff, 224, 224, 224);
        static constexpr gp::ARGB DEFAULT_HOVER_TEXT_COLOR                      = makeARGB(0xff, 207, 81, 43);
        static constexpr gp::ARGB DEFAULT_ACTIVE_TEXT_COLOR                     = makeARGB(0xff, 224, 224, 224);
    };
}