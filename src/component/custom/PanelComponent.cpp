#include "../../pch.h"
#include "PanelComponent.h"

#include "../../Geometry.h"

bool Awincs::PanelComponent::checkAffiliationIgnoreChildren(const Point& pt) const
{
	return Geometry::IntRectangle::checkAffiliationIgnoreChildren(getAnchorPoint(), getDimensions(), pt);
}

void Awincs::PanelComponent::draw(HDC hdc) const
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
