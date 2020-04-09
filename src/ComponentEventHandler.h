#pragma once

#include "components/ComponentEvent.h"

namespace Awincs
{
	namespace ComponentEvent
	{
		class ComponentEventHandler
		{
		public:
			using ShouldParentHandleEvent = bool;

		public:
			virtual ShouldParentHandleEvent handleEvent(const MouseButtonEvent&) { return true;  }
			virtual ShouldParentHandleEvent handleEvent(const MouseWheelEvent&) { return true; }
			virtual ShouldParentHandleEvent handleEvent(const KeyEvent&) { return true; }
			virtual ShouldParentHandleEvent handleEvent(const InputEvent&) { return true; }
		};
	}
}