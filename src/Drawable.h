#pragma once
#include <windows.h>

namespace AWC
{
	class Drawable
	{
	public:
		virtual void draw(HDC hdc) const = 0;
		virtual void redraw() = 0;
		virtual bool shouldRedraw() const = 0;
	};
}