#pragma once

#include "CoreEvent.h"

namespace Awincs
{
	namespace Event
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
				KeyEvent(KeyEventAction action = KeyEventAction::UNKNOWN, int keyCode = 0)
					:
					Event(),
					action(action),
					keyCode(keyCode)
				{}
				KeyEventAction action = KeyEventAction::UNKNOWN;
				int keyCode = 0;
			};

			struct InputEvent
				:
				public Event
			{
				InputEvent(wchar_t character = L'\0')
					:
					Event(),
					character(character)
				{}
				wchar_t character = L'\0';
			};
		}
	}
}