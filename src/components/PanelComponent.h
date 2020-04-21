#pragma once

#include "pch.h"

#include "../Component.h"

namespace Awincs
{
    class PanelComponent
        :
        public Component
    {
    public:
        PanelComponent();
        PanelComponent(const Point& a, const Dimensions& d);
        virtual void setBold(bool enabled = true);
        virtual void setItalic(bool enabled = true);
        virtual void showText();
        virtual void hideText();
        virtual void setText(std::wstring text);
        virtual void setBackgroundColor(gp::ARGB color, ComponentState state = ComponentState::DEFAULT);
        virtual void setTextColor(gp::ARGB color, ComponentState state = ComponentState::DEFAULT);
        virtual void setTextAnchorPoint(const Point& a);
        virtual void setTextAlignment(gp::StringAlignment horizontal, gp::StringAlignment vertical);
        virtual void setFontSize(int px);
        virtual void setFontFamily(std::wstring fontFamily);
        virtual void setBackgroundImage(std::wstring imagePath, ComponentState state = ComponentState::DEFAULT);
        virtual void setBackgroundImage(int resourceId, ComponentState state = ComponentState::DEFAULT);
        virtual void setOpacity(float opacity);
        virtual ShouldParentHandleEvent handleEvent(const Event::Mouse::ButtonEvent& e) override;

    protected:
        virtual void draw(gp::Graphics&) const override;
        virtual gp::ARGB p_calculateColor(gp::ARGB color) const;
        virtual void p_setBold(bool enabled = true);
        virtual void p_setItalic(bool enabled = true);
        virtual void p_showText();
        virtual void p_hideText();
        virtual void p_setText(std::wstring text);
        virtual void p_setBackgroundColor(gp::ARGB color, ComponentState state = ComponentState::DEFAULT);
        virtual void p_setTextColor(gp::ARGB color, ComponentState state = ComponentState::DEFAULT);
        virtual void p_setTextAnchorPoint(const Point& a);
        virtual Point p_getTextAnchorPoint();
        virtual void p_setTextAlignment(gp::StringAlignment horizontal, gp::StringAlignment vertical);
        virtual std::pair<gp::StringAlignment, gp::StringAlignment> p_getTextAlignment();
        virtual void p_setFontSize(int px);
        virtual void p_setFontFamily(std::wstring fontFamily);
        virtual void p_setBackgroundImage(std::wstring imagePath, ComponentState state = ComponentState::DEFAULT);
        virtual void p_setBackgroundImage(int resourceId, ComponentState state = ComponentState::DEFAULT);
        virtual void p_setOpacity(float opacity);

    private:
        static constexpr gp::ARGB DEFAULT_BACKGROUND_COLOR                                      = makeARGB(0x10, 0x20, 0x30);
        static constexpr gp::ARGB DEFAULT_HOVER_BACKGROUND_COLOR                                = makeARGB(0x10, 0x20, 0x30);
        static constexpr gp::ARGB DEFAULT_ACTIVE_BACKGROUND_COLOR                               = makeARGB(0x10, 0x20, 0x30);
        static constexpr gp::ARGB DEFAULT_TEXT_COLOR                                            = makeARGB(0x99, 0x99, 0x99);
        static constexpr gp::ARGB DEFAULT_HOVER_TEXT_COLOR                                      = makeARGB(0xff, 0xff, 0xff);
        static constexpr gp::ARGB DEFAULT_ACTIVE_TEXT_COLOR                                     = makeARGB(0xff, 0xff, 0xff);
        static constexpr gp::StringAlignment DEFAULT_VERTICAL_ALIGNMENT                         = gp::StringAlignment::StringAlignmentNear;
        static constexpr gp::StringAlignment DEFAULT_HORIZONTAL_ALIGNMENT                       = gp::StringAlignment::StringAlignmentNear;
        static constexpr Geometry::Point2D<gp::REAL> DEFAULT_TEXT_ANCHOR_POINT                  = { 0, 0 };
        static constexpr float DEFAULT_OPACITY                                                  = 1;
        static constexpr int DEFAULT_FONT_SIZE                                                  = 14;
        static constexpr gp::Unit DEFAULT_FONT_UNIT                                             = gp::Unit::UnitPixel;
        static constexpr gp::FontStyle DEFAULT_FONT_STYLE                                       = gp::FontStyle::FontStyleRegular;
        static constexpr const wchar_t* DEFAULT_FONT_FAMILY                                     = L"Tahoma";

        bool isItalic                                                                           = false;
        bool isBold                                                                             = false;
        gp::StringFormat textFormat;
        std::map<ComponentState, std::shared_ptr<gp::Image>> backgroundImages                   = {};
        std::wstring fontFamily                                                                 = DEFAULT_FONT_FAMILY;
        gp::REAL fontSize                                                                       = static_cast<gp::REAL>(DEFAULT_FONT_SIZE);
        gp::PointF textAnchorPoint                                                              = gp::PointF{ DEFAULT_TEXT_ANCHOR_POINT.x, DEFAULT_TEXT_ANCHOR_POINT.y };
        float opacity                                                                           = DEFAULT_OPACITY;
        bool isTextShown                                                                        = false;
        std::wstring text                                                                       = L"";
        gp::FontStyle fontStyle                                                                 = DEFAULT_FONT_STYLE;
        std::map<ComponentState, gp::ARGB> backgroundColors                                     = { {ComponentState::DEFAULT, DEFAULT_BACKGROUND_COLOR},
                                                                                                    {ComponentState::HOVER,   DEFAULT_HOVER_BACKGROUND_COLOR},
                                                                                                    {ComponentState::ACTIVE,  DEFAULT_ACTIVE_BACKGROUND_COLOR} };
        std::map<ComponentState, gp::ARGB> textColors                                           = { {ComponentState::DEFAULT, DEFAULT_TEXT_COLOR},
                                                                                                    {ComponentState::HOVER,   DEFAULT_HOVER_TEXT_COLOR},
                                                                                                    {ComponentState::ACTIVE,  DEFAULT_ACTIVE_TEXT_COLOR} };
    };
}