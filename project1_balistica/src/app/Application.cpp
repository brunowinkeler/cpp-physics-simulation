#include "Application.h"

#include "core/RaylibDefinitions.h"

#include "imgui.h"
#include "raylib.h"
#include "rlImGui.h"

namespace physim
{
    const char *APP_TITLE{"Physics Simulation — Balistica v1.0"};

    Application::Application()
        : uiMenus{session},
          sceneRenderer{session},
          overlayRenderer{session.getSimulation()}
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
            const float frameTime = GetFrameTime();

            handleSimulationShortcuts();
            sceneRenderer.updateCamera(frameTime);
            updateSimulation(frameTime);
            draw();
        }
    }

    void Application::handleSimulationShortcuts()
    {
        if (ImGui::GetIO().WantCaptureKeyboard || ImGui::GetIO().WantTextInput)
        {
            return;
        }

        Simulation &simulation = session.getSimulation();

        if (IsKeyPressed(KEY_SPACE))
        {
            if (simulation.isRunning())
            {
                simulation.stop();
            }
            else
            {
                simulation.start();
            }
        }

        if (IsKeyPressed(KEY_R))
        {
            simulation.reset();
        }
    }

    void Application::updateSimulation(float frameTime)
    {
        const float timeStep = frameTime * session.getEnvironment().timeScale; // Scale time step by environment's time scale
        session.getSimulation().update(timeStep);
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
