#include "Application.h"
#include "core/RaylibDefinitions.h"
#include "ui/UiMenus.h"

#include "raylib.h"
#include "rlImGui.h"
#include "raymath.h"

#include "imgui.h"

namespace physim
{
    const char *APP_TITLE{"Physics Simulation — Balistica v1.0"};

    Application::Application() : simulation{environment, projectile}, uiMenus{environment, projectile, simulation}, backgroundRenderer{}
    {
        const int screenW = 1280;
        const int screenH = 720;

        SetConfigFlags(FLAG_WINDOW_MAXIMIZED | FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
        InitWindow(screenW, screenH, APP_TITLE);
        SetTargetFPS(60);   // Fix the frame rate to 60 FPS, for now
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
            camera.update();
            updateSimulation();

            draw();
        }
    }

    void Application::updateSimulation()
    {
        float timeStep = GetFrameTime() * environment.timeScale; // Scale time step by environment's time scale
        simulation.update(timeStep);
    }

    void Application::updateEvents()
    {
    }

    void Application::drawWithCamera()
    {
        BeginMode2D(camera.getCamera2D());
        backgroundRenderer.render();
        trajectoryRenderer.render();
        projectileRenderer.render();
        EndMode2D();
    }

    void Application::drawSimulationOverlay() const
    {
        constexpr float MARGIN = 20.0f;
        constexpr float FONT_SIZE = 18.0f;
        constexpr float FONT_SPACING = 2.0f;

        const char *timeText = projectile.isLanded()
                                   ? TextFormat("Flight Time: %.3f s", simulation.getTimeGlobal())
                                   : TextFormat("Simulation Time: %.3f s", simulation.getTimeGlobal());
        const Vector2 textSize = MeasureTextEx(GetFontDefault(), timeText, FONT_SIZE, FONT_SPACING);
        const Vector2 textPosition = {static_cast<float>(GetScreenWidth()) - textSize.x - MARGIN, MARGIN};

        DrawTextEx(GetFontDefault(), timeText, textPosition, FONT_SIZE, FONT_SPACING, colors::White);
    }

    void Application::draw()
    {
        BeginDrawing();
        ClearBackground(physim::colors::CatppuccinMocha); // dark background (Catppuccin Mocha)

        drawWithCamera();
        drawSimulationOverlay();

        uiMenus.parametersSelectionScreen();

        // Draw mouse reference
        if (!ImGui::GetIO().WantCaptureMouse)
        {
            Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), camera.getCamera2D());
            DrawCircleV(GetMousePosition(), 2, colors::White);
            DrawTextEx(GetFontDefault(), TextFormat("[%i, %i]", (int)mouseWorldPos.x, (int)mouseWorldPos.y),
                       Vector2Add(GetMousePosition(), (Vector2){-44, -24}), 16, 2, colors::White);
            DrawText("Mouse left button drag to move, mouse wheel to zoom", 20, 20, 16, colors::White);
        }

        DrawFPS(20, GetScreenHeight() - 26);
        EndDrawing();
    }

}
