#pragma once

#include "pch.h"

#include "../Component.h"

namespace Awincs
{
	enum class ComponentState
	{
		DEFAULT,
		HOVER,
		ACTIVE
	};

	class ButtonComponent
		:
		public Component
	{
	public:
		void setBackgroundColor(gp::ARGB color, ComponentState state = ComponentState::DEFAULT);
		void setTitleColor(gp::ARGB color, ComponentState state = ComponentState::DEFAULT);
		void setTitle(const std::wstring& title);
		std::wstring getTitle() const;
		void onClick(std::function<void()>);
		virtual ShouldParentHandleEvent checkAffiliationIgnoreChildren(const Point& pt) const override;
		virtual ShouldParentHandleEvent handleEvent(const Event::Mouse::ButtonEvent& e) override;
		virtual ShouldParentHandleEvent handleEvent(const Event::Keyboard::KeyEvent& e) override;
		virtual ShouldParentHandleEvent handleEvent(const Event::Mouse::HoverStart& e) override;
		virtual ShouldParentHandleEvent handleEvent(const Event::Mouse::HoverEnd& e) override;
		virtual ShouldParentHandleEvent handleEvent(const Event::Mouse::Hover& e) override;

	protected:
		virtual void draw(gp::Graphics&) const override;
	

	private:
		static constexpr gp::ARGB DEFAULT_BACKGROUND_COLOR					= makeARGB(0xfF, 0xf, 0xf);
		static constexpr gp::ARGB DEFAULT_HOVER_BACKGROUND_COLOR			= makeARGB(0x2f, 0x2f, 0x2f);
		static constexpr gp::ARGB DEFAULT_ACTIVE_BACKGROUND_COLOR			= makeARGB(0x2f, 0x2f, 0x2f);
		static constexpr gp::ARGB DEFAULT_TITLE_COLOR						= makeARGB(0xff, 0xff, 0xff);
		static constexpr gp::ARGB DEFAULT_HOVER_TITLE_COLOR					= makeARGB(0xff, 0xff, 0xff);
		static constexpr gp::ARGB DEFAULT_ACTIVE_TITLE_COLOR				= makeARGB(0xff, 0xff, 0xff);
		static constexpr const wchar_t* DEFAULT_TITLE_FONT_FAMILY			= L"Tahoma";
		static constexpr gp::REAL DEFAULT_TITLE_SIZE						= 14;

		
		std::map<ComponentState, gp::ARGB> backgroundColors                 = { {ComponentState::DEFAULT, DEFAULT_BACKGROUND_COLOR},
																				{ComponentState::HOVER,   DEFAULT_HOVER_BACKGROUND_COLOR},
																				{ComponentState::ACTIVE,  DEFAULT_ACTIVE_BACKGROUND_COLOR} };
		std::map<ComponentState, gp::ARGB> titleColors						= { {ComponentState::DEFAULT, DEFAULT_TITLE_COLOR},
																				{ComponentState::HOVER,   DEFAULT_HOVER_TITLE_COLOR},
																				{ComponentState::ACTIVE,  DEFAULT_ACTIVE_TITLE_COLOR} };
		std::wstring title													= L"";
		ComponentState state												= ComponentState::DEFAULT;
	};
}