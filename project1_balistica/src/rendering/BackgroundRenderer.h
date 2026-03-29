#ifndef BACKGROUND_RENDERER_H
#define BACKGROUND_RENDERER_H

#include "raylib.h"

namespace physim
{
    class BackgroundRenderer
    {
    public:
        void render() const;

    private:
        void drawFloor() const;
    };
} // namespace physim

#endif // BACKGROUND_RENDERER_H
