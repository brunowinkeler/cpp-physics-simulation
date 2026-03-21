#ifndef PROJECTILE_RENDERER_H
#define PROJECTILE_RENDERER_H

#include "physics/Projectile.h"

namespace physim
{
    class ProjectileRenderer
    {
    public:
        ProjectileRenderer(Projectile &projectile) : projectile{projectile} {};
        ~ProjectileRenderer();

        void render();

    private:
        Projectile &projectile;
    };

} // namespace physim

#endif // PROJECTILE_RENDERER_H
