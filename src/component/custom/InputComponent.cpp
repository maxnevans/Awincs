#include "../../pch.h"
#include "InputComponent.h"

namespace Awincs
{
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
			
		Component::redraw();

		return true;
	}

	void InputComponent::draw(gp::Graphics& gfx) const
	{
		auto [width, height] = getDimensions();
		auto [x, y] = getGlobalAnchorPoint();

		gfx.FillRectangle(&gp::SolidBrush{ gp::Color{DEFAULT_BACKGROUND_COLOR} }, gp::Rect{ x, y, width, height });
		gfx.DrawString(text.c_str(), static_cast<INT>(text.size()),
			&gp::Font{ DEFAULT_TEXT_FONT_FAMILY, DEFAULT_TEXT_SIZE },
			gp::PointF{ static_cast<gp::REAL>(x), static_cast<gp::REAL>(y) },
			&gp::SolidBrush(gp::Color{ DEFAULT_TEXT_COLOR }));

		Component::draw(gfx);
	}
	HFONT InputComponent::getFont(std::wstring fontFamily)
	{
		return CreateFontW(0, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
			DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
			CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, fontFamily.c_str());
	}
}