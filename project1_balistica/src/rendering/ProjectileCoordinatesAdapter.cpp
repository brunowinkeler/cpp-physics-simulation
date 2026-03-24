#include "ProjectileCoordinatesAdapter.h"
#include "physics/Projectile.h"

namespace physim
{
    Position ProjectileCoordinatesAdapter::toScreenCoordinates(const Position &position)
    {
        Position screenPosition = position;
        screenPosition.x = position.x;  // Scale world x to screen x
        screenPosition.y = -position.y; // Scale world y to screen
        return screenPosition;
    }
}
