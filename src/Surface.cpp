#include "pch.h"
#include "Surface.h"

Awincs::Surface::Surface(HDC hdc, const Dimensions& dimensions)
	:
	g(hdc),
	sb(Gdiplus::Color{})
{
}

void Awincs::Surface::fillRectangle(const Color& c, const Rectangle& r)
{
	Gdiplus::Color color { 
		c.alpha == 255 ? c.alpha - 1 : c.alpha,
		c.red, 
		c.green, 
		c.blue
	};
	sb.SetColor(color);

	auto [x, y] = r.getAnchorPoint();
	auto [width, height] = r.getDimensions();
	Gdiplus::Rect rect{x,y,width,height};

	g.FillRectangle(&sb, rect);
}

void Awincs::Surface::drawText()
{
	static_assert(false);
}
