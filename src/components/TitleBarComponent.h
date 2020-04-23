#pragma once

#include "PanelComponent.h"
#include "CloseButtonComponent.h"

namespace Awincs
{
    class TitleBarComponent
        :
        public PanelComponent
    {
    public:
        TitleBarComponent();
        TitleBarComponent(const Point& a, const Dimensions& d);
        std::shared_ptr<CloseButtonComponent> getCloseButton();

    protected:
        void p_setMoveCapture(const std::shared_ptr<WindowController>&);
        void p_unsetMoveCapture(const std::shared_ptr<WindowController>&);

    private:
        std::shared_ptr<CloseButtonComponent> p_getCloseButton();
        void p_setupCallbacks();
        void p_setupOnDimensionsChange();
        void p_setupOnWindowControllerChange();

    private:
        std::shared_ptr<WindowController> windowController = nullptr;
        std::shared_ptr<CloseButtonComponent> closeButton = nullptr;
    };
}