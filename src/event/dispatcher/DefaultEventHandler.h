#pragma once

#include "../HierarchalEventHandler.h"

namespace Awincs
{
	namespace Event
	{
		template<typename GHandler>
		class DefaultEventHandler
			:
			public HierarchalEventHandler<GHandler>
		{
		};
	}
}