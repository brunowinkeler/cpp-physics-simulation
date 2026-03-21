#include "BackgroundRenderer.h"

namespace physim
{
    inline constexpr int MIN_WIDTH_BUILDING = 50;
    inline constexpr int MAX_WIDTH_BUILDING = 150;
    inline constexpr int MIN_HEIGHT_BUILDING = 100;
    inline constexpr int MAX_HEIGHT_BUILDING = 300;
    inline constexpr int MIN_X_SCREEN = -5000;
    inline constexpr int MAX_X_SCREEN = 5000;
    inline constexpr int Y_GROUND = 0;

    BackgroundRenderer::BackgroundRenderer()
    {
        float spacing = 0.0f;
        // Initialize buildings with random positions and colors
        for (int i = 0; i < MAX_BUILDINGS; ++i)
        {
            buildings[i].rect = {
                .x = spacing,
                .y = static_cast<float>(GetRandomValue(MIN_HEIGHT_BUILDING, MAX_HEIGHT_BUILDING) + Y_GROUND),
                .width = static_cast<float>(GetRandomValue(MIN_WIDTH_BUILDING, MAX_WIDTH_BUILDING)),
                .height = static_cast<float>(GetRandomValue(MIN_HEIGHT_BUILDING, MAX_HEIGHT_BUILDING))};
            buildings[i].color = (Color){GetRandomValue(50, 200), GetRandomValue(50, 200), GetRandomValue(50, 200), 255};

            spacing += buildings[i].rect.width;
        }
    }
} // namespace physim
