#ifndef GAS_COLLISION_H
#define GAS_COLLISION_H

#include "GasParticle.h"

namespace physim
{
    struct GasBoxBounds
    {
        float width;
        float height;
    };

    bool resolveWallCollision(GasParticle &particle, const GasBoxBounds &boxBounds, float &wallImpulseAccumulator);
    bool resolveParticleCollision(GasParticle &first, GasParticle &second);
} // namespace physim

#endif // GAS_COLLISION_H
