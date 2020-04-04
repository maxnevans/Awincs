#pragma once
#include "base/Exception.h"

namespace AWC
{
	class WindowException
		:
		public Exception
	{
	public:
		using Exception::Exception;
	};
}