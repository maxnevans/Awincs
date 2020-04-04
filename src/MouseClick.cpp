#include "MouseClick.h"
#include "support/Console.h"

LRESULT AWC::MouseClick::operator()(std::shared_ptr<WindowComponent> window, LRESULT prevHandlerReturnValue, WPARAM wParam, LPARAM lParam, UINT messageType)
{
	Console::getConsole() << L"Clicked!\n";

	return 0;
}

AWC::MouseClick::WindowMessageTypes AWC::MouseClick::getMessageTypes() const
{
	return WindowMessageTypes{MESSAGE_TYPE};
}
