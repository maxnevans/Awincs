#pragma once

#include "../HierarchalEventHandler.h"
#include "Geometry.h"

namespace Awincs
{
	namespace Event
	{
		template<typename GHandler>
		class MouseEventHandler
			:
			public HierarchalEventHandler<GHandler>
		{
		public:
			using Point = Geometry::IntPoint2D;

		public:
			virtual bool checkMousePoint(const Point&) const = 0;
		};
	}
}