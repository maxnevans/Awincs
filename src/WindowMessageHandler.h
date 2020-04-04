#pragma once
#include <windows.h>
#include <vector>
#include "WindowComponent.h"

namespace AWC
{
	class WindowComponent;

	class WindowMessageHandler
	{
	public:
		using WindowMessageTypes = std::vector<UINT> ;

	public:
		virtual LRESULT operator()(std::shared_ptr<WindowComponent> window, LRESULT prevHandlerReturnValue, WPARAM wParam, LPARAM lParam, UINT messageType) = 0;
		virtual WindowMessageTypes getMessageTypes() const = 0;
		virtual ~WindowMessageHandler() {}
	};
}