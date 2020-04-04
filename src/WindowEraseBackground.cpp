#include "WindowEraseBackground.h"

LRESULT AWC::WindowEraseBackground::operator()(std::shared_ptr<WindowComponent> window, LRESULT prevResult, 
	WPARAM wParam, LPARAM lParam, UINT messageType)
{
	return TRUE;
}

AWC::WindowEraseBackground::WindowMessageTypes AWC::WindowEraseBackground::getMessageTypes() const
{
	return WindowMessageTypes{ MESSAGE_TYPE };
}
