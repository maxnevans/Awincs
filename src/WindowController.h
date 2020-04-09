#pragma once

#include "pch.h"

#include "WinAPIWindow.h"
#include "components/Component.h"
#include "components/ComponentEvent.h"

namespace Awincs
{
	class WinAPIWindow;

	enum class WindowBorder
	{
		LEFT,
		RIGHT,
		TOP,
		BOTTOM
	};

	class WindowController
		:
		public Component
	{
	public:
		using CaptureCallback = std::function<bool(Point)>;

	public:
		WindowController();
		WindowController(const Point& anchorPoint, const Dimensions& dims);
		WindowController(const WindowController&) = delete;
		WindowController& operator=(const WindowController&) = delete;

		void setTitle(const std::wstring& title);
		const std::wstring& getTitle() const;
		void setDimensions(const Dimensions& dims);
		const Dimensions& getDimensions() const;
		void setAnchorPoint(const Point&);
		const Point& getAnchorPoint() const;
		void show();
		void hide();
		void redraw();
		void setMoveCapture(CaptureCallback);
		void setSizeCaptures(CaptureCallback left, CaptureCallback right, CaptureCallback top, CaptureCallback bottom);
		bool testMoveCapture(const Point&) const;
		std::set<WindowBorder> testSizeCapture(const Point&) const;
		virtual bool checkAffiliationIgnoreChildren(const Point& p) const override;
		Point transformToLocalPoint(const Point& p) const;
		virtual void closeWindow() override;
		virtual void minimizeWindow() override;
		virtual void maximizeWindow() override;

	protected:
		virtual void draw(HDC) const override;

	private:
		static constexpr COLORREF DEFAULT_BACKGROUND_COLOR = RGB(10, 20, 30);

		COLORREF backgroundColor = DEFAULT_BACKGROUND_COLOR;
		std::unique_ptr<WinAPIWindow> window;
		std::map<WindowBorder, CaptureCallback> sizeCaptures;
		CaptureCallback moveCapture;
	};
}