#include "pch.h"
#include "ButtonComponent.h"

#include <DebugConsole.h>

#include "Geometry.h"

namespace Awincs
{
	void ButtonComponent::draw(gp::Graphics& gfx) const
	{
		auto [width, height] = getDimensions();
		auto [x, y] = getGlobalAnchorPoint();
		gfx.FillRectangle(&gp::SolidBrush{ gp::Color{backgroundColors.at(state)} }, gp::Rect{ x, y, width, height });
		gfx.DrawString(title.c_str(), static_cast<INT>(title.size()), 
			&gp::Font{ DEFAULT_TITLE_FONT_FAMILY, DEFAULT_TITLE_SIZE, 0, Gdiplus::UnitPixel }, 
			gp::PointF{ static_cast<gp::REAL>(x), static_cast<gp::REAL>(y) },
			&gp::SolidBrush(gp::Color{ titleColors.at(state) }));
	}

	void ButtonComponent::setBackgroundColor(gp::ARGB color, ComponentState state)
	{
		backgroundColors[state] = color;
	}

	void ButtonComponent::setTitleColor(gp::ARGB color, ComponentState state)
	{
		titleColors[state] = color;
	}

	void ButtonComponent::setTitle(const std::wstring& title)
	{
		this->title = title;
	}

	std::wstring ButtonComponent::getTitle() const
	{
		return title;
	}

	void ButtonComponent::onClick(std::function<void()>)
	{
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

	ButtonComponent::ShouldParentHandleEvent ButtonComponent::handleEvent(const Event::Mouse::HoverStart& e)
	{
		auto shouldHandle = Component::handleEvent(e);
		expect(shouldHandle);

		state = ComponentState::HOVER;

		Component::redraw();

		DCONSOLE(L"ButtonComponent: HoverStart\n");
		return true;
	}

	ButtonComponent::ShouldParentHandleEvent ButtonComponent::handleEvent(const Event::Mouse::HoverEnd& e)
	{
		auto shouldHandle = Component::handleEvent(e);
		expect(shouldHandle);

		state = ComponentState::DEFAULT;

		Component::redraw();

		DCONSOLE(L"ButtonComponent: HoverEnd\n");
		return true;
	}

	ButtonComponent::ShouldParentHandleEvent ButtonComponent::handleEvent(const Event::Mouse::Hover& e)
	{
		auto shouldHandle = Component::handleEvent(e);
		expect(shouldHandle);

		return true;
	}
}