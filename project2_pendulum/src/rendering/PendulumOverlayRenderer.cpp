#include "PendulumOverlayRenderer.h"

#include "core/RaylibDefinitions.h"
#include "simulation/PendulumSession.h"

#include "imgui.h"
#include "raylib.h"

namespace physim
{
    namespace
    {
        constexpr float MARGIN = 20.0f;
        constexpr float FONT_SIZE = 18.0f;
        constexpr float FONT_SPACING = 2.0f;

        const char *modeLabel(PendulumMode mode)
        {
            switch (mode)
            {
            case PendulumMode::Double:
                return "Double";
            case PendulumMode::Simple:
            default:
                return "Simple";
            }
        }

        const char *stateLabel(PendulumSimulationState state)
        {
            switch (state)
            {
            case PendulumSimulationState::Running:
                return "Running";
            case PendulumSimulationState::Paused:
                return "Paused";
            case PendulumSimulationState::Idle:
            default:
                return "Idle";
            }
        }
    }

    void PendulumOverlayRenderer::render() const
    {
        renderStatus();
        renderInteractionHint();
        DrawFPS(20, GetScreenHeight() - 26);
    }

    void PendulumOverlayRenderer::renderStatus() const
    {
        const auto &simulation = session.getSimulation();
        const char *timeText = TextFormat("%s | %s | t = %.3f s", modeLabel(simulation.getMode()), stateLabel(simulation.getState()), simulation.getTimeGlobal());
        const char *energyText = TextFormat("E = %.4f J | dE = %+0.4f J", simulation.getCurrentTotalEnergy(), simulation.getEnergyDelta());

        const Vector2 timeTextSize = MeasureTextEx(GetFontDefault(), timeText, FONT_SIZE, FONT_SPACING);
        const Vector2 energyTextSize = MeasureTextEx(GetFontDefault(), energyText, FONT_SIZE, FONT_SPACING);
        const float width = timeTextSize.x > energyTextSize.x ? timeTextSize.x : energyTextSize.x;
        const Vector2 timePosition = {static_cast<float>(GetScreenWidth()) - width - MARGIN, MARGIN};
        const Vector2 energyPosition = {timePosition.x, timePosition.y + FONT_SIZE + 6.0f};

        DrawTextEx(GetFontDefault(), timeText, timePosition, FONT_SIZE, FONT_SPACING, colors::White);
        DrawTextEx(GetFontDefault(), energyText, energyPosition, FONT_SIZE, FONT_SPACING, colors::White);
    }

    void PendulumOverlayRenderer::renderInteractionHint() const
    {
        if (ImGui::GetIO().WantCaptureMouse)
        {
            return;
        }

        DrawCircleV(GetMousePosition(), 2.0f, colors::White);
        DrawText("WASD pan | Q/E zoom | F reset camera | Space start/pause | R reset | 1 simple | 2 double",
                 20,
                 20,
                 16,
                 colors::White);
    }
} // namespace physim
