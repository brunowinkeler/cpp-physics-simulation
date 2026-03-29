#ifndef PROJECTILE_COORDINATES_ADAPTER_H
#define PROJECTILE_COORDINATES_ADAPTER_H

#include "physics/Projectile.h"

#include "raylib.h"

namespace physim
{
    class ProjectileCoordinatesAdapter
    {
    public:
        static Vector2 toRenderPosition(const Position &position);
        static Vector2 toRenderPosition(float x, float y);
        static Vector2 toScreenPosition(const Position &position, const Camera2D &camera2D);
        static Vector2 toScreenPosition(float x, float y, const Camera2D &camera2D);
        static Rectangle toRenderRectangle(const Rectangle &worldRectangle);
    };
}

#endif // PROJECTILE_COORDINATES_ADAPTER_H
