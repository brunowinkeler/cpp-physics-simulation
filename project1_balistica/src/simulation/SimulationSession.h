#ifndef SIMULATION_SESSION_H
#define SIMULATION_SESSION_H

#include "physics/Environment.h"
#include "physics/Projectile.h"
#include "Simulation.h"

namespace physim
{
    class SimulationSession
    {
    public:
        SimulationSession() : simulation{environment, projectile} {}

        Environment &getEnvironment() { return environment; }
        const Environment &getEnvironment() const { return environment; }

        Projectile &getProjectile() { return projectile; }
        const Projectile &getProjectile() const { return projectile; }

        Simulation &getSimulation() { return simulation; }
        const Simulation &getSimulation() const { return simulation; }

    private:
        Environment environment;
        Projectile projectile;
        Simulation simulation;
    };
} // namespace physim

#endif // SIMULATION_SESSION_H
