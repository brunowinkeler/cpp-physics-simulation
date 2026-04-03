#ifndef PENDULUM_SIMULATION_H
#define PENDULUM_SIMULATION_H

#include "core/simulation/TrajectoryRecorder.h"

#include "physics/DoublePendulum.h"
#include "physics/PendulumEnvironment.h"
#include "physics/PendulumMode.h"
#include "physics/SimplePendulum.h"

namespace physim
{
    enum class PendulumSimulationState
    {
        Idle = 0,
        Running,
        Paused
    };

    class PendulumSimulation
    {
    public:
        PendulumSimulation(PendulumEnvironment &environment,
                           SimplePendulum &simplePendulum,
                           DoublePendulum &doublePendulum);

        void start();
        void stop();
        void reset();
        void update(float timeStep);
        void setMode(PendulumMode mode);
        void setPhysicsTimeStep(float timeStep);

        PendulumMode getMode() const { return mode; }
        bool isSimpleMode() const { return mode == PendulumMode::Simple; }
        bool isDoubleMode() const { return mode == PendulumMode::Double; }

        PendulumSimulationState getState() const { return state; }
        bool isIdle() const { return state == PendulumSimulationState::Idle; }
        bool isRunning() const { return state == PendulumSimulationState::Running; }
        bool isPaused() const { return state == PendulumSimulationState::Paused; }

        float getTimeGlobal() const { return timeGlobal; }
        float getPhysicsTimeStep() const { return physicsTimeStep; }
        float getCurrentTotalEnergy() const;
        float getInitialTotalEnergy() const { return initialTotalEnergy; }
        float getEnergyDelta() const { return getCurrentTotalEnergy() - initialTotalEnergy; }

        const std::vector<TrajectoryPoint> &getPrimaryTrailPoints() const { return primaryTrail.getPoints(); }
        const std::vector<TrajectoryPoint> &getSecondaryTrailPoints() const { return secondaryTrail.getPoints(); }

    private:
        void resetActiveSystem();
        void clearTrails();
        void recordCurrentState(bool forceSample);

        PendulumEnvironment &environment;
        SimplePendulum &simplePendulum;
        DoublePendulum &doublePendulum;
        TrajectoryRecorder primaryTrail;
        TrajectoryRecorder secondaryTrail;

        float accumulatedFrameTime{0.0f};
        float timeGlobal{0.0f};
        float physicsTimeStep{0.002f};
        float initialTotalEnergy{0.0f};
        PendulumMode mode{PendulumMode::Simple};
        PendulumSimulationState state{PendulumSimulationState::Idle};
    };
} // namespace physim

#endif // PENDULUM_SIMULATION_H
