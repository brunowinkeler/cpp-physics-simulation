#include "Simulation.h"

#include "raylib.h"

#include <algorithm>

namespace physim
{

    Simulation::Simulation(Environment &environment, Projectile &projectile)
        : projectile{projectile}, environment{environment} {}

    void Simulation::start()
    {
        if (projectile.isLanded())
        {
            return;
        }

        running = true;
        if (!projectile.isLaunched())
        {
            projectile.launch();
            trajectoryRecorder.record(projectile.getPosition().x, projectile.getPosition().y,
                                      timeGlobal,
                                      projectile.getCurrentSpeed());
        }
    }

    void Simulation::stop()
    {
        running = false;
    }

    void Simulation::update(float timeStep)
    {
        if (!running || timeStep <= 0.0f)
        {
            return;
        }

        timePerFrame += timeStep;

        while (running && timePerFrame >= timeStepPhysics)
        {
            projectile.update(timeStepPhysics, environment);
            timeGlobal += projectile.getLastUpdateDuration();
            timePerFrame -= timeStepPhysics;

            trajectoryRecorder.record(projectile.getPosition().x, projectile.getPosition().y,
                                      timeGlobal,
                                      projectile.getCurrentSpeed());

            if (projectile.isLanded())
            {
                running = false;
                timePerFrame = 0.0f;
            }
        }
    }

    void Simulation::reset()
    {
        projectile.reset();
        trajectoryRecorder.clear();
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
