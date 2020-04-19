#include "pch.h"
#include "PanelComponent.h"

#include "Geometry.h"


namespace Awincs
{
	void PanelComponent::draw(gp::Graphics& gfx) const
	{
		auto [x, y] = p_transformToGlobal(Point{ 0, 0 });
		auto [width, height] = p_getDimensions();
		auto state = p_getState();

		if (auto findable = backgroundImages.find(state); findable != backgroundImages.end())
		{
			auto image = findable->second;
			gfx.DrawImage(image.get(), gp::Rect{ x, y, width, height});
		}
		else
		{
			gfx.FillRectangle(&gp::SolidBrush{ gp::Color{p_calculateColor(backgroundColors.at(state))} }, gp::Rect{ x, y, width, height });
		}

		if (isTextShown)
		{
			gfx.DrawString(text.c_str(), static_cast<INT>(text.size()),
				&gp::Font{ fontFamily.c_str(), fontSize, fontStyle, DEFAULT_FONT_UNIT },
				p_transformToGlobal(textAnchorPoint),
				&textFormat,
				&gp::SolidBrush(gp::Color{ p_calculateColor(textColors.at(state)) }));
		}
	}

	gp::ARGB PanelComponent::p_calculateColor(gp::ARGB color) const
	{
		if (opacity == 1)
			return color;

		return setARGBOpacity(static_cast<int>(getARGBOpacity(color) * opacity), color);
	}

	PanelComponent::PanelComponent()
		:
		PanelComponent({}, {})
	{
	}

	PanelComponent::PanelComponent(const Point& a, const Dimensions& d)
		:
		Component(a, d)
	{
		textFormat.SetAlignment(DEFAULT_HORIZONTAL_ALIGNMENT);
		textFormat.SetLineAlignment(DEFAULT_VERTICAL_ALIGNMENT);
	}

	void PanelComponent::setBold(bool enabled)
	{
		p_setBold(enabled);
	}

	void PanelComponent::setItalic(bool enabled)
	{
		p_setItalic(enabled);
	}

	void PanelComponent::showText()
	{
		p_showText();
	}

	void PanelComponent::hideText()
	{
		p_hideText();
	}

	void PanelComponent::setText(std::wstring text)
	{
		p_setText(text);
	}

	void PanelComponent::setBackgroundColor(gp::ARGB color, ComponentState state)
	{
		p_setBackgroundColor(color, state);
	}

	void PanelComponent::setTextColor(gp::ARGB color, ComponentState state)
	{
		p_setTextColor(color, state);
	}

	void PanelComponent::setTextAnchorPoint(const Point& a)
	{
		p_setTextAnchorPoint(a);
	}

	void PanelComponent::setTextAlignment(gp::StringAlignment horizontal, gp::StringAlignment vertical)
	{
		p_setTextAlignment(horizontal, vertical);
	}

	void PanelComponent::setFontSize(int px)
	{
		p_setFontSize(px);
	}

	void PanelComponent::setFontFamily(std::wstring fontFamily)
	{
		p_setFontFamily(fontFamily);
	}

	void PanelComponent::setBackgroundImage(std::wstring imagePath, ComponentState state)
	{
		p_setBackgroundImage(imagePath, state);
	}

	void PanelComponent::setBackgroundImage(int resourceId, ComponentState state)
	{
		p_setBackgroundImage(resourceId, state);
	}

	void PanelComponent::setOpacity(float opacity)
	{
		p_setOpacity(opacity);
	}

	PanelComponent::ShouldParentHandleEvent PanelComponent::handleEvent(const Event::Mouse::ButtonEvent& e)
	{
		bool shouldHandleEvent = Component::handleEvent(e);
		expect(shouldHandleEvent);

		if (e.action == Event::Mouse::ButtonAction::DOWN)
		{
			p_setFocusOnThisComponent();
		}

		return true;
	}

	void PanelComponent::p_setBold(bool enabled)
	{
		if (isItalic)
		{
			fontStyle = enabled ? gp::FontStyle::FontStyleBoldItalic : gp::FontStyle::FontStyleItalic;
		}
		else
		{
			fontStyle = enabled ? gp::FontStyle::FontStyleBold : gp::FontStyle::FontStyleRegular;
		}
	}

	void PanelComponent::p_setItalic(bool enabled)
	{
		if (isBold)
		{
			fontStyle = enabled ? gp::FontStyle::FontStyleBoldItalic : gp::FontStyle::FontStyleBold;
		}
		else
		{
			fontStyle = enabled ? gp::FontStyle::FontStyleItalic : gp::FontStyle::FontStyleRegular;
		}
	}

	void PanelComponent::p_showText()
	{
		isTextShown = true;
	}

	void PanelComponent::p_hideText()
	{
		isTextShown = false;
	}

	void PanelComponent::p_setText(std::wstring text)
	{
		this->text = text;
	}

	void PanelComponent::p_setBackgroundColor(gp::ARGB color, ComponentState state)
	{
		backgroundColors[state] = color;
	}

	void PanelComponent::p_setTextColor(gp::ARGB color, ComponentState state)
	{
		textColors[state] = color;
	}

	void PanelComponent::p_setTextAnchorPoint(const Point& a)
	{
		textAnchorPoint = gp::PointF(static_cast<gp::REAL>(a.x), static_cast<gp::REAL>(a.y));
	}

	PanelComponent::Point PanelComponent::p_getTextAnchorPoint()
	{
		return { static_cast<int>(textAnchorPoint.X), static_cast<int>(textAnchorPoint.Y) };
	}

	void PanelComponent::p_setTextAlignment(gp::StringAlignment horizontal, gp::StringAlignment vertical)
	{
		textFormat.SetAlignment(horizontal);
		textFormat.SetLineAlignment(vertical);
	}

	std::pair<gp::StringAlignment, gp::StringAlignment> PanelComponent::p_getTextAlignment()
	{
		return { textFormat.GetAlignment(), textFormat.GetLineAlignment() };
	}

	void PanelComponent::p_setFontSize(int px)
	{
		fontSize = static_cast<gp::REAL>(px);
	}

	void PanelComponent::p_setFontFamily(std::wstring fontFamily)
	{
		this->fontFamily = fontFamily;
	}

	void PanelComponent::p_setBackgroundImage(std::wstring imagePath, ComponentState state)
	{
		auto image = std::make_shared<gp::Image>(imagePath.c_str());
		backgroundImages[state] = image;
	}

	void PanelComponent::p_setBackgroundImage(int resourceId, ComponentState state)
	{
		auto bitmap = std::shared_ptr<gp::Bitmap>(gp::Bitmap::FromResource(GetModuleHandle(NULL), MAKEINTRESOURCE(resourceId)));
		backgroundImages[state] = bitmap;
	}

	void PanelComponent::p_setOpacity(float opacity)
	{
		this->opacity = opacity;
	}
}
