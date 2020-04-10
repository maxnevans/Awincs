#include "pch.h"
#include "WindowController.h"

#include "WindowControllerException.h"
#include "../../DebugConsole/include/DebugConsole.h"

namespace Awincs
{
	WindowController::WindowController(const Point& anchorPoint, const Dimensions& dims)
		:
		Component({0, 0}, dims),
		window(std::make_unique<WinAPIWindow>(*this, anchorPoint, dims))
	{
		window->create();
	}

	void WindowController::setTitle(const std::wstring& title)
	{
		window->setTitle(title);
	}

	const std::wstring& WindowController::getTitle() const
	{
		return window->getTitle();
	}

	void WindowController::setDimensions(const Dimensions& dims)
	{
		window->setDimensions(dims);
	}

	void WindowController::setMaxDimensions(const Dimensions& dims)
	{
		window->setMaxDimensions(dims);
	}

	void WindowController::setMinDimensions(const Dimensions& dims)
	{
		window->setMinDimensions(dims);
	}

	const WindowController::Dimensions& WindowController::getDimensions() const
	{
		return window->getDimensions();
	}

	const WindowController::Dimensions& WindowController::getMaxDimensions() const
	{
		return window->getMaxDimensions();
	}

	const WindowController::Dimensions& WindowController::getMinDimensions() const
	{
		return window->getMinDimensions();
	}

	void WindowController::setAnchorPoint(const Point& p)
	{
		window->setAnchorPoint(p);
	}

	const WindowController::Point& WindowController::getAnchorPoint() const
	{
		return window->getAnchorPoint();
	}

	void WindowController::show()
	{
		window->show();
	}
	void WindowController::hide()
	{
		window->hide();
	}
	void WindowController::redraw()
	{
		DCONSOLE(L"WindowController::redraw()\n");
		p_redraw();
	}
	void WindowController::closeWindow()
	{
		window->close();
	}
	void WindowController::minimizeWindow()
	{
		window->minimize();
	}
	void WindowController::maximizeWindow()
	{
		window->maximize();
	}
	WindowController::ShouldParentHandleEvent WindowController::handleEvent(const ComponentEvent::Window::MoveEvent& e)
	{
		expect(Component::handleEvent(e));
		p_redraw();
		return true;
	}
	WindowController::ShouldParentHandleEvent WindowController::handleEvent(const ComponentEvent::Window::ResizeEvent& e)
	{
		expect(Component::handleEvent(e));
		p_redraw();
		return true;
	}
	void WindowController::setMoveCapture(CaptureCallback cb)
	{
		moveCapture = cb;
	}
	bool WindowController::testMoveCapture(const Point& p) const
	{
		if (!moveCapture)
			return false;

		return moveCapture(transformToLocalPoint(p));
	}
	bool WindowController::checkAffiliationIgnoreChildren(const Point& p) const
	{
		auto [width, height] = getDimensions();

		return (p.x >= 0) && (p.x < width) && (p.y >= 0) && (p.y < height);
	}

	WindowController::Point WindowController::transformToLocalPoint(const Point& p) const
	{
		auto [x, y] = getAnchorPoint();

		return {p.x - x, p.y - y};
	}

	void WindowController::draw(HDC hdc) const
	{
		auto brush = CreateSolidBrush(backgroundColor);
		auto prevBrush = SelectObject(hdc, brush);

		auto pen = CreatePen(PS_NULL, 0, RGB(0, 0, 0));
		auto prevPen = SelectObject(hdc, pen);

		auto [width, height] = window->getDimensions();

		Rectangle(hdc, 0, 0, width, height);

		SelectObject(hdc, prevPen);
		DeleteObject(pen);

		SelectObject(hdc, prevBrush);
		DeleteObject(brush);

		Component::draw(hdc);
	}
	void WindowController::p_redraw()
	{
		window->draw([&](HDC hdc) {
			this->draw(hdc);
		});
	}
}

