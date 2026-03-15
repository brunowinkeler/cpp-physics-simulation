#ifndef RAYLIB_DEFINITIONS_H
#define RAYLIB_DEFINITIONS_H

#include <raylib.h>

namespace physim
{
    namespace colors
    {
        inline constexpr ::Color Red{255, 0, 0, 255};
        inline constexpr ::Color Green{0, 255, 0, 255};
        inline constexpr ::Color LightGreen{166, 227, 161, 255};
        inline constexpr ::Color Blue{0, 0, 255, 255};
        inline constexpr ::Color LightBlue{137, 220, 235, 255};
        inline constexpr ::Color Yellow{255, 255, 0, 255};
        inline constexpr ::Color Magenta{255, 0, 255, 255};
        inline constexpr ::Color Cyan{0, 255, 255, 255};
        inline constexpr ::Color White{255, 255, 255, 255};
        inline constexpr ::Color Black{0, 0, 0, 255};
        inline constexpr ::Color LightGray{200, 200, 200, 255};
        inline constexpr ::Color DarkGray{80, 80, 80, 255};
        inline constexpr ::Color Orange{255, 179, 135, 255};
        inline constexpr ::Color CatppuccinMocha{30, 30, 46, 255};
        inline constexpr ::Color LightPink{243, 139, 168, 255};
    }
}

#endif // RAYLIB_DEFINITIONS_H
