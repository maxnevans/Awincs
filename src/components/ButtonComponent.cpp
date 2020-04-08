#include "ButtonComponent.h"
#include "../../../DebugConsole/include/DebugConsole.h"
#include "../Geometry.h"

void Awincs::ButtonComponent::draw(HDC hdc) const
{
	auto brush = CreateSolidBrush(backgroundColor);
	auto prevBrush = SelectObject(hdc, brush);

	auto pen = CreatePen(PS_NULL, 0, RGB(0,0,0));
	auto prevPen = SelectObject(hdc, pen);

	auto [width, height] = getDimensions();
	auto [x, y] = getGlobalAnchorPoint();

	Rectangle(hdc, x, y, x + width, y + height);

	HFONT font = getFont(DEFAULT_TITLE_FONT_FAMILY);
	auto prevFont = SelectObject(hdc, font);

	SIZE size = {};
	GetTextExtentPoint32(hdc, title.c_str(), static_cast<int>(title.size()), &size);

	int actualX = x + width / 2 - size.cx /2;
	int actualY = y + height / 2 - size.cy / 2;

	TextOut(hdc, actualX, actualY, title.c_str(), static_cast<int>(title.size()));

	SelectObject(hdc, prevFont);
	DeleteObject(font);

	SelectObject(hdc, prevPen);
	DeleteObject(pen);

	SelectObject(hdc, prevBrush);
	DeleteObject(brush);

	Component::draw(hdc);
}

void Awincs::ButtonComponent::setTitle(const std::wstring& title)
{
	this->title = title;
}

std::wstring Awincs::ButtonComponent::getTitle() const
{
	return title;
}

bool Awincs::ButtonComponent::checkAffiliation(const Point& pt) const
{
	return Geometry::IntRectangle::checkAffiliation(getAnchorPoint(), getDimensions(), pt);
}

void Awincs::ButtonComponent::handleEvent(const MouseEvent& e)
{
	Component::handleEvent(e);

	auto mouseEvent = static_cast<const ComponentEvent::MouseEvent&>(e);

	DCONSOLE(L"Button component event handling: coordinates(" << mouseEvent.point.x << L"," << mouseEvent.point.y << L")\n");
}

HFONT Awincs::ButtonComponent::getFont(std::wstring fontFamily)
{
	return CreateFontW(
		0, 0, 0, 0, 
		FW_NORMAL, FALSE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, 
		CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, fontFamily.c_str()
	);
}
