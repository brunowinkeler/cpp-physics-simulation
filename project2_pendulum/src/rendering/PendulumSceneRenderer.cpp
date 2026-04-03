#include "PendulumSceneRenderer.h"

#include "raylib.h"

namespace physim
{
    PendulumSceneRenderer::PendulumSceneRenderer(const PendulumSession &session)
        : backgroundRenderer{session},
          trajectoryRenderer{session},
          pendulumRenderer{session}
    {
    }

    void PendulumSceneRenderer::updateCamera(float frameTime)
    {
        camera.update(frameTime);
    }

    void PendulumSceneRenderer::resetCamera()
    {
        camera.reset();
    }

    void PendulumSceneRenderer::render()
    {
        BeginMode2D(camera.getCamera2D());
        backgroundRenderer.render();
        trajectoryRenderer.render();
        pendulumRenderer.render();
        EndMode2D();
    }
} // namespace physim
