#include "WindowSizingMoving.h"

LRESULT AWC::WindowSizingMoving::operator()(std::shared_ptr<WindowComponent> window, LRESULT prevRetValue, WPARAM wParam, LPARAM lParam, UINT messageType)
{
	switch (messageType)
	{
		case WM_SIZE:
		{
			window->setDimensions({ LOWORD(lParam), HIWORD(lParam) });
			return 0;
		}
		case WM_MOVING:
		case WM_SIZING:
		{
			PRECT pRect = reinterpret_cast<PRECT>(lParam);

			window->setAnchorPoint({ pRect->left, pRect->top });
			window->setDimensions({ pRect->right - pRect->left, pRect->bottom - pRect->top });

			return TRUE;
		}
		case WM_MOVE:
		{
			window->setAnchorPoint({ LOWORD(lParam), HIWORD(lParam) });
			return 0;
		}
	}

	return 0;
}

AWC::WindowSizingMoving::WindowMessageTypes AWC::WindowSizingMoving::getMessageTypes() const
{
	int n = sizeof(MESSAGE_TYPES) / sizeof(MESSAGE_TYPES[0]);

	return WindowMessageTypes(MESSAGE_TYPES, MESSAGE_TYPES + n);
}
