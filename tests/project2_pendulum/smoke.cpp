#include "project2_pendulum/src/physics/PendulumMode.h"
#include "project2_pendulum/src/rendering/PendulumOverlayRenderer.h"
#include "project2_pendulum/src/rendering/PendulumSceneRenderer.h"
#include "project2_pendulum/src/simulation/PendulumSession.h"
#include "project2_pendulum/src/ui/PendulumUiMenus.h"

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
        std::puts("Project2PendulumSmokeTests skipped: no graphical display available.");
        return 0;
    }

    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(1280, 720, "Physics Simulation — Project 2 Smoke");
    rlImGuiSetup(true);

    physim::PendulumSession session;
    physim::PendulumSceneRenderer sceneRenderer{session};
    physim::PendulumOverlayRenderer overlayRenderer{session};
    physim::PendulumUiMenus uiMenus{session};

    if (automatedSmokeRun)
    {
        session.getEnvironment().timeScale = 200.0f;
        session.getSimulation().setPhysicsTimeStep(1.0f / 240.0f);
    }

    sceneRenderer.resetCamera();
    session.getSimulation().start();

    int remainingAutomatedFrames = automatedSmokeRun ? 8 : -1;
    bool switchedToDoubleMode = false;

    while (!WindowShouldClose() && !IsKeyDown(KEY_ESCAPE))
    {
        const float frameTime = automatedSmokeRun ? (1.0f / 60.0f) : GetFrameTime();

        if (automatedSmokeRun && !switchedToDoubleMode && remainingAutomatedFrames <= 6)
        {
            session.getSimulation().stop();
            session.getSimulation().setMode(physim::PendulumMode::Double);
            session.getSimulation().start();
            switchedToDoubleMode = true;
        }

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
