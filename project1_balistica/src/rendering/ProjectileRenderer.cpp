#include "ProjectileRenderer.h"
#include "core/RaylibDefinitions.h"

#include "raylib.h"

namespace physim
{
    void ProjectileRenderer::render()
    {
        Position pos = projectile.getPosition();
        DrawCircleV({pos.x, pos.y}, projectile.getRadius() * 10.0f, colors::LightPink);
    }

} // namespace physim
