#include "../../pch.h"
#include "PanelComponent.h"

#include "../../Geometry.h"

void Awincs::PanelComponent::draw(gp::Graphics& gfx) const
{
	auto [width, height] = getDimensions();
	auto [x, y] = getGlobalAnchorPoint();

	gfx.FillRectangle(&gp::SolidBrush{ gp::Color{backgroundColor} },gp::Rect{ x, y, width, height });
}
