#pragma once
#include "WindowMessageHandler.h"

namespace AWC
{
	class WindowSizingMoving
		:
		public WindowMessageHandler
	{
	public:
		virtual LRESULT operator()(std::shared_ptr<WindowComponent> window, LRESULT prevRetValue,
			WPARAM wParam, LPARAM lParam, UINT messageType = MESSAGE_TYPES[0]) override;
		virtual WindowMessageTypes getMessageTypes() const override;

	private:
		static constexpr UINT MESSAGE_TYPES[] = { WM_SIZING, WM_SIZE, WM_MOVE, WM_MOVING };
	};
}