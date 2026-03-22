#ifndef SIMULATION_H
#define SIMULATION_H

#include "physics/Projectile.h"
#include "physics/Environment.h"

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

    private:
        Projectile &projectile;
        Environment &environment;
        float time;

        bool running{false};
    };
}

#endif // SIMULATION_H
