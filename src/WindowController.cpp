#include "pch.h"
#include "WindowController.h"

#include <DebugConsole.h>

#include "event/dispatcher/MouseEventDispatcher.h"
#include "event/dispatcher/KeyboardEventDispatcher.h"
#include "event/dispatcher/DefaultEventDispatcher.h"

namespace Awincs
{
	WindowController::WindowController(const Point& anchorPoint, const Dimensions& dims)
		:
		Component({0, 0}, dims),
		eventManager(shared_from_this()),
		window(std::make_unique<WinAPIWindow>(*this, anchorPoint, dims))
	{
		eventManager.registerDispatcher<Event::MouseEventDispatcher<Component>>();
		eventManager.registerDispatcher<Event::KeyboardEventDispatcher<Component>>();
		eventManager.registerDispatcher<Event::DefaultEventDispatcher<Component>>();

		p_setupDrawCallback(*window);
		p_setRedrawCallback([this] { p_redraw(); });
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

	Event::EventManager<Component>& WindowController::getEventManager()
	{
		return eventManager;
	}

	WindowController::ShouldParentHandleEvent WindowController::handleEvent(const Event::Window::MoveEvent& e)
	{
		return p_handleWindowEvent(e);
	}

	WindowController::ShouldParentHandleEvent WindowController::handleEvent(const Event::Window::ResizeEvent& e)
	{
		return p_handleWindowEvent(e);
	}

	WindowController::ShouldParentHandleEvent WindowController::handleEvent(const Event::Window::MinimizeEvent& e)
	{
		return p_handleWindowEvent(e);
	}

	WindowController::ShouldParentHandleEvent WindowController::handleEvent(const Event::Window::MaximizeEvent& e)
	{
		return p_handleWindowEvent(e);
	}

	WindowController::ShouldParentHandleEvent WindowController::handleEvent(const Event::Window::RestoreEvent& e)
	{
		return p_handleWindowEvent(e);
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
		auto [width, height] = window->getDimensions();
		return (p.x >= 0) && (p.x < width) && (p.y >= 0) && (p.y < height);
	}

	WindowController::Point WindowController::transformToLocalPoint(const Point& p) const
	{
		return p - window->getAnchorPoint();
	}

	void WindowController::draw(Gdiplus::Graphics& gfx) const
	{
		auto [width, height] = window->getDimensions();
		gfx.FillRectangle(&gp::SolidBrush{ gp::Color{backgroundColor} }, gp::Rect{0, 0, width, height});
	}

	void WindowController::p_redraw()
	{
		Component::redraw();
		window->redraw();
	}

	void WindowController::p_setupDrawCallback(WinAPIWindow& wnd)
	{
		/* TODO: supply draw manager for drawing with DrawPolicies */
		wnd.setDrawCallback([this] (Gdiplus::Graphics& gfx)
		{
			this->p_draw(gfx);
		});
	}
}

