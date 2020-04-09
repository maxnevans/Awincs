#pragma once

#include "../pch.h"

#include "../base/DebugUntils.h"
#include "../Geometry.h"

namespace Awincs
{
	namespace ComponentEvent
	{
		struct Event
		{
		};

		enum class ModificationKey
		{
			CTRL,
			SHIFT
		};

		enum class MouseButtonType
		{
			UNKNOWN,
			LEFT,
			RIGHT,
			MIDDLE,
			XBUTTON,
			YBUTTON
		};

		struct MouseEvent
			:
			public Event
		{
			Geometry::IntPoint2D point = {};
			std::set<ModificationKey> modificationKeys = {};
			std::set<MouseButtonType> pressedMouseButtons = {};
		};

		enum class MouseButtonAction
		{
			UNKNOWN,
			UP,
			DOWN
		};

		struct MouseButtonEvent
			:
			public MouseEvent
		{
			MouseButtonType buttonType = MouseButtonType::UNKNOWN;
			MouseButtonAction action = MouseButtonAction::UNKNOWN;
		};

		enum class ScrollDirection
		{
			UNKNOWN,
			UP,
			DOWN
		};

		struct MouseWheelEvent
			:
			public MouseEvent
		{
			ScrollDirection direction = ScrollDirection::UNKNOWN;
			int amountScrolled = 0;
		};

		struct KeyboardEvent
			:
			public Event
		{
		};

		enum class KeyEventAction
		{
			UNKNOWN,
			UP,
			DOWN
		};

		struct KeyEvent
			:
			public KeyboardEvent
		{
			KeyEventAction action = KeyEventAction::UNKNOWN;
			int keyCode = 0;
		};

		struct InputEvent
			:
			public KeyboardEvent
		{
			wchar_t character = 0;
		};
	}
}