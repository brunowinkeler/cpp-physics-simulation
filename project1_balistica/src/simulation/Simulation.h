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

        Projectile getProjectile() const;
        Environment getEnvironment() const;

        float getTimeGlobal() const { return timeGlobal; }

    private:
        Projectile &projectile;
        Environment &environment;
        TrajectoryRecorder trajectoryRecorder;

        float timePerFrame;
        float timeGlobal;
        float timeStepPhysics{0.001f};

        bool running{false};
    };
}

#endif // SIMULATION_H
