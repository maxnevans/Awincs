#include "WindowPaint.h"

LRESULT AWC::WindowPaint::operator()(std::shared_ptr<WindowComponent> window, LRESULT prevHandlerReturnValue, WPARAM wParam, LPARAM lParam, UINT messageType)
{
	

	PAINTSTRUCT ps = {};
	HDC hdc = BeginPaint(window->getHWND(), &ps);

	auto [width, height] = window->getDimensions();
	HDC memHdc = CreateCompatibleDC(hdc);
	HBITMAP hBitmap = CreateCompatibleBitmap(hdc, width, height);
	SelectObject(memHdc, hBitmap);

	window->foreachDrawable([&memHdc](Drawable& drawable){
		drawable.draw(memHdc);
	});

	BitBlt(hdc, 0, 0, width, height, memHdc, 0, 0, SRCCOPY);
	EndPaint(window->getHWND(), &ps);

	DeleteObject(memHdc);
	DeleteObject(hBitmap);

	return 0;
}

AWC::WindowPaint::WindowMessageTypes AWC::WindowPaint::getMessageTypes() const
{
	return WindowMessageTypes{ MESSAGE_TYPE };
}
