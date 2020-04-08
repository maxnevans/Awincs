#include "WindowController.h"
#include "components/Component.h"
#include "components/ComponentEvent.h"

#include <algorithm>
#include "WindowControllerException.h"

namespace Awincs
{
	WindowController::WindowController()
		:
		window(new WinAPIWindow())
	{
		window->registerEventCallback([&](const ComponentEvent::Event& e) {
			switch (e.eventType)
			{
				case ComponentEvent::EventType::MOUSE:
				{
					auto mouseEvent = static_cast<const ComponentEvent::MouseEvent&>(e);
					for (const auto& child : components)
					{
						if (child->checkAffiliation(mouseEvent.point))
						{
							child->handleEvent(mouseEvent);
							break;
						}
					}
					break;
				}
				case ComponentEvent::EventType::KEY:
				{
					auto keyEvent = static_cast<const ComponentEvent::KeyEvent&>(e);
					for (const auto& child : components)
						child->handleEvent(keyEvent);
					break;
				}
			}
		});
	}
	void WindowController::setTitle(std::wstring title)
	{
		window->setTitle(title);
	}
	void WindowController::addComponent(std::shared_ptr<Component> component)
	{
		components.push_back(component);
	}
	void WindowController::removeComponent(std::shared_ptr<Component> component)
	{
		auto findable = std::find(components.begin(), components.end(), component);

		if (findable == components.end())
			throw WindowControllerException(L"Failed to remove component from window control: component does not belong to this window control!");

		components.erase(findable);
	}
	void WindowController::show()
	{
		window->show();
	}
	void WindowController::hide()
	{
		window->hide();
	}
	void WindowController::redraw()
	{
		window->draw([&](HDC hdc) {
			for (auto& component : components)
				component->draw(hdc);
		});
	}
}

