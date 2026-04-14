#include "GasBrownianApplication.h"

#include "core/RaylibDefinitions.h"

#include "imgui.h"
#include "raylib.h"
#include "rlImGui.h"

namespace physim
{
    namespace
    {
        const char *APP_TITLE{"Physics Simulation - Gas and Brownian Lab v0.1"};
    }

    GasBrownianApplication::GasBrownianApplication()
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

    GasBrownianApplication::~GasBrownianApplication()
    {
        rlImGuiShutdown();
        CloseWindow();
    }

    void GasBrownianApplication::run()
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

    void GasBrownianApplication::handleSimulationShortcuts()
    {
        if (ImGui::GetIO().WantCaptureKeyboard || ImGui::GetIO().WantTextInput)
        {
            return;
        }

        GasSimulation &simulation = session.getSimulation();

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
            simulation.setMode(GasSimulationMode::IdealGas);
        }

        if (IsKeyPressed(KEY_TWO))
        {
            simulation.setMode(GasSimulationMode::Brownian);
        }

        if (IsKeyPressed(KEY_T))
        {
            simulation.setTracerTrailEnabled(!simulation.isTracerTrailEnabled());
        }
    }

    void GasBrownianApplication::updateSimulation(float frameTime)
    {
        const float timeStep = frameTime * session.getEnvironment().timeScale;
        session.getSimulation().update(timeStep);
    }

    void GasBrownianApplication::draw()
    {
        BeginDrawing();
        ClearBackground(colors::CatppuccinMocha);

        sceneRenderer.render();
        overlayRenderer.render();
        uiMenus.render();

        EndDrawing();
    }
} // namespace physim
