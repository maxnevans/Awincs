#include "PanelComponent.h"
#include "../Geometry.h"

bool AWC::PanelComponent::checkAffiliation(const Point& pt) const
{
	return Geometry::IntRectangle::checkAffiliation(getAnchorPoint(), getDimensions(), pt);
}

void AWC::PanelComponent::draw(HDC hdc) const
{
	auto brush = CreateSolidBrush(backgroundColor);
	auto prevBrush = SelectObject(hdc, brush);

	auto [width, height] = getDimensions();
	auto [x, y] = getGlobalAnchorPoint();

	Rectangle(hdc, x, y, x + width, y + height);

	SelectObject(hdc, prevBrush);
	DeleteObject(brush);

	Component::draw(hdc);
}
