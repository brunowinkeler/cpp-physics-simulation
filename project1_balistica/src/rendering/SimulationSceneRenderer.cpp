#include "SimulationSceneRenderer.h"

#include "raylib.h"

namespace physim
{
    SimulationSceneRenderer::SimulationSceneRenderer(Simulation &simulation, Projectile &projectile)
        : backgroundRenderer{}, trajectoryRenderer{simulation, projectile}, projectileRenderer{projectile}
    {
    }

    void SimulationSceneRenderer::updateCamera(float frameTime)
    {
        camera.update(frameTime);
    }

    void SimulationSceneRenderer::render()
    {
        BeginMode2D(camera.getCamera2D());
        backgroundRenderer.render();
        trajectoryRenderer.render(camera.getCamera2D());
        projectileRenderer.render();
        EndMode2D();
    }

    void SimulationSceneRenderer::setHighlightedLaunches(int selectedLaunchId, int hoveredLaunchId)
    {
        trajectoryRenderer.setHighlightedLaunches(selectedLaunchId, hoveredLaunchId);
    }
} // namespace physim
