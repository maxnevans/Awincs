#include "../pch.h"

#include "CloseButtonComponent.h"

#include "../WindowStateHandler.h"

namespace Awincs
{
	CloseButtonComponent::ShouldParentHandleEvent CloseButtonComponent::handleEvent(const MouseButtonEvent& e)
	{
		expect(ButtonComponent::handleEvent(e));

		if (e.action == ComponentEvent::MouseButtonAction::DOWN && e.buttonType == ComponentEvent::MouseButtonType::LEFT)
		{
			auto parent = getParent().lock();
			expect(parent);
			parent->closeWindow();
		}

		return true;
	}
}
