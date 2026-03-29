#ifndef APPLICATION_H
#define APPLICATION_H

#include "physics/Environment.h"
#include "physics/Projectile.h"
#include "rendering/SimulationOverlayRenderer.h"
#include "rendering/SimulationSceneRenderer.h"
#include "ui/UiMenus.h"
#include "simulation/Simulation.h"

namespace physim
{
    class Application
    {
    public:
        Application();
        ~Application();

        void run();

    private:
        void updateSimulation();
        void draw();
        void syncSceneHighlights();

        Environment environment;
        Projectile projectile;

        Simulation simulation;
        UiMenus uiMenus;
        SimulationSceneRenderer sceneRenderer{simulation, projectile};
        SimulationOverlayRenderer overlayRenderer{simulation};
    };
}

#endif // APPLICATION_H
