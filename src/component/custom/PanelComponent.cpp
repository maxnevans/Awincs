#include "../../pch.h"
#include "PanelComponent.h"

#include "../../Geometry.h"

bool Awincs::PanelComponent::checkAffiliationIgnoreChildren(const Point& pt) const
{
	return Geometry::IntRectangle::checkAffiliationIgnoreChildren(getAnchorPoint(), getDimensions(), pt);
}

void Awincs::PanelComponent::draw(gp::Graphics& gfx) const
{
	auto [width, height] = getDimensions();
	auto [x, y] = getGlobalAnchorPoint();

	gfx.FillRectangle(&gp::SolidBrush{ gp::Color{backgroundColor} },gp::Rect{ x, y, width, height });

	Component::draw(gfx);
}
