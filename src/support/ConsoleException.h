#pragma once
#include "../base/Exception.h"

namespace AWC
{
	class ConsoleException
		:
		public Exception
	{
	public:
		using Exception::Exception;
	};
}