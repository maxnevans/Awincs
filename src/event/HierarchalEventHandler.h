#pragma once

#include "Hierarchal.h"
#include "EventHandler.h"

namespace Awincs
{
	namespace Event
	{
		template<typename GEventHandler>
		class HierarchalEventHandler
			:
			public Hierarchal<GEventHandler>,
			public EventHandler
		{};
	}
}