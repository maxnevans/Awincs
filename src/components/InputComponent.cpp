#include "pch.h"
#include "InputComponent.h"

namespace Awincs
{
	void InputComponent::onValueChange(OnValueChangeCallback cb)
	{
		onValueChangeCallback = cb;
	}

	InputComponent::ShouldParentHandleEvent InputComponent::handleEvent(const Event::Keyboard::InputEvent& e)
	{
		auto shouldHandle = Component::handleEvent(e);
		expect(shouldHandle);

		if (e.character == L'\b')
		{
			if (!text.empty())
				text.pop_back();
		}
		else
		{
			text += e.character;
		}

		if (onValueChangeCallback)
			onValueChangeCallback(text);
			
		Component::redraw();

		return true;
	}

	InputComponent::ShouldParentHandleEvent InputComponent::handleEvent(const Event::Mouse::ButtonEvent& e)
	{
		bool shouldParentHandle = Component::handleEvent(e);
		expect(shouldParentHandle);

		Component::redraw();

		return true;
	}

	void InputComponent::draw(gp::Graphics& gfx) const
	{
		auto [width, height] = p_getDimensions();
		auto [x, y] = p_transformToGlobal(p_getAnchorPoint());

		gfx.FillRectangle(&gp::SolidBrush{ gp::Color{DEFAULT_BACKGROUND_COLOR} }, gp::Rect{ x, y, width, height });

		if (p_isFocused())
		{
			gfx.DrawRectangle(&gp::Pen{ gp::Color{DEFAULT_OUTLINE_COLOR} }, gp::Rect{ x, y, width, height });
		}

		gfx.DrawString(text.c_str(), static_cast<INT>(text.size()),
			&gp::Font{ DEFAULT_TEXT_FONT_FAMILY, DEFAULT_TEXT_SIZE },
			gp::PointF{ static_cast<gp::REAL>(x), static_cast<gp::REAL>(y) },
			&gp::SolidBrush(gp::Color{ DEFAULT_TEXT_COLOR }));
	}
}