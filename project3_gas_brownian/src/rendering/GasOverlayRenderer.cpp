#include "GasOverlayRenderer.h"

#include "core/RaylibDefinitions.h"
#include "simulation/GasSession.h"

#include "raylib.h"

namespace physim
{
    GasOverlayRenderer::GasOverlayRenderer(const GasSession &session)
        : session{session}
    {
    }

    void GasOverlayRenderer::render() const
    {
        const GasSimulation &simulation = session.getSimulation();
        const char *modeLabel = simulation.isBrownianMode() ? "Brownian" : "Ideal Gas";

        DrawText(TextFormat("Mode: %s", modeLabel), 20, GetScreenHeight() - 92, 20, colors::LightGray);
        DrawText("Space: start/pause | R: reset | 1/2: mode | T: tracer trail | F: camera", 20, GetScreenHeight() - 66, 18, colors::DarkGray);
        DrawText("Mouse drag: pan | Mouse wheel: zoom | Q/E: zoom | WASD: pan", 20, GetScreenHeight() - 42, 18, colors::DarkGray);
    }
} // namespace physim
