#include "PendulumTrajectoryRenderer.h"

#include "PendulumCoordinatesAdapter.h"
#include "core/RaylibDefinitions.h"
#include "core/simulation/TrajectoryRecorder.h"
#include "simulation/PendulumSession.h"

#include "raylib.h"

namespace physim
{
    void PendulumTrajectoryRenderer::render() const
    {
        const auto &visualization = session.getVisualizationSettings();
        const auto &simulation = session.getSimulation();

        if (visualization.showPrimaryTrail)
        {
            drawTrail(simulation.getPrimaryTrailPoints(), {137, 220, 235, 220}, 0.028f);
        }

        if (simulation.isDoubleMode() && visualization.showSecondaryTrail)
        {
            drawTrail(simulation.getSecondaryTrailPoints(), {255, 179, 135, 190}, 0.022f);
        }
    }

    void PendulumTrajectoryRenderer::drawTrail(const std::vector<TrajectoryPoint> &points, Color color, float thickness) const
    {
        if (points.size() < 2)
        {
            return;
        }

        for (std::size_t index = 1; index < points.size(); ++index)
        {
            DrawLineEx(PendulumCoordinatesAdapter::toRenderPosition(points[index - 1].x, points[index - 1].y),
                       PendulumCoordinatesAdapter::toRenderPosition(points[index].x, points[index].y),
                       thickness,
                       color);
        }
    }
} // namespace physim
