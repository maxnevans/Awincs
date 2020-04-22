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

    protected:
        void p_setMoveCapture(const std::shared_ptr<WindowController>&);
        void p_unsetMoveCapture(const std::shared_ptr<WindowController>&);

    private:
        void p_setupCallbacks();
        void p_setupOnDimensionsChange();
        void p_setupOnWindowControllerChange();

    private:
        std::shared_ptr<WindowController> windowController = nullptr;
        std::shared_ptr<CloseButtonComponent> closeButton = nullptr;
    };
}