#include "pch.h"
#include "WindowController.h"

#include "components/ComponentEvent.h"
#include "WindowControllerException.h"
#include "../../DebugConsole/include/DebugConsole.h"

namespace Awincs
{
	WindowController::WindowController()
		:
		Component({1280, 720}),
		window(std::make_unique<WinAPIWindow>(*this))
	{
	}

	void WindowController::setTitle(std::wstring title)
	{
		window->setTitle(title);
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
		DCONSOLE(L"WindowController::redraw()\n");
		window->draw([&](HDC hdc) {
			this->draw(hdc); 
		});
	}
	void WindowController::draw(HDC hdc) const
	{
		Component::draw(hdc);
	}
}

