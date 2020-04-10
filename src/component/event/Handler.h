#pragma once

#include "Event.h"

namespace Awincs
{
	namespace ComponentEvent
	{
		class Handler
		{
		public:
			using ShouldParentHandleEvent = bool;

		public:
			virtual ShouldParentHandleEvent handleEvent(const Mouse::ButtonEvent&) { return true;  }
			virtual ShouldParentHandleEvent handleEvent(const Mouse::WheelEvent&) { return true; }
			virtual ShouldParentHandleEvent handleEvent(const Keyboard::KeyEvent&) { return true; }
			virtual ShouldParentHandleEvent handleEvent(const Keyboard::InputEvent&) { return true; }
		};
	}
}