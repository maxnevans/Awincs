#pragma once

#include "../pch.h"

#include "Component.h"

namespace Awincs
{
	class ButtonComponent
		:
		public Component
	{
	public:
		void setTitle(const std::wstring& title);
		std::wstring getTitle() const;
		virtual ShouldParentHandleEvent checkAffiliationIgnoreChildren(const Point& pt) const override;
		virtual ShouldParentHandleEvent handleEvent(const ComponentEvent::MouseButtonEvent& e) override;
		virtual ShouldParentHandleEvent handleEvent(const ComponentEvent::KeyEvent& e) override;

	protected:
		virtual void draw(HDC hdc) const override;
		
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