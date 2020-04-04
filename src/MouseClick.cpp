#include "MouseClick.h"
#include "../../DebugConsole/include/DebugConsole.h"

LRESULT AWC::MouseClick::operator()(std::shared_ptr<WindowComponent> window, LRESULT prevHandlerReturnValue, WPARAM wParam, LPARAM lParam, UINT messageType)
{
	DebugConsole::Console::getConsole() << L"Clicked!\n";

	return 0;
}

AWC::MouseClick::WindowMessageTypes AWC::MouseClick::getMessageTypes() const
{
	return WindowMessageTypes{MESSAGE_TYPE};
}
