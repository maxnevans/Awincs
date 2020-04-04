#include "WindowClose.h"

LRESULT AWC::WindowClose::operator()(std::shared_ptr<WindowComponent> window, LRESULT prevHandlerReturnValue, WPARAM wParam, LPARAM lParam, UINT messageType)
{
	PostQuitMessage(0);
	return 0;
}

AWC::WindowClose::WindowMessageTypes AWC::WindowClose::getMessageTypes() const
{
	return WindowMessageTypes{MESSAGE_TYPE};
}
