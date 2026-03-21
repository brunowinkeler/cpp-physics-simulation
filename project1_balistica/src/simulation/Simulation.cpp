#include "Simulation.h"

#include "raylib.h"

namespace physim
{
    Simulation::Simulation(Environment &environment, Projectile &projectile)
        : projectile{projectile}, environment{environment}, time{0.0f} {}

    void Simulation::run(float timeStep)
    {
        projectile.update(timeStep, environment);
        time += timeStep;
    }

    void Simulation::reset()
    {
        projectile.reset();
        time = 0.0f;
    }

    Projectile Simulation::getProjectile() const
    {
        return projectile;
    }

    Environment Simulation::getEnvironment() const
    {
        return environment;
    }
}
