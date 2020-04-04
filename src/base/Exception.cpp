#include "Exception.h"

namespace AWC
{
	Exception::Exception(std::wstring message)
		:
		message(message)
	{
	}

	const std::wstring& Exception::what() const noexcept
	{
		return this->message;
	}
}
