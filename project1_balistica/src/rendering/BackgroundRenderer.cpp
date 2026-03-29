#include "BackgroundRenderer.h"
#include "core/RaylibDefinitions.h"

namespace physim
{
    inline constexpr float FLOOR_LEFT = -1000000.0f;
    inline constexpr float FLOOR_WIDTH = 2000000.0f;
    inline constexpr float FLOOR_HEIGHT = 1000000.0f;
    inline constexpr int MIN_WIDTH_BUILDING = 50;
    inline constexpr int MAX_WIDTH_BUILDING = 150;
    inline constexpr int MIN_HEIGHT_BUILDING = 100;
    inline constexpr int MAX_HEIGHT_BUILDING = 300;
    inline constexpr int MIN_X_SCREEN = -100;
    inline constexpr int MAX_X_SCREEN = 2000;
    inline constexpr int Y_GROUND = 0;

    BackgroundRenderer::BackgroundRenderer()
    {
        float spacing = MIN_X_SCREEN;
        // Initialize buildings with random positions and colors
        for (int i = 0; i < MAX_BUILDINGS; ++i)
        {
            buildings[i].rect = {
                .x = spacing,
                .y = static_cast<float>(GetRandomValue(MIN_HEIGHT_BUILDING, MAX_HEIGHT_BUILDING) + Y_GROUND),
                .width = static_cast<float>(GetRandomValue(MIN_WIDTH_BUILDING, MAX_WIDTH_BUILDING)),
                .height = static_cast<float>(GetRandomValue(MIN_HEIGHT_BUILDING, MAX_HEIGHT_BUILDING))};
            buildings[i].color = (Color){static_cast<unsigned char>(GetRandomValue(50, 200)),
                                         static_cast<unsigned char>(GetRandomValue(50, 200)),
                                         static_cast<unsigned char>(GetRandomValue(50, 200)),
                                         static_cast<unsigned char>(200)}; // alpha

            spacing += buildings[i].rect.width;
        }
    }

    void BackgroundRenderer::render()
    {
        drawFloor();

        // // Draw buildings
        // for (const auto &building : buildings)
        // {
        //     DrawRectangleRec(building.rect, building.color);
        // }
    }

    void BackgroundRenderer::drawFloor()
    {
        Rectangle floorRect = {FLOOR_LEFT, static_cast<float>(Y_GROUND), FLOOR_WIDTH, FLOOR_HEIGHT};
        DrawRectangleRec(floorRect, colors::DarkGray);
    }
} // namespace physim
