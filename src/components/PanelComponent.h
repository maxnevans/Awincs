#pragma once

#include "../pch.h"

#include "Component.h"

namespace Awincs
{
	class PanelComponent
		:
		public Component
	{
	public:
		virtual bool checkAffiliation(const Point& pt) const override;
		virtual void draw(HDC hdc) const override;

	private:
		static constexpr COLORREF DEFAULT_BACKGROUND_COLOR = RGB(10, 20, 30);

		COLORREF backgroundColor = DEFAULT_BACKGROUND_COLOR;
	};
}