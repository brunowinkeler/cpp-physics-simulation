#ifndef TRAJECTORY_RENDERER_H
#define TRAJECTORY_RENDERER_H

#include "physics/Projectile.h"
#include "simulation/Simulation.h"

#include "raylib.h"

namespace physim
{
    class TrajectoryRenderer
    {
    public:
        TrajectoryRenderer(const Simulation &simulation, const Projectile &projectile)
            : simulation{simulation}, projectile{projectile}
        {
        }

        void render(const Camera2D &camera2D) const;
        void setHighlightedLaunches(int selectedLaunchId, int hoveredLaunchId)
        {
            this->selectedLaunchId = selectedLaunchId;
            this->hoveredLaunchId = hoveredLaunchId;
        }

    private:
        int resolveHoveredTrajectoryId(const Camera2D &camera2D) const;
        float calculateTrajectoryHoverDistanceSquared(const std::vector<TrajectoryPoint> &trajectoryPoints,
                                                      const Camera2D &camera2D) const;
        void renderHistoricalTrajectories(int hoveredTrajectoryId) const;
        void renderCurrentTrajectory(int hoveredTrajectoryId) const;
        void renderTrajectoryRecord(const std::vector<TrajectoryPoint> &trajectoryPoints,
                                    const std::optional<TrajectoryPoint> &apexPoint,
                                    float finalRange,
                                    const TrajectoryStyle &style,
                                    int labelLane,
                                    int emphasisLevel,
                                    bool showLabels) const;
        void drawTrajectoryPath(const std::vector<TrajectoryPoint> &trajectoryPoints,
                                const TrajectoryStyle &style,
                                int emphasisLevel) const;
        void drawApexMarker(const std::optional<TrajectoryPoint> &apexPoint,
                            const TrajectoryStyle &style,
                            int labelLane,
                            int emphasisLevel,
                            bool showLabels) const;
        void drawFinalRangeMarker(float finalRange,
                                  const TrajectoryStyle &style,
                                  int labelLane,
                                  int emphasisLevel,
                                  bool showLabel) const;

        const Simulation &simulation;
        const Projectile &projectile;
        int selectedLaunchId{-1};
        int hoveredLaunchId{-1};
    };
} // namespace physim

#endif // TRAJECTORY_RENDERER_H
