#ifndef GAS_PARTICLE_H
#define GAS_PARTICLE_H

#include "GasMath.h"

namespace physim
{
    struct GasParticle
    {
        GasVector2 position;
        GasVector2 velocity;
        float mass;
        float radius;
        bool isTracer{false};
    };
} // namespace physim

#endif // GAS_PARTICLE_H
