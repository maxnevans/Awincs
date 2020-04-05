#pragma once
#include "Component.h"
#include <string>

namespace AWC
{
	class ButtonComponent
		:
		public Component
	{
	public:
		virtual void draw(HDC hdc) const override;
		void setTitle(std::wstring title);
		std::wstring getTitle() const;
		
	private:
		static HFONT getFont(std::wstring fontFamily);

	private:
		static constexpr COLORREF DEFAULT_BACKGROUND_COLOR					= RGB(0xfF, 0xf, 0xf);
		static constexpr COLORREF DEFAULT_HOVER_BACKGROUND_COLOR			= RGB(0x2f, 0x2f, 0x2f);
		static constexpr COLORREF DEFAULT_TITLE_COLOR						= RGB(0xff, 0xff, 0xff);
		static constexpr COLORREF DEFAULT_HOVER_TITLE_COLOR					= RGB(0xff, 0xff, 0xff);
		static constexpr const wchar_t* DEFAULT_TITLE_FONT_FAMILY			= L"Tahoma";

		std::wstring title = L"";
		COLORREF backgroundColor = DEFAULT_BACKGROUND_COLOR;
		COLORREF hoverBackgroundColor = DEFAULT_HOVER_BACKGROUND_COLOR;
		COLORREF titleColor = DEFAULT_TITLE_COLOR;
		COLORREF hoverTitleColor = DEFAULT_HOVER_TITLE_COLOR;
	};
}