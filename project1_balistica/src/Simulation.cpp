#include "Simulation.h"

namespace physim
{
    Simulation::Simulation(Projectile &projectile, Environment &environment)
        : projectile{projectile}, environment{environment}, time{0.0f} {}

    void Simulation::run(float timeStep)
    {
        while (projectile.getPosition().y >= 0)
        {
            projectile.update(timeStep, environment);
            time += timeStep;
        }
    }

    void Simulation::reset()
    {
        projectile.reset();
        time = 0.0;
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
