#ifndef SIMULATION_SCENE_RENDERER_H
#define SIMULATION_SCENE_RENDERER_H

#include "BackgroundRenderer.h"
#include "Camera.h"
#include "ProjectileRenderer.h"
#include "TrajectoryRenderer.h"

namespace physim
{
    class Simulation;
    class Projectile;

    class SimulationSceneRenderer
    {
    public:
        SimulationSceneRenderer(Simulation &simulation, Projectile &projectile);

        void updateCamera();
        void render();
        void setHighlightedLaunches(int selectedLaunchId, int hoveredLaunchId);

    private:
        BackgroundRenderer backgroundRenderer;
        TrajectoryRenderer trajectoryRenderer;
        ProjectileRenderer projectileRenderer;
        Camera camera;
    };
} // namespace physim

#endif // SIMULATION_SCENE_RENDERER_H
