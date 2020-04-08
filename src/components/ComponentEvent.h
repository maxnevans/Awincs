#pragma once
#include "../base/DebugUntils.h"
#include "../Geometry.h"

namespace Awincs
{
	namespace ComponentEvent
	{
		enum class EventType
		{
			UNKNOWN,
			MOUSE,
			KEY
		};

		struct Event
		{
			EventType eventType = EventType::UNKNOWN;
		};

		enum class MouseEventType
		{
			UNKNOWN,
			UP,
			DOWN,
			MOVE,
			HOVER,
			LEAVE,
			CLICK,
			DBL_CLICK
		};

		enum class ModificationKey
		{
			NONE,
			SHIFT,
			CTRL,
			ALT
		};

		struct MouseEvent
			:
			public Event
		{
			MouseEvent() { eventType = EventType::MOUSE; }
			MouseEventType mouseEventType = MouseEventType::UNKNOWN;
			ModificationKey modKeys = ModificationKey::NONE;
			Geometry::IntPoint2D point = {};
		};

		enum class KeyEventType
		{
			UNKNOWN,
			UP,
			DOWN,
			INPUT
		};

		struct KeyEvent
			:
			public Event
		{
			KeyEvent() { eventType = EventType::KEY; }
			KeyEventType keyEventType = KeyEventType::UNKNOWN;
			ModificationKey modKeys = ModificationKey::NONE;
			int keyCode = 0;
		};
	}
}