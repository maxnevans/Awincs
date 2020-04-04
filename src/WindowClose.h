#pragma once
#include "WindowMessageHandler.h"

namespace AWC
{
	class WindowClose
		:
		public WindowMessageHandler
	{
	public:
		virtual LRESULT operator()(std::shared_ptr<WindowComponent> window, LRESULT prevHandlerReturnValue, WPARAM wParam, LPARAM lParam, UINT messageType = MESSAGE_TYPE) override;
		virtual WindowMessageTypes getMessageTypes() const override;

	private:
		static constexpr UINT MESSAGE_TYPE = WM_CLOSE;
	};
}