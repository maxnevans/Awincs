#pragma once

#include "base/Exception.h"

namespace Awincs
{
	class WindowException
		:
		public Exception
	{
	public:
		using Exception::Exception;
	};
}