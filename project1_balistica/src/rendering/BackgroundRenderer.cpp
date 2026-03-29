#include "BackgroundRenderer.h"
#include "core/RaylibDefinitions.h"

#include "ProjectileCoordinatesAdapter.h"

namespace physim
{
    inline constexpr float FLOOR_LEFT = -1000000.0f;
    inline constexpr float FLOOR_WIDTH = 2000000.0f;
    inline constexpr float FLOOR_DEPTH = 1000000.0f;

    void BackgroundRenderer::render() const
    {
        drawFloor();
    }

    void BackgroundRenderer::drawFloor() const
    {
        const Rectangle floorBounds = {FLOOR_LEFT, -FLOOR_DEPTH, FLOOR_WIDTH, FLOOR_DEPTH};
        DrawRectangleRec(ProjectileCoordinatesAdapter::toRenderRectangle(floorBounds), colors::DarkGray);
    }
} // namespace physim
