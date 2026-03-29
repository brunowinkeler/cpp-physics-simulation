#ifndef PROJECTILE_RENDERER_H
#define PROJECTILE_RENDERER_H

#include "physics/Projectile.h"

namespace physim
{
    class ProjectileRenderer
    {
    public:
        explicit ProjectileRenderer(const Projectile &projectile) : projectile{projectile} {}

        void render() const;

    private:
        const Projectile &projectile;
    };

} // namespace physim

#endif // PROJECTILE_RENDERER_H
