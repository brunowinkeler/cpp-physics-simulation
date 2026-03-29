#include "ProjectileRenderer.h"
#include "core/RaylibDefinitions.h"

#include "raylib.h"

#include <algorithm>

#include "ProjectileCoordinatesAdapter.h"

namespace physim
{
    inline constexpr float RADIUS_SCALE = 5.0f;
    inline constexpr float MIN_SCREEN_RADIUS = 4.0f;
    inline constexpr float TEXT_OFFSET = 8.0f;
    inline constexpr float FONT_SIZE = 14.0f;
    inline constexpr float FONT_SPACING = 2.0f;

    void ProjectileRenderer::render()
    {
        const Position projectilePos = projectile.getPosition();
        Vector2 renderPos = ProjectileCoordinatesAdapter::toRenderPosition(projectilePos);
        float radius = std::max(projectile.getRadius() * RADIUS_SCALE, MIN_SCREEN_RADIUS);
        renderPos.y -= radius;
        DrawCircleV(renderPos, radius, colors::LightPink);
        const Vector2 labelPosition = {
            renderPos.x + radius + TEXT_OFFSET,
            renderPos.y - radius - TEXT_OFFSET,
        };
        DrawTextEx(GetFontDefault(),
                   TextFormat("[%f, %f]", projectilePos.x, projectilePos.y),
                   labelPosition,
                   FONT_SIZE, FONT_SPACING,
                   colors::White);
    }

} // namespace physim
