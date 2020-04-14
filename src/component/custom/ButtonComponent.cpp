#include "../../pch.h"
#include "ButtonComponent.h"

#include <DebugConsole.h>

#include "../../Geometry.h"

namespace Awincs
{
	void ButtonComponent::draw(gp::Graphics& gfx) const
	{

		auto [width, height] = getDimensions();
		auto [x, y] = getGlobalAnchorPoint();

		gfx.FillRectangle(&gp::SolidBrush{ gp::Color{backgroundColor} }, gp::Rect{ x, y, width, height });		
		gfx.DrawString(title.c_str(), static_cast<INT>(title.size()), 
			&gp::Font{ DEFAULT_TITLE_FONT_FAMILY, DEFAULT_TITLE_SIZE }, 
			gp::PointF{ static_cast<gp::REAL>(x), static_cast<gp::REAL>(y) },
			&gp::SolidBrush(gp::Color{ DEFAULT_TITLE_COLOR }));

		Component::draw(gfx);
	}

	void ButtonComponent::setTitle(const std::wstring& title)
	{
		this->title = title;
	}

	std::wstring ButtonComponent::getTitle() const
	{
		return title;
	}

	bool ButtonComponent::checkAffiliationIgnoreChildren(const Point& pt) const
	{
		return Geometry::IntRectangle::checkAffiliationIgnoreChildren(getAnchorPoint(), getDimensions(), pt);
	}

	bool ButtonComponent::handleEvent(const ComponentEvent::Mouse::ButtonEvent& e)
	{
		bool shouldHandle = Component::handleEvent(e);

		expect(shouldHandle);

		DCONSOLE(L"Button component event handling: coordinates(" << e.point.x << L"," << e.point.y << L")\n");

		return true;
	}

	ButtonComponent::ShouldParentHandleEvent ButtonComponent::handleEvent(const Event::Keyboard::KeyEvent& e)
	{
		bool shouldHandle = Component::handleEvent(e);

		expect(shouldHandle);

		DCONSOLE(L"KeyEvent in button handler!\n");

		return true;
	}
}