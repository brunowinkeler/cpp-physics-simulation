#ifndef UIMANAGER_H
#define UIMANAGER_H

#include "Environment.h"
#include "Simulation.h"
#include "Projectile.h"
#include "UiMenus.h"

namespace physim
{
    class UiManager
    {
    public:
        UiManager();
        ~UiManager();

        void run();
        void reset();

    private:
        void updateSimulation();
        void updateEvents();
        void draw();

        Environment environment;
        Projectile projectile;

        Simulation simulation;
        UiMenus uiMenus;
    };
}

#endif // UIMANAGER_H
