#pragma once

#include "pch.h"

namespace Awincs
{
	class Drawable
	{
	public:
		virtual void draw(Gdiplus::Graphics& gfx) const = 0;
	};
}