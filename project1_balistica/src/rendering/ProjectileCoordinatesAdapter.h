#ifndef PROJECTILE_COORDINATES_ADAPTER_H
#define PROJECTILE_COORDINATES_ADAPTER_H

namespace physim
{
    class Position;

    class ProjectileCoordinatesAdapter
    {
    public:
        static Position toScreenCoordinates(const Position &position);
    };
}

#endif // PROJECTILE_COORDINATES_ADAPTER_H
