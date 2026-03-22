#ifndef APPLICATION_H
#define APPLICATION_H

#include "physics/Environment.h"
#include "simulation/Simulation.h"
#include "physics/Projectile.h"
#include "ui/UiMenus.h"
#include "rendering/BackgroundRenderer.h"
#include "rendering/Camera.h"
#include "rendering/ProjectileRenderer.h"

namespace physim
{
    class Application
    {
    public:
        Application();
        ~Application();

        void run();
        void reset();

    private:
        void updateSimulation();
        void updateEvents();
        void draw();
        void drawWithCamera();

        Environment environment;
        Projectile projectile;

        Simulation simulation;
        UiMenus uiMenus;
        BackgroundRenderer backgroundRenderer;
        ProjectileRenderer projectileRenderer{projectile};
        Camera camera;
    };
}

#endif // APPLICATION_H
