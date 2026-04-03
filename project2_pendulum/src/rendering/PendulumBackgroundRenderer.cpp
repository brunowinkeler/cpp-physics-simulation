#include "PendulumBackgroundRenderer.h"

#include "PendulumCoordinatesAdapter.h"
#include "core/RaylibDefinitions.h"
#include "simulation/PendulumSession.h"

#include "raylib.h"

#include <algorithm>
#include <cmath>

namespace physim
{
    namespace
    {
        constexpr float MIN_SCENE_EXTENT = 4.0f;
        constexpr float SCENE_EXTENT_SCALE = 3.0f;
        constexpr float MINOR_GRID_STEP = 0.5f;
        constexpr float MAJOR_GRID_STEP = 1.0f;
        constexpr Color MINOR_GRID_COLOR{70, 70, 90, 120};
        constexpr Color MAJOR_GRID_COLOR{110, 110, 130, 170};
        constexpr Color AXIS_X_COLOR{148, 226, 213, 230};
        constexpr Color AXIS_Y_COLOR{245, 194, 231, 230};
    }

    void PendulumBackgroundRenderer::render() const
    {
        const auto &visualization = session.getVisualizationSettings();
        const float extent = resolveSceneExtent();

        if (visualization.showGrid)
        {
            drawGrid(extent);
        }

        if (visualization.showAxes)
        {
            drawAxes(extent);
        }

        drawPivot();
    }

    float PendulumBackgroundRenderer::resolveSceneExtent() const
    {
        const auto &simulation = session.getSimulation();
        if (simulation.isDoubleMode())
        {
            return std::max(MIN_SCENE_EXTENT,
                            (session.getDoublePendulum().getLength1() + session.getDoublePendulum().getLength2()) * SCENE_EXTENT_SCALE);
        }

        return std::max(MIN_SCENE_EXTENT, session.getSimplePendulum().getLength() * SCENE_EXTENT_SCALE);
    }

    void PendulumBackgroundRenderer::drawGrid(float extent) const
    {
        for (float x = -extent; x <= extent; x += MINOR_GRID_STEP)
        {
            const bool isMajorLine = std::abs(std::fmod(x, MAJOR_GRID_STEP)) < 1.0e-4f;
            const Color lineColor = isMajorLine ? MAJOR_GRID_COLOR : MINOR_GRID_COLOR;
            DrawLineEx(PendulumCoordinatesAdapter::toRenderPosition(x, -extent),
                       PendulumCoordinatesAdapter::toRenderPosition(x, extent),
                       isMajorLine ? 0.018f : 0.012f,
                       lineColor);
        }

        for (float y = -extent; y <= extent; y += MINOR_GRID_STEP)
        {
            const bool isMajorLine = std::abs(std::fmod(y, MAJOR_GRID_STEP)) < 1.0e-4f;
            const Color lineColor = isMajorLine ? MAJOR_GRID_COLOR : MINOR_GRID_COLOR;
            DrawLineEx(PendulumCoordinatesAdapter::toRenderPosition(-extent, y),
                       PendulumCoordinatesAdapter::toRenderPosition(extent, y),
                       isMajorLine ? 0.018f : 0.012f,
                       lineColor);
        }
    }

    void PendulumBackgroundRenderer::drawAxes(float extent) const
    {
        DrawLineEx(PendulumCoordinatesAdapter::toRenderPosition(-extent, 0.0f),
                   PendulumCoordinatesAdapter::toRenderPosition(extent, 0.0f),
                   0.028f,
                   AXIS_X_COLOR);
        DrawLineEx(PendulumCoordinatesAdapter::toRenderPosition(0.0f, -extent),
                   PendulumCoordinatesAdapter::toRenderPosition(0.0f, extent),
                   0.028f,
                   AXIS_Y_COLOR);
    }

    void PendulumBackgroundRenderer::drawPivot() const
    {
        DrawCircleV(PendulumCoordinatesAdapter::toRenderPosition(0.0f, 0.0f), 0.05f, colors::White);
        DrawCircleLinesV(PendulumCoordinatesAdapter::toRenderPosition(0.0f, 0.0f), 0.08f, colors::LightGray);
    }
} // namespace physim
