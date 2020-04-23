#pragma once

#include "pch.h"

#include "ButtonComponent.h"

namespace Awincs
{
    class CloseButtonComponent
        :
        public ButtonComponent
    {
    public:
        using FltPoint = Geometry::Point2D<float>;
    public:
        CloseButtonComponent();
        virtual void setCrossColor(gp::ARGB color, ComponentState state = ComponentState::DEFAULT);

        /* Event handlers */
        virtual ShouldParentHandleEvent handleEvent(const Event::Mouse::ButtonEvent&) override;

    protected:
        virtual void draw(gp::Graphics&) const override;
        void p_setCrossColor(gp::ARGB color, ComponentState state = ComponentState::DEFAULT);

        std::vector<FltPoint> p_transformVertices(std::vector<FltPoint>) const;

    private:
        static constexpr gp::ARGB DEFAULT_BACKGROUND_COLOR                          = makeARGB(0x00, 0x00, 0x00, 0x00);
        static constexpr gp::ARGB DEFAULT_HOVER_BACKGROUND_COLOR                    = makeARGB(0x00, 0x00, 0x00, 0x00);
        static constexpr gp::ARGB DEFAULT_ACTIVE_BACKGROUND_COLOR                   = makeARGB(0xff, 207, 81, 43);
        static constexpr gp::ARGB DEFAULT_CROSS_COLOR                               = makeARGB(0xff, 224, 224, 224);
        static constexpr gp::ARGB DEFAULT_HOVER_CROSS_COLOR                         = makeARGB(0xff, 207, 81, 43);
        static constexpr gp::ARGB DEFAULT_ACTIVE_CROSS_COLOR                        = makeARGB(0xff, 224, 224, 224);
        static constexpr gp::REAL DEFAULT_CROSS_WIDTH                               = 2.f;
        static constexpr FltPoint DEFAULT_CROSS_VERTICES[]                          = { {-1.0f,  1.0f}, { 1.0f, -1.0f},   // top left to bottom right
                                                                                        { 1.0f,  1.0f}, {-1.0f, -1.0f} }; // top right to bottom left
        static constexpr float DEFAULT_CROSS_SIZE                                   = .3f;

        std::map<ComponentState, gp::ARGB> crossColors                              = { {ComponentState::DEFAULT, DEFAULT_CROSS_COLOR},
                                                                                        {ComponentState::HOVER,   DEFAULT_HOVER_CROSS_COLOR},
                                                                                        {ComponentState::ACTIVE,  DEFAULT_ACTIVE_CROSS_COLOR} };
    };
}