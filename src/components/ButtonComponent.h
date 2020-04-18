#pragma once

#include "pch.h"

#include "PanelComponent.h"

namespace Awincs
{
	class ButtonComponent
		:
		public PanelComponent
	{
	public:
		using OnClickCallback = std::function<void(const Point&)>;

	public:
		ButtonComponent();
		virtual void setDimensions(const Dimensions& d) override;
		virtual void onClick(OnClickCallback cb);
		virtual ShouldParentHandleEvent handleEvent(const Event::Mouse::ButtonEvent& e) override;
		virtual ShouldParentHandleEvent handleEvent(const Event::Keyboard::KeyEvent& e) override;
		virtual ShouldParentHandleEvent handleEvent(const Event::Mouse::HoverStart& e) override;
		virtual ShouldParentHandleEvent handleEvent(const Event::Mouse::HoverEnd& e) override;
		virtual ShouldParentHandleEvent handleEvent(const Event::Mouse::Hover& e) override;

	protected:
		static constexpr gp::StringAlignment DEFAULT_VERTICAL_ALIGNMENT = gp::StringAlignment::StringAlignmentCenter;
		static constexpr gp::StringAlignment DEFAULT_HORIZONTAL_ALIGNMENT = gp::StringAlignment::StringAlignmentCenter;

		OnClickCallback onClickCallback;
	};
}