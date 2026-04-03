#ifndef PENDULUM_SCENE_RENDERER_H
#define PENDULUM_SCENE_RENDERER_H

#include "PendulumBackgroundRenderer.h"
#include "PendulumCamera.h"
#include "PendulumRenderer.h"
#include "PendulumTrajectoryRenderer.h"

namespace physim
{
    class PendulumSession;

    class PendulumSceneRenderer
    {
    public:
        explicit PendulumSceneRenderer(const PendulumSession &session);

        void updateCamera(float frameTime);
        void resetCamera();
        void render();

    private:
        PendulumBackgroundRenderer backgroundRenderer;
        PendulumTrajectoryRenderer trajectoryRenderer;
        PendulumRenderer pendulumRenderer;
        PendulumCamera camera;
    };
} // namespace physim

#endif // PENDULUM_SCENE_RENDERER_H
