#pragma once

#include "CoreEvent.h"

namespace Awincs
{
	namespace Event
	{
		namespace Window
		{
			struct Event
				:
				public CoreEvent
			{
			};

			struct ResizeEvent
				:
				public Event
			{
			};

			struct MoveEvent
				:
				public Event
			{
			};

			struct MinimizeEvent
				:
				public Event
			{
			};

			struct MaximizeEvent
				:
				public Event
			{
			};

			struct CloseEvent
				:
				public Event
			{
			};

			struct RestoreEvent
				:
				public Event
			{
			};
		}
	}
}