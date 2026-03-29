#ifndef TRAJECTORY_RENDERER_H
#define TRAJECTORY_RENDERER_H

#include "physics/Projectile.h"
#include "simulation/Simulation.h"

namespace physim
{
    class TrajectoryRenderer
    {
    public:
        TrajectoryRenderer(const Simulation &simulation, const Projectile &projectile)
            : simulation{simulation}, projectile{projectile}
        {
        }

        void render() const;

    private:
        void drawTrajectoryPath() const;
        void drawApexMarker() const;

        const Simulation &simulation;
        const Projectile &projectile;
    };
} // namespace physim

#endif // TRAJECTORY_RENDERER_H
