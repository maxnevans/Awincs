#pragma once
#include <string>

namespace AWC
{
	//template<typename T>
	class Exception
	{
	public:
		Exception() = default;
		Exception(std::wstring message) : message(message) {}
		//Exception(T number) : message(std::to_wstring(number)) {}
		virtual const std::wstring& what() const noexcept { return message; }

	private:
		std::wstring message;
	};
}
