#include "ProjectileRenderer.h"
#include "core/RaylibDefinitions.h"

#include "raylib.h"
#include "raymath.h"

#include "ProjectileCoordinatesAdapter.h"

namespace physim
{
    inline constexpr float RADIUS_SCALE = 5.0f;
    inline constexpr float TEXT_OFFSET = 8.0f;
    inline constexpr float FONT_SIZE = 14.0f;
    inline constexpr float FONT_SPACING = 2.0f;

    void ProjectileRenderer::render()
    {
        Position projectilePos = projectile.getPosition();
        Position screenPos = ProjectileCoordinatesAdapter::toScreenCoordinates(projectile.getPosition());
        float radius = projectile.getRadius() * RADIUS_SCALE; // Scale radius for better visibility
        screenPos.y -= radius;
        DrawCircleV({screenPos.x, screenPos.y}, radius, colors::LightPink);
        DrawTextEx(GetFontDefault(),
                   TextFormat("[%f, %f]", projectilePos.x, projectilePos.y),
                   Vector2Add((Vector2){screenPos.x, screenPos.y},
                              (Vector2){radius + TEXT_OFFSET, -radius - TEXT_OFFSET}),
                   FONT_SIZE, FONT_SPACING,
                   colors::White);
    }

} // namespace physim
