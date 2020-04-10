#include "../../pch.h"
#include "InputComponent.h"

namespace Awincs
{
	InputComponent::ShouldParentHandleEvent InputComponent::handleEvent(const Event::Keyboard::InputEvent& e)
	{
		expect(Component::handleEvent(e));

		if (e.character == L'\b')
		{
			if (!value.empty())
				value.pop_back();
		}
		else
		{
			value += e.character;
		}
			
		Component::redraw();

		return true;
	}

	void InputComponent::draw(HDC hdc) const
	{
		auto brush = CreateSolidBrush(backgroundColor);
		auto prevBrush = SelectObject(hdc, brush);

		auto pen = CreatePen(PS_NULL, 0, RGB(0, 0, 0));
		auto prevPen = SelectObject(hdc, pen);

		auto [width, height] = getDimensions();
		auto [x, y] = getGlobalAnchorPoint();

		Rectangle(hdc, x, y, x + width, y + height);

		HFONT font = getFont(DEFAULT_TEXT_FONT_FAMILY);
		auto prevFont = SelectObject(hdc, font);

		SIZE size = {};
		GetTextExtentPoint32(hdc, value.c_str(), static_cast<int>(value.size()), &size);

		int actualX = x + width / 2 - size.cx / 2;
		int actualY = y + height / 2 - size.cy / 2;

		TextOut(hdc, actualX, actualY, value.c_str(), static_cast<int>(value.size()));

		SelectObject(hdc, prevFont);
		DeleteObject(font);

		SelectObject(hdc, prevPen);
		DeleteObject(pen);

		SelectObject(hdc, prevBrush);
		DeleteObject(brush);

		Component::draw(hdc);
	}
	HFONT InputComponent::getFont(std::wstring fontFamily)
	{
		return CreateFontW(0, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
			DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
			CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, fontFamily.c_str());
	}
}