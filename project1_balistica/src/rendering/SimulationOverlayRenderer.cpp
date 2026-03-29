#include "SimulationOverlayRenderer.h"

#include "core/RaylibDefinitions.h"
#include "simulation/Simulation.h"

#include "imgui.h"
#include "raylib.h"

namespace physim
{
    namespace
    {
        constexpr float MARGIN = 20.0f;
        constexpr float FONT_SIZE = 18.0f;
        constexpr float FONT_SPACING = 2.0f;
    }

    void SimulationOverlayRenderer::render() const
    {
        renderSimulationTime();
        renderInteractionHint();
        DrawFPS(20, GetScreenHeight() - 26);
    }

    void SimulationOverlayRenderer::renderSimulationTime() const
    {
        const char *timeText = nullptr;
        switch (simulation.getState())
        {
        case SimulationState::Landed:
            timeText = TextFormat("Flight Time: %.3f s", simulation.getTimeGlobal());
            break;
        case SimulationState::Paused:
            timeText = TextFormat("Paused Time: %.3f s", simulation.getTimeGlobal());
            break;
        case SimulationState::Idle:
        case SimulationState::Running:
        default:
            timeText = TextFormat("Simulation Time: %.3f s", simulation.getTimeGlobal());
            break;
        }

        const Vector2 textSize = MeasureTextEx(GetFontDefault(), timeText, FONT_SIZE, FONT_SPACING);
        const Vector2 textPosition = {static_cast<float>(GetScreenWidth()) - textSize.x - MARGIN, MARGIN};
        DrawTextEx(GetFontDefault(), timeText, textPosition, FONT_SIZE, FONT_SPACING, colors::White);
    }

    void SimulationOverlayRenderer::renderInteractionHint() const
    {
        if (ImGui::GetIO().WantCaptureMouse)
        {
            return;
        }

        DrawCircleV(GetMousePosition(), 2, colors::White);
        DrawText("WASD pan | Q/E zoom | F reset camera | Space start/pause | R reset", 20, 20, 16, colors::White);
    }
} // namespace physim
