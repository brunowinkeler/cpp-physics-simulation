#include "PendulumApplication.h"

#include "core/RaylibDefinitions.h"

#include "imgui.h"
#include "raylib.h"
#include "rlImGui.h"

namespace physim
{
    namespace
    {
        const char *APP_TITLE{"Physics Simulation - Pendulum Lab v0.1"};
    }

    PendulumApplication::PendulumApplication()
        : uiMenus{session},
          sceneRenderer{session},
          overlayRenderer{session}
    {
        SetConfigFlags(FLAG_WINDOW_RESIZABLE);
        InitWindow(1280, 720, APP_TITLE);
        MaximizeWindow();
        rlImGuiSetup(true);
        sceneRenderer.resetCamera();
    }

    PendulumApplication::~PendulumApplication()
    {
        rlImGuiShutdown();
        CloseWindow();
    }

    void PendulumApplication::run()
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

    void PendulumApplication::handleSimulationShortcuts()
    {
        if (ImGui::GetIO().WantCaptureKeyboard || ImGui::GetIO().WantTextInput)
        {
            return;
        }

        PendulumSimulation &simulation = session.getSimulation();

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

        if (IsKeyPressed(KEY_ONE))
        {
            simulation.setMode(PendulumMode::Simple);
        }

        if (IsKeyPressed(KEY_TWO))
        {
            simulation.setMode(PendulumMode::Double);
        }
    }

    void PendulumApplication::updateSimulation(float frameTime)
    {
        const float timeStep = frameTime * session.getEnvironment().timeScale;
        session.getSimulation().update(timeStep);
    }

    void PendulumApplication::draw()
    {
        BeginDrawing();
        ClearBackground(colors::CatppuccinMocha);

        sceneRenderer.render();
        overlayRenderer.render();
        uiMenus.render();

        EndDrawing();
    }
} // namespace physim
