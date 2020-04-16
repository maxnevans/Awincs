#pragma once

#include "pch.h"

#include "../Component.h"

namespace Awincs
{
	class InputComponent
		:
		public Component
	{
	public:
		virtual ShouldParentHandleEvent handleEvent(const Event::Keyboard::InputEvent&) override;

	protected:
		virtual void draw(gp::Graphics&) const override;

	private:
		static constexpr gp::ARGB DEFAULT_BACKGROUND_COLOR = makeARGB(25, 50, 100);
		static constexpr gp::ARGB DEFAULT_CURSOR_COLOR = makeARGB(255, 255, 255);
		static constexpr gp::ARGB DEFAULT_TEXT_COLOR = makeARGB(170, 170, 170);
		static constexpr const wchar_t* DEFAULT_TEXT_FONT_FAMILY = L"Tahoma";
		static constexpr gp::REAL DEFAULT_TEXT_SIZE = 17;

		std::wstring text = L"";
	};
}