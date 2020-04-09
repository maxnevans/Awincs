#pragma once

#include "pch.h"

#include "WinAPIWindow.h"
#include "components/Component.h"

namespace Awincs
{
	class WindowController
		:
		public Component
	{
	public:
		WindowController();
		WindowController(const WindowController&) = delete;
		WindowController& operator=(const WindowController&) = delete;

		void setTitle(std::wstring title);
		void show();
		void hide();
		void redraw();

	private:
		std::unique_ptr<WinAPIWindow> window;
	};
}