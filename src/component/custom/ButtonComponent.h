#pragma once

#include "../../pch.h"

#include "../Component.h"

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
		virtual ShouldParentHandleEvent handleEvent(const Event::Mouse::ButtonEvent& e) override;
		virtual ShouldParentHandleEvent handleEvent(const Event::Keyboard::KeyEvent& e) override;
		virtual ShouldParentHandleEvent handleEvent(const Event::Mouse::HoverStart& e) override;
		virtual ShouldParentHandleEvent handleEvent(const Event::Mouse::HoverEnd& e) override;
		virtual ShouldParentHandleEvent handleEvent(const Event::Mouse::Hover& e) override;

	protected:
		virtual void draw(gp::Graphics&) const override;
	

	private:
		static constexpr gp::ARGB DEFAULT_BACKGROUND_COLOR					= makeARGB(0xfF, 0xf, 0xf);
		static constexpr gp::ARGB DEFAULT_HOVER_BACKGROUND_COLOR			= makeARGB(0x2f, 0x2f, 0x2f);
		static constexpr gp::ARGB DEFAULT_TITLE_COLOR						= makeARGB(0xff, 0xff, 0xff);
		static constexpr gp::ARGB DEFAULT_HOVER_TITLE_COLOR					= makeARGB(0xff, 0xff, 0xff);
		static constexpr const wchar_t* DEFAULT_TITLE_FONT_FAMILY			= L"Tahoma";
		static constexpr gp::REAL DEFAULT_TITLE_SIZE						= 17;

		std::wstring title = L"";
		gp::ARGB backgroundColor = DEFAULT_BACKGROUND_COLOR;
		gp::ARGB hoverBackgroundColor = DEFAULT_HOVER_BACKGROUND_COLOR;
		gp::ARGB titleColor = DEFAULT_TITLE_COLOR;
		gp::ARGB hoverTitleColor = DEFAULT_HOVER_TITLE_COLOR;
	};
}