#ifndef BACKGROUND_RENDERER_H
#define BACKGROUND_RENDERER_H

#include <array>
#include "raylib.h"

namespace physim
{
    inline constexpr int MAX_BUILDINGS = 100;

    struct Building
    {
        Rectangle rect;
        Color color;
    };

    class BackgroundRenderer
    {
    public:
        BackgroundRenderer();
        void render();

    private:
        std::array<Building, MAX_BUILDINGS> buildings;
    };
} // namespace physim

#endif // BACKGROUND_RENDERER_H
