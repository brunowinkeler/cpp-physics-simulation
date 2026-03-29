#include "TrajectoryRenderer.h"

#include "core/RaylibDefinitions.h"

#include "raylib.h"

#include <algorithm>

#include "ProjectileCoordinatesAdapter.h"

namespace physim
{
    namespace
    {
        constexpr float TRAJECTORY_THICKNESS = 1.75f;
        constexpr Color TRAJECTORY_COLOR{137, 220, 235, 180};
        constexpr Color APEX_COLOR{255, 179, 135, 255};
        constexpr Color APEX_GUIDE_COLOR{255, 179, 135, 170};
        constexpr Color RANGE_COLOR{166, 227, 161, 220};
        constexpr float APEX_MARKER_RADIUS = 3.5f;
        constexpr float APEX_LINE_THICKNESS = 1.0f;
        constexpr float APEX_DASH_LENGTH = 6.0f;
        constexpr float APEX_GAP_LENGTH = 4.0f;
        constexpr float RANGE_LINE_Y = -12.0f;
        constexpr float RANGE_LINE_THICKNESS = 1.25f;
        constexpr float RANGE_TICK_HALF_HEIGHT = 3.5f;
        constexpr float LABEL_OFFSET_X = 8.0f;
        constexpr float LABEL_OFFSET_Y = -20.0f;
        constexpr float FONT_SIZE = 12.0f;
        constexpr float FONT_SPACING = 1.0f;
        constexpr float LABEL_LINE_SPACING = 14.0f;
        constexpr float RANGE_LABEL_OFFSET_Y = 18.0f;

        void drawDashedVerticalLine(float x, float startY, float endY, float thickness, Color color)
        {
            const float topY = std::min(startY, endY);
            const float bottomY = std::max(startY, endY);

            for (float currentY = topY; currentY < bottomY; currentY += APEX_DASH_LENGTH + APEX_GAP_LENGTH)
            {
                const float dashEndY = std::min(currentY + APEX_DASH_LENGTH, bottomY);
                DrawLineEx({x, currentY}, {x, dashEndY}, thickness, color);
            }
        }
    }

    void TrajectoryRenderer::render() const
    {
        drawTrajectoryPath();
        drawApexMarker();
        drawFinalRangeMarker();
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

        drawDashedVerticalLine(apexScreenPosition.x,
                               apexScreenPosition.y,
                               groundScreenPosition.y,
                               APEX_LINE_THICKNESS,
                               APEX_GUIDE_COLOR);
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

    void TrajectoryRenderer::drawFinalRangeMarker() const
    {
        if (!projectile.isLanded())
        {
            return;
        }

        const float range = projectile.getPosition().x;
        if (range <= 0.0f)
        {
            return;
        }

        const Position launchLinePosition = ProjectileCoordinatesAdapter::toScreenCoordinates({0.0f, RANGE_LINE_Y});
        const Position landingLinePosition = ProjectileCoordinatesAdapter::toScreenCoordinates({range, RANGE_LINE_Y});
        const Position launchGroundPosition = ProjectileCoordinatesAdapter::toScreenCoordinates({0.0f, 0.0f});
        const Position landingGroundPosition = ProjectileCoordinatesAdapter::toScreenCoordinates({range, 0.0f});

        DrawLineEx({launchLinePosition.x, launchLinePosition.y},
                   {landingLinePosition.x, landingLinePosition.y},
                   RANGE_LINE_THICKNESS,
                   RANGE_COLOR);
        DrawLineEx({launchGroundPosition.x, launchGroundPosition.y - RANGE_TICK_HALF_HEIGHT},
                   {launchGroundPosition.x, launchLinePosition.y + RANGE_TICK_HALF_HEIGHT},
                   RANGE_LINE_THICKNESS,
                   RANGE_COLOR);
        DrawLineEx({landingGroundPosition.x, landingGroundPosition.y - RANGE_TICK_HALF_HEIGHT},
                   {landingGroundPosition.x, landingLinePosition.y + RANGE_TICK_HALF_HEIGHT},
                   RANGE_LINE_THICKNESS,
                   RANGE_COLOR);

        const char *rangeLabel = TextFormat("range = %.2f m", range);
        const Vector2 labelSize = MeasureTextEx(GetFontDefault(), rangeLabel, FONT_SIZE, FONT_SPACING);
        const float labelCenterX = (launchLinePosition.x + landingLinePosition.x) * 0.5f;
        DrawTextEx(GetFontDefault(),
                   rangeLabel,
                   {labelCenterX - (labelSize.x * 0.5f), launchLinePosition.y + RANGE_LABEL_OFFSET_Y},
                   FONT_SIZE,
                   FONT_SPACING,
                   colors::White);
    }
} // namespace physim
