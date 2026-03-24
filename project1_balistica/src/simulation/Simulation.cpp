#include "Simulation.h"

#include "raylib.h"

namespace physim
{

    Simulation::Simulation(Environment &environment, Projectile &projectile)
        : projectile{projectile}, environment{environment}, time{0.0f} {}

    void Simulation::start()
    {
        running = true;
        if (!projectile.isLaunched())
        {
            projectile.launch();
        }
    }

    void Simulation::stop()
    {
        running = false;
    }

    void Simulation::update(float timeStep)
    {
        if (running)
        {
            time += timeStep;
            while (time >= timeStepPhysics)
            {
                projectile.update(timeStepPhysics, environment);
                time -= timeStepPhysics;
            }
        }
    }

    void Simulation::reset()
    {
        projectile.reset();
        time = 0.0f;
        running = false;
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
