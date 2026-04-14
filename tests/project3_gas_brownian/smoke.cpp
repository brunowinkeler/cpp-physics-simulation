#include "project3_gas_brownian/src/physics/GasMode.h"
#include "project3_gas_brownian/src/rendering/GasOverlayRenderer.h"
#include "project3_gas_brownian/src/rendering/GasSceneRenderer.h"
#include "project3_gas_brownian/src/simulation/GasSession.h"
#include "project3_gas_brownian/src/ui/GasUiMenus.h"

#include "core/RaylibDefinitions.h"

#include "raylib.h"
#include "rlImGui.h"

#include <cstdlib>
#include <cstdio>
#include <string_view>

namespace
{
    bool hasFlag(int argc, char **argv, std::string_view flag)
    {
        for (int index = 1; index < argc; ++index)
        {
            if (argv[index] == flag)
            {
                return true;
            }
        }

        return false;
    }

    bool hasGraphicalDisplay()
    {
        return std::getenv("DISPLAY") != nullptr || std::getenv("WAYLAND_DISPLAY") != nullptr;
    }
}

int main(int argc, char **argv)
{
    const bool automatedSmokeRun = hasFlag(argc, argv, "--ctest");
    if (automatedSmokeRun && !hasGraphicalDisplay())
    {
        std::puts("Project3GasBrownianSmokeTests skipped: no graphical display available.");
        return 0;
    }

    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(1280, 720, "Physics Simulation - Project 3 Smoke");
    rlImGuiSetup(true);

    physim::GasSession session;
    physim::GasSceneRenderer sceneRenderer{session};
    physim::GasOverlayRenderer overlayRenderer{session};
    physim::GasUiMenus uiMenus{session};

    if (automatedSmokeRun)
    {
        session.getEnvironment().timeScale = 20.0f;
        session.getSimulation().setParticleCount(1000);
        session.getSimulation().setMode(physim::GasSimulationMode::Brownian);
        session.getSimulation().setPhysicsTimeStep(1.0f / 240.0f);
        session.getSimulation().setTracerTrailEnabled(true);
    }

    sceneRenderer.resetCamera();
    session.getSimulation().start();

    int remainingAutomatedFrames = automatedSmokeRun ? 12 : -1;

    while (!WindowShouldClose() && !IsKeyDown(KEY_ESCAPE))
    {
        const float frameTime = automatedSmokeRun ? (1.0f / 60.0f) : GetFrameTime();

        sceneRenderer.updateCamera(frameTime);
        session.getSimulation().update(frameTime * session.getEnvironment().timeScale);

        BeginDrawing();
        ClearBackground(physim::colors::CatppuccinMocha);
        sceneRenderer.render();
        overlayRenderer.render();
        uiMenus.render();
        EndDrawing();

        if (automatedSmokeRun)
        {
            --remainingAutomatedFrames;
            if (remainingAutomatedFrames <= 0)
            {
                break;
            }
        }
    }

    rlImGuiShutdown();
    CloseWindow();
    return 0;
}
