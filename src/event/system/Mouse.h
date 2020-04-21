#pragma once

#include "../CoreEvent.h"
#include "Keyboard.h"

namespace Awincs
{
    namespace ComponentEvent
    {
        namespace Mouse
        {
            enum class ButtonType
            {
                UNKNOWN,
                LEFT,
                RIGHT,
                MIDDLE,
                XBUTTON,
                YBUTTON
            };

            struct Event
                :
                public CoreEvent
            {
                Geometry::IntPoint2D point = {};
                std::set<Keyboard::ModificationKey> modificationKeys = {};
                std::set<ButtonType> pressedMouseButtons = {};
            };

            enum class ButtonAction
            {
                UNKNOWN,
                UP,
                DOWN
            };

            struct ButtonEvent
                :
                public Event
            {
                ButtonType buttonType = ButtonType::UNKNOWN;
                ButtonAction action = ButtonAction::UNKNOWN;
            };

            enum class ScrollDirection
            {
                UNKNOWN,
                UP,
                DOWN
            };

            struct Hover
                :
                public Event
            {

            };

            struct HoverStart
                :
                public Hover
            {
            };

            struct HoverEnd
                :
                public Hover
            {
            };

            struct WheelEvent
                :
                public Event
            {
                ScrollDirection direction = ScrollDirection::UNKNOWN;
                int amountScrolled = 0;
            };
        }
    }
}