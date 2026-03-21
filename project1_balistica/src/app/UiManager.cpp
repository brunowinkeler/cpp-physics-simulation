#include "UiManager.h"
#include "core/RaylibDefinitions.h"
#include "ui/UiMenus.h"

#include "raylib.h"
#include "rlImGui.h"

namespace physim
{
    const char *APP_TITLE{"Physics Simulation — Balistica v1.0"};

    UiManager::UiManager() : simulation{environment, projectile}, uiMenus{environment, projectile}
    {
        const int screenW = 1280;
        const int screenH = 720;

        SetConfigFlags(FLAG_WINDOW_MAXIMIZED | FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
        InitWindow(screenW, screenH, APP_TITLE);
        SetTargetFPS(60);   // Fix the frame rate to 60 FPS, for now
        rlImGuiSetup(true); // dark theme
    }

    UiManager::~UiManager()
    {
        rlImGuiShutdown();
        CloseWindow();
    }

    void UiManager::run()
    {
        while (!WindowShouldClose())
        {
            updateSimulation();

            draw();
        }
    }

    void UiManager::updateSimulation()
    {
        simulation.run(0.01);
    }

    void UiManager::updateEvents()
    {
    }

    void UiManager::draw()
    {
        BeginDrawing();
        ClearBackground(physim::colors::CatppuccinMocha); // dark background (Catppuccin Mocha)

        uiMenus.parametersSelectionScreen();

        DrawFPS(20, GetScreenHeight() - 26);
        EndDrawing();
    }

}
