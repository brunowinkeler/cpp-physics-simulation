#include "TrajectoryRenderer.h"

#include "core/RaylibDefinitions.h"

#include "raylib.h"

#include "ProjectileCoordinatesAdapter.h"

namespace physim
{
    namespace
    {
        constexpr float TRAJECTORY_THICKNESS = 1.75f;
        constexpr Color TRAJECTORY_COLOR{137, 220, 235, 180};
        constexpr Color APEX_COLOR{255, 179, 135, 255};
        constexpr float APEX_MARKER_RADIUS = 3.5f;
        constexpr float APEX_LINE_THICKNESS = 1.5f;
        constexpr float LABEL_OFFSET_X = 8.0f;
        constexpr float LABEL_OFFSET_Y = -20.0f;
        constexpr float FONT_SIZE = 12.0f;
        constexpr float FONT_SPACING = 1.0f;
        constexpr float LABEL_LINE_SPACING = 14.0f;
    }

    void TrajectoryRenderer::render() const
    {
        drawTrajectoryPath();
        drawApexMarker();
    }

    void TrajectoryRenderer::drawTrajectoryPath() const
    {
        const auto &trajectoryPoints = simulation.getTrajectoryPoints();
        if (trajectoryPoints.size() < 2)
        {
            return;
        }

        for (std::size_t index = 1; index < trajectoryPoints.size(); ++index)
        {
            const Position startPoint = ProjectileCoordinatesAdapter::toScreenCoordinates({trajectoryPoints[index - 1].x, trajectoryPoints[index - 1].y});
            const Position endPoint = ProjectileCoordinatesAdapter::toScreenCoordinates({trajectoryPoints[index].x, trajectoryPoints[index].y});

            DrawLineEx({startPoint.x, startPoint.y}, {endPoint.x, endPoint.y}, TRAJECTORY_THICKNESS, TRAJECTORY_COLOR);
        }
    }

    void TrajectoryRenderer::drawApexMarker() const
    {
        const std::optional<TrajectoryPoint> apexPoint = simulation.getApexPoint();
        if (!apexPoint.has_value())
        {
            return;
        }

        const Position apexScreenPosition = ProjectileCoordinatesAdapter::toScreenCoordinates({apexPoint->x, apexPoint->y});
        const Position groundScreenPosition = ProjectileCoordinatesAdapter::toScreenCoordinates({apexPoint->x, 0.0f});

        DrawLineEx({apexScreenPosition.x, apexScreenPosition.y},
                   {groundScreenPosition.x, groundScreenPosition.y},
                   APEX_LINE_THICKNESS,
                   APEX_COLOR);
        DrawCircleLinesV({apexScreenPosition.x, apexScreenPosition.y}, APEX_MARKER_RADIUS, APEX_COLOR);

        const Vector2 labelPosition = {apexScreenPosition.x + LABEL_OFFSET_X, apexScreenPosition.y + LABEL_OFFSET_Y};
        DrawTextEx(GetFontDefault(),
                   TextFormat("h = %.2f m", apexPoint->y),
                   labelPosition,
                   FONT_SIZE,
                   FONT_SPACING,
                   colors::White);
        DrawTextEx(GetFontDefault(),
                   TextFormat("t = %.2f s", apexPoint->time),
                   {labelPosition.x, labelPosition.y + LABEL_LINE_SPACING},
                   FONT_SIZE,
                   FONT_SPACING,
                   colors::White);
    }
} // namespace physim
