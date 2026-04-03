#ifndef PENDULUM_COORDINATES_ADAPTER_H
#define PENDULUM_COORDINATES_ADAPTER_H

#include "raylib.h"

namespace physim
{
    class PendulumCoordinatesAdapter
    {
    public:
        static Vector2 toRenderPosition(float x, float y)
        {
            return {x, -y};
        }

        static Vector2 toScreenPosition(float x, float y, const Camera2D &camera2D)
        {
            return GetWorldToScreen2D(toRenderPosition(x, y), camera2D);
        }
    };
} // namespace physim

#endif // PENDULUM_COORDINATES_ADAPTER_H
