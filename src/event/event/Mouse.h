#pragma once

#include "CoreEvent.h"
#include "Keyboard.h"

namespace Awincs
{
	namespace Event
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
				Event(Geometry::IntPoint2D point = {}, std::set<Keyboard::ModificationKey> modificationKeys = {}, std::set<ButtonType> pressedMouseButtons = {})
					:
					point(point),
					modificationKeys(modificationKeys),
					pressedMouseButtons(pressedMouseButtons)
				{}
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
				ButtonEvent(ButtonType buttonType = ButtonType::UNKNOWN, ButtonAction action = ButtonAction::UNKNOWN)
					:
					Event(),
					buttonType(buttonType),
					action(action)
				{}
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
				HoverStart() = default;
				HoverStart(const Hover& hover)
					:
					Hover(hover)
				{}
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
				WheelEvent(ScrollDirection direction = ScrollDirection::UNKNOWN, int amountScrolled = 0)
					:
					Event(),
					direction(direction),
					amountScrolled(amountScrolled)
				{}
				ScrollDirection direction = ScrollDirection::UNKNOWN;
				int amountScrolled = 0;
			};
		}
	}
}