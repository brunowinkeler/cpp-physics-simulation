#include "ProjectileCoordinatesAdapter.h"

namespace physim
{
    Vector2 ProjectileCoordinatesAdapter::toRenderPosition(const Position &position)
    {
        return toRenderPosition(position.x, position.y);
    }

    Vector2 ProjectileCoordinatesAdapter::toRenderPosition(float x, float y)
    {
        return {x, -y};
    }

    Vector2 ProjectileCoordinatesAdapter::toScreenPosition(const Position &position, const Camera2D &camera2D)
    {
        return toScreenPosition(position.x, position.y, camera2D);
    }

    Vector2 ProjectileCoordinatesAdapter::toScreenPosition(float x, float y, const Camera2D &camera2D)
    {
        const Vector2 renderPosition = toRenderPosition(x, y);
        return GetWorldToScreen2D(renderPosition, camera2D);
    }

    Rectangle ProjectileCoordinatesAdapter::toRenderRectangle(const Rectangle &worldRectangle)
    {
        return {
            worldRectangle.x,
            -(worldRectangle.y + worldRectangle.height),
            worldRectangle.width,
            worldRectangle.height,
        };
    }
}
