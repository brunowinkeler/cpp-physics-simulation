#include "Projectile.h"
#include "core/GlobalDefinitions.h"

#include <cmath>

namespace physim
{
    Projectile::Projectile(float velocity, float angle)
        : velocity{velocity}, angle{angle}, position{0.0, 0.0}, mass{1.0f} {}

    void Projectile::update(float timeStep, const Environment &env)
    {
        float angleRad = angle * constants::DEG_TO_RAD;

        float vx = velocity * cosf(angleRad);
        float vy = (velocity * sinf(angleRad)) - (env.gravity * timeStep);

        position.x = velocity * timeStep * cosf(angleRad);
    }

    void Projectile::reset()
    {
        position = {0.0, 0.0};
        velocity = 0.0f;
        angle = 0.0f;
        mass = 1.0f;
    }
} // namespace physim
