#pragma once

#include "../../pch.h"

#include "../Component.h"

namespace Awincs
{
	class PanelComponent
		:
		public Component
	{
	protected:
		virtual void draw(gp::Graphics&) const override;

	private:
		static constexpr gp::ARGB DEFAULT_BACKGROUND_COLOR = makeARGB(0x10, 0x20, 0x30);

		COLORREF backgroundColor = DEFAULT_BACKGROUND_COLOR;
	};
}