#include "Application.h"

#include "core/RaylibDefinitions.h"

#include "raylib.h"
#include "rlImGui.h"

namespace physim
{
    const char *APP_TITLE{"Physics Simulation — Balistica v1.0"};

    Application::Application() : simulation{environment, projectile}, uiMenus{environment, projectile, simulation}
    {
        const int screenW = 1280;
        const int screenH = 720;

        SetConfigFlags(FLAG_WINDOW_RESIZABLE);
        InitWindow(screenW, screenH, APP_TITLE);
        MaximizeWindow();
        rlImGuiSetup(true); // dark theme
    }

    Application::~Application()
    {
        rlImGuiShutdown();
        CloseWindow();
    }

    void Application::run()
    {
        while (!WindowShouldClose())
        {
            sceneRenderer.updateCamera();
            updateSimulation();
            draw();
        }
    }

    void Application::updateSimulation()
    {
        float timeStep = GetFrameTime() * environment.timeScale; // Scale time step by environment's time scale
        simulation.update(timeStep);
    }

    void Application::syncSceneHighlights()
    {
        sceneRenderer.setHighlightedLaunches(uiMenus.getSelectedHistoryEntryId(), uiMenus.getHoveredHistoryEntryId());
    }

    void Application::draw()
    {
        BeginDrawing();
        ClearBackground(physim::colors::CatppuccinMocha); // dark background (Catppuccin Mocha)

        syncSceneHighlights();
        sceneRenderer.render();
        overlayRenderer.render();

        uiMenus.parametersSelectionScreen();
        syncSceneHighlights();
        EndDrawing();
    }

}
