#ifndef PENDULUM_CAMERA_H
#define PENDULUM_CAMERA_H

#include "raylib.h"

namespace physim
{
    class PendulumCamera
    {
    public:
        void update(float frameTime);
        void reset();
        Camera2D &getCamera2D() { return camera2D; }
        const Camera2D &getCamera2D() const { return camera2D; }

    private:
        Camera2D camera2D{
            .offset = {640.0f, 140.0f},
            .target = {0.0f, 0.0f},
            .rotation = 0.0f,
            .zoom = 160.0f};
    };
} // namespace physim

#endif // PENDULUM_CAMERA_H
