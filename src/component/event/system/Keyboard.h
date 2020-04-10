#pragma once

#include "../CoreEvent.h"

namespace Awincs
{
	namespace ComponentEvent
	{
		namespace Keyboard
		{
			enum class ModificationKey
			{
				CTRL,
				SHIFT
			};

			struct Event
				:
				public CoreEvent
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
				public Event
			{
				KeyEventAction action = KeyEventAction::UNKNOWN;
				int keyCode = 0;
			};

			struct InputEvent
				:
				public Event
			{
				wchar_t character = L'\0';
			};
		}
	}
}