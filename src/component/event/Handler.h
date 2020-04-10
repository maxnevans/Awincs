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
			virtual ShouldParentHandleEvent handleEvent(const Mouse::ButtonEvent&) = 0;
			virtual ShouldParentHandleEvent handleEvent(const Mouse::WheelEvent&) = 0;
			virtual ShouldParentHandleEvent handleEvent(const Keyboard::KeyEvent&) = 0;
			virtual ShouldParentHandleEvent handleEvent(const Keyboard::InputEvent&) = 0;
			virtual ShouldParentHandleEvent handleEvent(const Window::ResizeEvent&) = 0;
			virtual ShouldParentHandleEvent handleEvent(const Window::MoveEvent&) = 0;
			virtual ShouldParentHandleEvent handleEvent(const Window::MaximizeEvent&) = 0;
			virtual ShouldParentHandleEvent handleEvent(const Window::MinimizeEvent&) = 0;
			virtual ShouldParentHandleEvent handleEvent(const Window::CloseEvent&) = 0;
			virtual ShouldParentHandleEvent handleEvent(const Window::RestoreEvent&) = 0;
		};
	}
}