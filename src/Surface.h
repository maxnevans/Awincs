#pragma once

#include "pch.h"

#include "Geometry.h"

namespace Awincs
{
	struct Color
	{
		unsigned char alpha;
		unsigned char red;
		unsigned char green;
		unsigned char blue;
	};

	using Dimensions = Geometry::IntDimensions2D;
	using Rectangle = Geometry::IntRectangle;

	class Surface
	{
	public:
		Surface(HDC hdc, const Dimensions& dimensions);
		virtual void fillRectangle(const Color&, const Rectangle&);
		virtual void drawText();

	private:
		Gdiplus::Graphics g;
		Gdiplus::SolidBrush sb;
	};
}