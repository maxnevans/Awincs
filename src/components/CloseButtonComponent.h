#pragma once

#include "../pch.h"

#include "ButtonComponent.h"

namespace Awincs
{
	class CloseButtonComponent
		:
		public ButtonComponent
	{
	public:
		virtual ShouldParentHandleEvent handleEvent(const MouseButtonEvent&) override;
	};
}