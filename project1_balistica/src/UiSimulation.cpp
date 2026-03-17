#include "UiSimulation.h"

#include "raylib.h"
#include "rlImGui.h"

namespace physim
{
    UiSimulation::UiSimulation()
    {
        // ------------------------------------------------------------------
        // Initialization
        // ------------------------------------------------------------------
        const int screenW = 1280;
        const int screenH = 720;

        SetConfigFlags(FLAG_VSYNC_HINT | FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE);
        InitWindow(screenW, screenH, "Physics Simulation — Infraestrutura v0.1");
        // SetTargetFPS(60);
        rlImGuiSetup(true); // dark theme
    }
}
