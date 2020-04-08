#pragma once
#include "WindowException.h"

namespace Awincs
{

	class WindowControllerException
		:
		public Exception
	{
	public:
		using Exception::Exception;
	};
}