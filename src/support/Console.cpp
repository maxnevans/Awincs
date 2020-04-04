#include "Console.h"
#include "ConsoleException.h"
#include <cassert>

std::unique_ptr<AWC::Console> AWC::Console::instance;

AWC::Console::Console()
{
    if (AllocConsole() == FALSE)
    {
        assert(false && "Failed to create second console!");
    }
}

AWC::Console::~Console()
{
	FreeConsole();
}

AWC::Console& AWC::Console::operator<<(std::wstring message)
{
    if (WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), message.c_str(), message.size(), nullptr, nullptr) == FALSE)
    {
        throw ConsoleException(L"Failed to write to console!");
    }
    return *this;
}

AWC::Console& AWC::Console::getConsole()
{
    if (!instance)
    {
        instance = std::unique_ptr<Console>{new Console()};
    }

    return *instance;
}
