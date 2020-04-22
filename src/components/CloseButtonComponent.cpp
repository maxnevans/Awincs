#include "pch.h"

#include "CloseButtonComponent.h"

#include "WindowStateHandler.h"
#include "WindowController.h"

namespace Awincs
{
    CloseButtonComponent::CloseButtonComponent()
    {
        p_setBackgroundColor(DEFAULT_BACKGROUND_COLOR, ComponentState::DEFAULT);
        p_setBackgroundColor(DEFAULT_HOVER_BACKGROUND_COLOR, ComponentState::HOVER);
        p_setBackgroundColor(DEFAULT_ACTIVE_BACKGROUND_COLOR, ComponentState::ACTIVE);
    }
    CloseButtonComponent::ShouldParentHandleEvent CloseButtonComponent::handleEvent(const Event::Mouse::ButtonEvent& e)
    {
        auto shouldHandle = ButtonComponent::handleEvent(e);
        expect(shouldHandle);

        if (e.action == Event::Mouse::ButtonAction::DOWN && e.buttonType == Event::Mouse::ButtonType::LEFT)
        {
            auto wc = p_getWindowController();
            expect(wc);
            wc->closeWindow();
        }

        return true;
    }
    void CloseButtonComponent::draw(gp::Graphics& gfx) const
    {
        ButtonComponent::draw(gfx);

        gp::Pen pen{ gp::Color{crossColors.at(p_getState())}, DEFAULT_CROSS_WIDTH };

        auto transformedCrossVertices = p_transformVertices(std::vector<FltPoint>(&DEFAULT_CROSS_VERTICES[0], &DEFAULT_CROSS_VERTICES[4]));
        auto replaceCallback = [this](const FltPoint& pt) -> gp::Point { 
            auto p = p_transformToGlobal(pt);
            return {static_cast<INT>(p.x), static_cast<INT>(p.y)};
        };
        auto prepared = std::vector<gp::Point>{};
        prepared.reserve(transformedCrossVertices.capacity());
        std::transform(transformedCrossVertices.begin(), transformedCrossVertices.end(), std::back_inserter(prepared), replaceCallback);

        gfx.DrawLine(&pen, prepared[0], prepared[1]);
        gfx.DrawLine(&pen, prepared[2], prepared[3]);
    }
    std::vector<CloseButtonComponent::FltPoint> CloseButtonComponent::p_transformVertices(std::vector<FltPoint> vtcs) const
    {
        auto [width, height] = p_getDimensions();
        const float wFactor = width / 2.f;
        const float hFactor = height / 2.f;
        const float sFactor = DEFAULT_CROSS_SIZE;

        for (auto& vtx : vtcs)
        {
            vtx.x = vtx.x * sFactor * wFactor + wFactor;
            vtx.y = vtx.y * sFactor * hFactor + hFactor;
        }

        return vtcs;
    }
}
