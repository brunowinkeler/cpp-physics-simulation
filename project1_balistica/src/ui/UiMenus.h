#ifndef UIMENUS_H
#define UIMENUS_H

#include "physics/Environment.h"
#include "physics/Projectile.h"
#include "simulation/Simulation.h"

namespace physim
{
    class UiMenus
    {
    public:
        UiMenus() = delete;
        UiMenus(Environment &env, Projectile &proj, Simulation &sim);
        void parametersSelectionScreen();

    private:
        Environment &environment;
        Projectile &projectile;
        Simulation &simulation;

        bool showDemoWindow{false}; // For testing and demonstration purposes
    };
} // namespace physim

#endif // UIMENUS_H
