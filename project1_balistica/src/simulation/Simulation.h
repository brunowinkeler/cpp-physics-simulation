#ifndef SIMULATION_H
#define SIMULATION_H

#include "physics/Projectile.h"
#include "physics/Environment.h"
#include "TrajectoryRecorder.h"

namespace physim
{
    class Simulation
    {
    public:
        Simulation(Environment &environment, Projectile &projectile);

        void start();
        void stop();
        void update(float timeStep);
        void reset();

        bool isRunning() const { return running; }
        Projectile getProjectile() const;
        Environment getEnvironment() const;

        float getTimeGlobal() const { return timeGlobal; }
        float getPhysicsTimeStep() const { return timeStepPhysics; }
        void setPhysicsTimeStep(float timeStep);

    private:
        Projectile &projectile;
        Environment &environment;
        TrajectoryRecorder trajectoryRecorder;

        float timePerFrame{0.0f};
        float timeGlobal{0.0f};
        float timeStepPhysics{0.001f};

        bool running{false};
    };
}

#endif // SIMULATION_H
