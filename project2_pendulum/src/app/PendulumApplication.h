#ifndef PENDULUM_APPLICATION_H
#define PENDULUM_APPLICATION_H

#include "rendering/PendulumOverlayRenderer.h"
#include "rendering/PendulumSceneRenderer.h"
#include "simulation/PendulumSession.h"
#include "ui/PendulumUiMenus.h"

namespace physim
{
    class PendulumApplication
    {
    public:
        PendulumApplication();
        ~PendulumApplication();

        void run();

    private:
        void handleSimulationShortcuts();
        void updateSimulation(float frameTime);
        void draw();

        PendulumSession session;
        PendulumUiMenus uiMenus;
        PendulumSceneRenderer sceneRenderer;
        PendulumOverlayRenderer overlayRenderer;
    };
} // namespace physim

#endif // PENDULUM_APPLICATION_H
