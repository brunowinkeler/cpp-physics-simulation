#include "raylib.h"
#include "rlImGui.h"
#include "imgui.h"
#include "glm/glm.hpp"
#include "glm/gtc/constants.hpp"
#include <cstdio>

// Smoke-test: verifies that all dependencies compile and link correctly.
// Project 1 starts once this window opens without errors.

int main()
{
    // ------------------------------------------------------------------
    // Initialization
    // ------------------------------------------------------------------
    const int screenW = 900;
    const int screenH = 600;

    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(screenW, screenH, "Physics Simulation — Infraestrutura v0.1");
    // SetTargetFPS(60);
    rlImGuiSetup(true);

    // ------------------------------------------------------------------
    // Minimum state for the ImGui panel
    // ------------------------------------------------------------------
    float gravity = 9.81f;
    float timeScale = 1.0f;
    bool showDemo = false;

    // GLM: just to confirm the library is available
    glm::vec2 origin{static_cast<float>(screenW) / 2.f,
                     static_cast<float>(screenH) / 2.f};

    // ------------------------------------------------------------------
    // Main loop
    // ------------------------------------------------------------------
    while (!WindowShouldClose() && !IsKeyDown(KEY_ESCAPE))
    {
        BeginDrawing();
        ClearBackground({30, 30, 46, 255}); // dark background (Catppuccin Mocha)

        // --- Raylib: status text -----------------------------------
        DrawText("Raylib OK", 20, 20, 20, {166, 227, 161, 255}); // green
        DrawText("ImGui  OK", 20, 46, 20, {137, 220, 235, 255}); // blue
        DrawText("GLM    OK", 20, 72, 20, {250, 179, 135, 255}); // orange

        char coordBuf[64];
        snprintf(coordBuf, sizeof(coordBuf),
                 "GLM vec2: (%.0f, %.0f)", origin.x, origin.y);
        DrawText(coordBuf, 20, 98, 18, LIGHTGRAY);

        // Circle representing the origin of the coordinate system
        DrawCircleV({origin.x, origin.y}, 8.f, {243, 139, 168, 255});

        // --- Dear ImGui: controls panel ---------------------------
        rlImGuiBegin();

        ImGui::SetNextWindowPos({620, 20}, ImGuiCond_Once);
        ImGui::SetNextWindowSize({260, 180}, ImGuiCond_Once);
        ImGui::Begin("Simulation Controls");

        ImGui::SliderFloat("Gravity (m/s^2)", &gravity, 0.f, 30.f, "%.2f");
        ImGui::SliderFloat("Time Scale", &timeScale, 0.1f, 5.f, "%.1fx");
        ImGui::Separator();
        ImGui::Checkbox("Show ImGui Demo", &showDemo);
        ImGui::Spacing();
        ImGui::TextDisabled("Stack: Raylib 5 + ImGui + GLM");

        ImGui::End();

        if (showDemo)
            ImGui::ShowDemoWindow(&showDemo);

        rlImGuiEnd();

        // FPS at bottom-left corner
        DrawFPS(20, GetScreenHeight() - 26);

        EndDrawing();
    }

    // ------------------------------------------------------------------
    // Cleanup
    // ------------------------------------------------------------------
    rlImGuiShutdown();
    CloseWindow();
    return 0;
}
