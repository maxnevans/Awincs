#pragma once

#include "pch.h"

#include <DebugConsole.h>

#include "WindowController.h"

namespace Awincs
{
    using AppRetType = std::vector<std::shared_ptr<WindowController>>;
    extern AppRetType App(std::vector<std::wstring> args);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    ENABLE_DEBUG_CONSOLE();

    /* Arguments parsing */
    std::vector<std::wstring> args;
    std::wstring_view rawArgs(pCmdLine);

    auto prevToken = rawArgs.begin();
    auto token = std::find(prevToken, rawArgs.end(), L' ');
    args.emplace_back(prevToken, token);

    while (token != rawArgs.end())
    {
        prevToken = token + 1;
        token = std::find(prevToken, rawArgs.end(), L' ');
        args.emplace_back(prevToken, token);
    }

    Gdiplus::GdiplusStartupInput input;
    ULONG_PTR gdiplusToken;
    Gdiplus::GdiplusStartup(&gdiplusToken, &input, nullptr);

    try
    {
        auto windows = Awincs::App(args);
        MSG msg;
        while (GetMessage(&msg, NULL, 0, 0))
        {
            DispatchMessage(&msg);
            TranslateMessage(&msg);
        }
    }
    catch (Awincs::Exception& e)
    {
        std::wstringstream message;
        message << L"Awincs exception happend: " << e.what().c_str() << L'\n';
        MessageBox(NULL, message.str().c_str(), L"Awincs Exception", MB_OK);
    }
    catch (...)
    {
        MessageBox(NULL, L"Unknown exception happend!", L"Unknown Exception", MB_OK);
    }

    Gdiplus::GdiplusShutdown(gdiplusToken);

    return 0;
}