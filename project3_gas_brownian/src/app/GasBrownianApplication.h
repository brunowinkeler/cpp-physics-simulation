#ifndef GAS_BROWNIAN_APPLICATION_H
#define GAS_BROWNIAN_APPLICATION_H

#include "rendering/GasOverlayRenderer.h"
#include "rendering/GasSceneRenderer.h"
#include "simulation/GasSession.h"
#include "ui/GasUiMenus.h"

namespace physim
{
    class GasBrownianApplication
    {
    public:
        GasBrownianApplication();
        ~GasBrownianApplication();

        void run();

    private:
        void handleSimulationShortcuts();
        void updateSimulation(float frameTime);
        void draw();

        GasSession session;
        GasUiMenus uiMenus;
        GasSceneRenderer sceneRenderer;
        GasOverlayRenderer overlayRenderer;
    };
} // namespace physim

#endif // GAS_BROWNIAN_APPLICATION_H
