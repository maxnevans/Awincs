#include "pch.h"
#include "TitleBarComponent.h"

#include "CloseButtonComponent.h"
#include "WindowController.h"

namespace Awincs
{
    TitleBarComponent::TitleBarComponent()
        :
        TitleBarComponent({}, {})
    {
    }
    TitleBarComponent::TitleBarComponent(const Point& a, const Dimensions& d)
        :
        PanelComponent(a, d)
    {
        /* shared_from_this() in constructor workaround */
        auto wptr = std::shared_ptr<TitleBarComponent>(this, [](TitleBarComponent*) {});

        p_setupCallbacks();

        p_setTextAlignment(gp::StringAlignment::StringAlignmentCenter, gp::StringAlignment::StringAlignmentCenter);
        p_setTextAnchorPoint({ d.width / 2, d.height / 2 });

        this->closeButton = std::make_shared<CloseButtonComponent>();
        this->closeButton->setParent(shared_from_this());
        this->closeButton->setDimensions({ d.height, d.height });
        this->closeButton->setAnchorPoint({ d.width - d.height, 0 });
    }

    void TitleBarComponent::p_setMoveCapture(const std::shared_ptr<WindowController>& wc)
    {
        auto moveCapture = [this](Point p) {
            return checkAffiliationDontIgnoreChildren(p);
        };
        if (wc)
            wc->setMoveCapture(moveCapture);
    }
    void TitleBarComponent::p_unsetMoveCapture(const std::shared_ptr<WindowController>& wc)
    {
        if (wc)
            wc->setMoveCapture(nullptr);
    }
    void TitleBarComponent::p_setupCallbacks()
    {
        p_setupOnDimensionsChange();
        p_setupOnWindowControllerChange();
    }
    void TitleBarComponent::p_setupOnDimensionsChange()
    {
        auto onDimensionsChange = [this](const Dimensions& current, const Dimensions& next)
        {
            p_setTextAlignment(gp::StringAlignment::StringAlignmentCenter, gp::StringAlignment::StringAlignmentCenter);
            p_setTextAnchorPoint({ next.width / 2, next.height / 2 });
            closeButton->setDimensions({ next.height, next.height });
            closeButton->setAnchorPoint({ next.width - next.height, 0 });
        };
        p_onDimensionsChange(onDimensionsChange);
    }
    void TitleBarComponent::p_setupOnWindowControllerChange()
    {
        auto wcChangeCallback = [this](std::shared_ptr<WindowController> wc)
        {
            if (wc)
                p_setMoveCapture(wc);
            else
                p_unsetMoveCapture(wc);
        };
        p_onWindowControllerChange(wcChangeCallback);
    }
}

