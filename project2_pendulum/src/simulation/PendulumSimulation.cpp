#include "PendulumSimulation.h"

#include <algorithm>

namespace physim
{
    namespace
    {
        constexpr float MIN_PHYSICS_TIME_STEP = 0.0001f;
    }

    PendulumSimulation::PendulumSimulation(PendulumEnvironment &environment,
                                           SimplePendulum &simplePendulum,
                                           DoublePendulum &doublePendulum)
        : environment{environment},
          simplePendulum{simplePendulum},
          doublePendulum{doublePendulum}
    {
        reset();
    }

    void PendulumSimulation::start()
    {
        if (state == PendulumSimulationState::Running)
        {
            return;
        }

        if (state == PendulumSimulationState::Idle)
        {
            resetActiveSystem();
            clearTrails();
            accumulatedFrameTime = 0.0f;
            timeGlobal = 0.0f;
            initialTotalEnergy = getCurrentTotalEnergy();
            recordCurrentState(true);
        }

        state = PendulumSimulationState::Running;
    }

    void PendulumSimulation::stop()
    {
        if (state == PendulumSimulationState::Running)
        {
            state = PendulumSimulationState::Paused;
        }
    }

    void PendulumSimulation::reset()
    {
        resetActiveSystem();
        clearTrails();
        accumulatedFrameTime = 0.0f;
        timeGlobal = 0.0f;
        initialTotalEnergy = getCurrentTotalEnergy();
        recordCurrentState(true);
        state = PendulumSimulationState::Idle;
    }

    void PendulumSimulation::update(float timeStep)
    {
        if (state != PendulumSimulationState::Running || timeStep <= 0.0f)
        {
            return;
        }

        accumulatedFrameTime += timeStep;
        while (accumulatedFrameTime >= physicsTimeStep)
        {
            if (mode == PendulumMode::Simple)
            {
                simplePendulum.update(physicsTimeStep, environment);
            }
            else
            {
                doublePendulum.update(physicsTimeStep, environment);
            }

            accumulatedFrameTime -= physicsTimeStep;
            timeGlobal += physicsTimeStep;
            recordCurrentState(false);
        }
    }

    void PendulumSimulation::setMode(PendulumMode mode)
    {
        if (state == PendulumSimulationState::Running || this->mode == mode)
        {
            return;
        }

        this->mode = mode;
        reset();
    }

    void PendulumSimulation::setPhysicsTimeStep(float timeStep)
    {
        physicsTimeStep = std::max(timeStep, MIN_PHYSICS_TIME_STEP);
    }

    float PendulumSimulation::getCurrentTotalEnergy() const
    {
        return mode == PendulumMode::Simple ? simplePendulum.getTotalEnergy(environment) : doublePendulum.getTotalEnergy(environment);
    }

    void PendulumSimulation::resetActiveSystem()
    {
        if (mode == PendulumMode::Simple)
        {
            simplePendulum.reset();
        }
        else
        {
            doublePendulum.reset();
        }
    }

    void PendulumSimulation::clearTrails()
    {
        primaryTrail.clear();
        secondaryTrail.clear();
    }

    void PendulumSimulation::recordCurrentState(bool forceSample)
    {
        if (mode == PendulumMode::Simple)
        {
            const PendulumBob bob = simplePendulum.getBob();
            primaryTrail.record(bob.x, bob.y, timeGlobal, bob.speed, forceSample);
            return;
        }

        const DoublePendulumBobs bobs = doublePendulum.getBobs();
        primaryTrail.record(bobs.first.x, bobs.first.y, timeGlobal, bobs.first.speed, forceSample);
        secondaryTrail.record(bobs.second.x, bobs.second.y, timeGlobal, bobs.second.speed, forceSample);
    }
} // namespace physim
