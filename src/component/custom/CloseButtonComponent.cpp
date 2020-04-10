#include "../../pch.h"

#include "CloseButtonComponent.h"

#include "../../WindowStateHandler.h"

namespace Awincs
{
	CloseButtonComponent::ShouldParentHandleEvent CloseButtonComponent::handleEvent(const Event::Mouse::ButtonEvent& e)
	{
		expect(ButtonComponent::handleEvent(e));

		if (e.action == Event::Mouse::ButtonAction::DOWN && e.buttonType == Event::Mouse::ButtonType::LEFT)
		{
			auto parent = getParent().lock();
			expect(parent);
			parent->closeWindow();
		}

		return true;
	}
}
