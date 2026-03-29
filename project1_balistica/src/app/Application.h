#ifndef APPLICATION_H
#define APPLICATION_H

#include "rendering/SimulationOverlayRenderer.h"
#include "rendering/SimulationSceneRenderer.h"
#include "simulation/SimulationSession.h"
#include "ui/UiMenus.h"

namespace physim
{
    class Application
    {
    public:
        Application();
        ~Application();

        void run();

    private:
        void handleSimulationShortcuts();
        void updateSimulation(float frameTime);
        void draw();
        void syncSceneHighlights();

        SimulationSession session;
        UiMenus uiMenus;
        SimulationSceneRenderer sceneRenderer;
        SimulationOverlayRenderer overlayRenderer;
    };
}

#endif // APPLICATION_H
