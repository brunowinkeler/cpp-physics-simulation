#ifndef SIMULATION_H
#define SIMULATION_H

#include "Projectile.h"
#include "Environment.h"

namespace physim
{
    class Simulation
    {
    public:
        Simulation(Environment &environment, Projectile &projectile);

        void run(float timeStep);
        void reset();

        Projectile getProjectile() const;
        Environment getEnvironment() const;

    private:
        Projectile &projectile;
        Environment &environment;
        float time;
    };
}

#endif // SIMULATION_H
