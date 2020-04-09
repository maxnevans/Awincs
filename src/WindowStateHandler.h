#pragma once

namespace Awincs
{
	class WindowStateHandler
	{
	public:
		virtual void closeWindow() = 0;
		virtual void minimizeWindow() = 0;
		virtual void maximizeWindow() = 0;
	};
}