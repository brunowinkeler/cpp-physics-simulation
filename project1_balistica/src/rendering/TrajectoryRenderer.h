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
        void setHighlightedLaunches(int selectedLaunchId, int hoveredLaunchId)
        {
            this->selectedLaunchId = selectedLaunchId;
            this->hoveredLaunchId = hoveredLaunchId;
        }

    private:
        void renderHistoricalTrajectories() const;
        void renderCurrentTrajectory() const;
        void renderTrajectoryRecord(const std::vector<TrajectoryPoint> &trajectoryPoints,
                                    const std::optional<TrajectoryPoint> &apexPoint,
                                    float finalRange,
                                    bool landed,
                                    const TrajectoryStyle &style,
                                    int labelLane,
                                    int emphasisLevel) const;
        void drawTrajectoryPath(const std::vector<TrajectoryPoint> &trajectoryPoints,
                                const TrajectoryStyle &style,
                                int emphasisLevel) const;
        void drawApexMarker(const std::optional<TrajectoryPoint> &apexPoint,
                            const TrajectoryStyle &style,
                            int labelLane,
                            int emphasisLevel) const;
        void drawFinalRangeMarker(float finalRange,
                                  const TrajectoryStyle &style,
                                  int labelLane,
                                  int emphasisLevel) const;

        const Simulation &simulation;
        const Projectile &projectile;
        int selectedLaunchId{-1};
        int hoveredLaunchId{-1};
    };
} // namespace physim

#endif // TRAJECTORY_RENDERER_H
