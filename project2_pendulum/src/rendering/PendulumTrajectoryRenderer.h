#ifndef PENDULUM_TRAJECTORY_RENDERER_H
#define PENDULUM_TRAJECTORY_RENDERER_H

#include "core/simulation/TrajectoryRecorder.h"

#include "raylib.h"

#include <vector>

namespace physim
{
    class PendulumSession;

    class PendulumTrajectoryRenderer
    {
    public:
        explicit PendulumTrajectoryRenderer(const PendulumSession &session)
            : session{session}
        {
        }

        void render() const;

    private:
        void drawTrail(const std::vector<TrajectoryPoint> &points, Color color, float thickness) const;

        const PendulumSession &session;
    };
} // namespace physim

#endif // PENDULUM_TRAJECTORY_RENDERER_H
