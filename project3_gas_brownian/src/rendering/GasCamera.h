#ifndef GAS_CAMERA_H
#define GAS_CAMERA_H

#include "raylib.h"

namespace physim
{
    class GasCamera
    {
    public:
        void update(float frameTime);
        void setTrackedBox(float boxWidth, float boxHeight);
        void reset();

        Camera2D &getCamera2D() { return camera2D; }
        const Camera2D &getCamera2D() const { return camera2D; }

    private:
        float trackedBoxWidth{12.0f};
        float trackedBoxHeight{7.0f};
        Camera2D camera2D{
            .offset = {640.0f, 360.0f},
            .target = {6.0f, -3.5f},
            .rotation = 0.0f,
            .zoom = 70.0f,
        };
    };
} // namespace physim

#endif // GAS_CAMERA_H
