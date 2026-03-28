#include "Simulation.h"

#include "raylib.h"

#include <algorithm>

namespace physim
{

    Simulation::Simulation(Environment &environment, Projectile &projectile)
        : projectile{projectile}, environment{environment} {}

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
            timeGlobal += timeStep;
            timePerFrame += timeStep;

            trajectoryRecorder.record(projectile.getPosition().x, projectile.getPosition().y,
                                      timeGlobal,
                                      projectile.getCurrentSpeed());

            while (timePerFrame >= timeStepPhysics)
            {
                projectile.update(timeStepPhysics, environment);
                timePerFrame -= timeStepPhysics;
            }
        }
    }

    void Simulation::reset()
    {
        projectile.reset();
        timePerFrame = 0.0f;
        timeGlobal = 0.0f;
        running = false;
    }

    void Simulation::setPhysicsTimeStep(float timeStep)
    {
        timeStepPhysics = std::max(timeStep, 0.0001f);
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
