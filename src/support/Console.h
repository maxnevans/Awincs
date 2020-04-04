#pragma once
#include <windows.h>
#include <memory>
#include <string>

namespace AWC
{
	class Console
	{
	private:
		Console();

	public:
		~Console();
		Console(const Console&) = delete;
		Console& operator=(const Console&) = delete;
		Console& operator<<(std::wstring message);
		static Console& getConsole();

	private:
		static std::unique_ptr<Console> instance;
	};

}
