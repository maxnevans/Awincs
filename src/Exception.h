#pragma once
#include <string>

namespace Awincs
{
	class Exception
	{
	public:
		Exception() = default;
		Exception(std::wstring message) : message(message) {}
		virtual const std::wstring& what() const noexcept { return message; }

	private:
		std::wstring message;
	};
}
