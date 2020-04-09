#pragma once

#include "../pch.h"

#include "Component.h"

namespace Awincs
{
	class InputComponent
		:
		public Component
	{
	public:
		virtual ShouldParentHandleEvent handleEvent(const InputEvent&) override;

	protected:
		virtual void draw(HDC) const override;

	private:
		static HFONT getFont(std::wstring fontFamily);

	private:
		static constexpr COLORREF backgroundColor = RGB(25, 50, 100);
		static constexpr COLORREF cursorColor = RGB(255, 255, 255);
		static constexpr COLORREF textColor = RGB(170, 170, 170);
		static constexpr const wchar_t* DEFAULT_TEXT_FONT_FAMILY = L"Tahoma";

		std::wstring value = L"";
	};
}