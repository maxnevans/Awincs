#pragma once
#include "WindowException.h"

namespace AWC
{

	class WindowControllerException
		:
		public Exception
	{
	public:
		using Exception::Exception;
	};
}