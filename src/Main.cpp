#include <windows.h>
#include "support/Console.h"
#include "base/Exception.h"
#include "WindowComponent.h"
#include "ButtonComponent.h"
#include "PanelComponent.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	using namespace AWC;

	try
	{
		Console::getConsole() << L"Hello world application!\n";
		WindowComponent::registerClass();

		auto wnd = std::shared_ptr<WindowComponent>(new WindowComponent());
		wnd->setTitle(L"Hello world!");

		auto panel = std::shared_ptr<PanelComponent>(new PanelComponent());
		panel->setAnchorPoint({400, 300});
		panel->setDimensions({100, 200});
		panel->setParent(wnd);

		auto button = std::shared_ptr<ButtonComponent>(new ButtonComponent());
		button->setAnchorPoint({10, 10});
		button->setDimensions({100, 40});
		button->setTitle(L"Button");
		button->setParent(panel);

		wnd->show();

		MSG msg;

		while (GetMessage(&msg, NULL, 0, 0))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	catch (Exception& ex)
	{
		MessageBoxW(NULL, ex.what().c_str(), L"Exception", MB_OK | MB_ICONERROR);
	}

	Console::getConsole() << L"Application closed.\n";
	system("pause");

	return 0;
}