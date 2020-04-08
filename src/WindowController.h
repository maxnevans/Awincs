#pragma once

#include <memory>
#include <vector>

#include "WinAPIWindow.h"
#include "components/Component.h"

namespace Awincs
{
	class WindowController
	{
	public:
		WindowController();
		WindowController(const WindowController&) = delete;
		WindowController& operator=(const WindowController&) = delete;

		void setTitle(std::wstring title);
		void addComponent(std::shared_ptr<Component> component);
		void removeComponent(std::shared_ptr<Component> component);
		void show();
		void hide();
		void redraw();

	private:
		std::unique_ptr<WinAPIWindow> window;
		std::vector<std::shared_ptr<Component>> components;
	};
}