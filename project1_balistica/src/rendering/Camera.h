#ifndef CAMERA_H
#define CAMERA_H

#include "raylib.h"

namespace physim
{
    class Camera
    {
    public:
        void update();
        void reset();
        Camera2D &getCamera2D() { return camera2D; }

    private:
        Camera2D camera2D{
            .offset = {540.0f, 600.0f},
            .target = {200.0f, 22.0f},
            .rotation = 0.0f,
            .zoom = 1.5f};
    };
} // namespace physim

#endif // CAMERA_H
