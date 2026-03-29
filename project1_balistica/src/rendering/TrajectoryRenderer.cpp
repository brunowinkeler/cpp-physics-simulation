#include "TrajectoryRenderer.h"

#include "core/RaylibDefinitions.h"

#include "raylib.h"

#include <algorithm>

#include "ProjectileCoordinatesAdapter.h"

namespace physim
{
    namespace
    {
        constexpr float BASE_TRAJECTORY_THICKNESS = 1.75f;
        constexpr float SELECTED_TRAJECTORY_THICKNESS = 2.75f;
        constexpr float HOVERED_TRAJECTORY_THICKNESS = 3.25f;
        constexpr Color APEX_LABEL_COLOR{255, 255, 255, 255};
        constexpr float BASE_APEX_MARKER_RADIUS = 3.5f;
        constexpr float SELECTED_APEX_MARKER_RADIUS = 4.5f;
        constexpr float HOVERED_APEX_MARKER_RADIUS = 5.0f;
        constexpr float BASE_APEX_LINE_THICKNESS = 1.0f;
        constexpr float EMPHASIZED_APEX_LINE_THICKNESS = 1.5f;
        constexpr float APEX_DASH_LENGTH = 6.0f;
        constexpr float APEX_GAP_LENGTH = 4.0f;
        constexpr float BASE_RANGE_LINE_Y = -12.0f;
        constexpr float RANGE_LINE_SPACING = 12.0f;
        constexpr float BASE_RANGE_LINE_THICKNESS = 1.25f;
        constexpr float EMPHASIZED_RANGE_LINE_THICKNESS = 1.75f;
        constexpr float RANGE_TICK_HALF_HEIGHT = 3.5f;
        constexpr float BASE_LABEL_OFFSET_X = 8.0f;
        constexpr float BASE_LABEL_OFFSET_Y = -20.0f;
        constexpr float LABEL_OFFSET_X_SPACING = 20.0f;
        constexpr float LABEL_OFFSET_Y_SPACING = 18.0f;
        constexpr float FONT_SIZE = 12.0f;
        constexpr float FONT_SPACING = 1.0f;
        constexpr float LABEL_LINE_SPACING = 14.0f;
        constexpr float RANGE_LABEL_OFFSET_Y = 18.0f;

        Color toRaylibColor(const TrajectoryStyle &style, unsigned char alpha)
        {
            return {style.r, style.g, style.b, alpha};
        }

        unsigned char resolveAlpha(const TrajectoryStyle &style, int emphasisLevel)
        {
            if (emphasisLevel == 2)
            {
                return 255;
            }

            if (emphasisLevel == 1)
            {
                return static_cast<unsigned char>(std::max<int>(style.a, 240));
            }

            return style.a;
        }

        float resolveTrajectoryThickness(int emphasisLevel)
        {
            if (emphasisLevel == 2)
            {
                return HOVERED_TRAJECTORY_THICKNESS;
            }

            if (emphasisLevel == 1)
            {
                return SELECTED_TRAJECTORY_THICKNESS;
            }

            return BASE_TRAJECTORY_THICKNESS;
        }

        float resolveApexMarkerRadius(int emphasisLevel)
        {
            if (emphasisLevel == 2)
            {
                return HOVERED_APEX_MARKER_RADIUS;
            }

            if (emphasisLevel == 1)
            {
                return SELECTED_APEX_MARKER_RADIUS;
            }

            return BASE_APEX_MARKER_RADIUS;
        }

        float resolveApexGuideThickness(int emphasisLevel)
        {
            return emphasisLevel > 0 ? EMPHASIZED_APEX_LINE_THICKNESS : BASE_APEX_LINE_THICKNESS;
        }

        float resolveRangeLineY(int labelLane)
        {
            return BASE_RANGE_LINE_Y - (RANGE_LINE_SPACING * static_cast<float>(labelLane % 6));
        }

        float resolveRangeLineThickness(int emphasisLevel)
        {
            return emphasisLevel > 0 ? EMPHASIZED_RANGE_LINE_THICKNESS : BASE_RANGE_LINE_THICKNESS;
        }

        Vector2 resolveApexLabelPosition(const Position &apexScreenPosition, int labelLane)
        {
            const float labelOffsetX = BASE_LABEL_OFFSET_X + (LABEL_OFFSET_X_SPACING * static_cast<float>(labelLane % 3));
            const float labelOffsetY = BASE_LABEL_OFFSET_Y - (LABEL_OFFSET_Y_SPACING * static_cast<float>(labelLane % 4));

            return {apexScreenPosition.x + labelOffsetX, apexScreenPosition.y + labelOffsetY};
        }

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
        renderHistoricalTrajectories();
        renderCurrentTrajectory();
    }

    void TrajectoryRenderer::renderHistoricalTrajectories() const
    {
        const auto &launchHistory = simulation.getLaunchHistory();
        for (int pass = 0; pass < 2; ++pass)
        {
            for (std::size_t index = 0; index < launchHistory.size(); ++index)
            {
                const LaunchHistoryEntry &launchHistoryEntry = launchHistory[index];
                const int emphasisLevel = (launchHistoryEntry.id == hoveredLaunchId) ? 2 : (launchHistoryEntry.id == selectedLaunchId ? 1 : 0);
                const bool shouldRenderInCurrentPass = (pass == 0) ? (emphasisLevel == 0) : (emphasisLevel > 0);
                if (!shouldRenderInCurrentPass)
                {
                    continue;
                }

                renderTrajectoryRecord(
                    launchHistoryEntry.trajectory.getPoints(),
                    launchHistoryEntry.trajectory.getApexPoint(),
                    launchHistoryEntry.finalRange,
                    launchHistoryEntry.landed,
                    launchHistoryEntry.style,
                    static_cast<int>(index),
                    emphasisLevel);
            }
        }
    }

    void TrajectoryRenderer::renderCurrentTrajectory() const
    {
        renderTrajectoryRecord(
            simulation.getTrajectoryPoints(),
            simulation.getApexPoint(),
            projectile.getPosition().x,
            projectile.isLanded(),
            simulation.getCurrentTrajectoryStyle(),
            static_cast<int>(simulation.getLaunchHistory().size()),
            0);
    }

    void TrajectoryRenderer::renderTrajectoryRecord(const std::vector<TrajectoryPoint> &trajectoryPoints,
                                                    const std::optional<TrajectoryPoint> &apexPoint,
                                                    float finalRange,
                                                    bool landed,
                                                    const TrajectoryStyle &style,
                                                    int labelLane,
                                                    int emphasisLevel) const
    {
        if (trajectoryPoints.empty())
        {
            return;
        }

        drawTrajectoryPath(trajectoryPoints, style, emphasisLevel);
        drawApexMarker(apexPoint, style, labelLane, emphasisLevel);
        drawFinalRangeMarker(finalRange, style, labelLane, emphasisLevel);

        if (!landed)
        {
            return;
        }
    }

    void TrajectoryRenderer::drawTrajectoryPath(const std::vector<TrajectoryPoint> &trajectoryPoints,
                                                const TrajectoryStyle &style,
                                                int emphasisLevel) const
    {
        if (trajectoryPoints.size() < 2)
        {
            return;
        }

        const Color trajectoryColor = toRaylibColor(style, resolveAlpha(style, emphasisLevel));
        const float lineThickness = resolveTrajectoryThickness(emphasisLevel);

        for (std::size_t index = 1; index < trajectoryPoints.size(); ++index)
        {
            const Position startPoint = ProjectileCoordinatesAdapter::toScreenCoordinates({trajectoryPoints[index - 1].x, trajectoryPoints[index - 1].y});
            const Position endPoint = ProjectileCoordinatesAdapter::toScreenCoordinates({trajectoryPoints[index].x, trajectoryPoints[index].y});

            DrawLineEx({startPoint.x, startPoint.y}, {endPoint.x, endPoint.y}, lineThickness, trajectoryColor);
        }
    }

    void TrajectoryRenderer::drawApexMarker(const std::optional<TrajectoryPoint> &apexPoint,
                                            const TrajectoryStyle &style,
                                            int labelLane,
                                            int emphasisLevel) const
    {
        if (!apexPoint.has_value())
        {
            return;
        }

        const Position apexScreenPosition = ProjectileCoordinatesAdapter::toScreenCoordinates({apexPoint->x, apexPoint->y});
        const Position groundScreenPosition = ProjectileCoordinatesAdapter::toScreenCoordinates({apexPoint->x, 0.0f});
        const Color apexColor = toRaylibColor(style, resolveAlpha(style, emphasisLevel));
        const Color guideColor = toRaylibColor(style, static_cast<unsigned char>(std::min<int>(255, resolveAlpha(style, emphasisLevel) - 40)));

        drawDashedVerticalLine(apexScreenPosition.x,
                               apexScreenPosition.y,
                               groundScreenPosition.y,
                               resolveApexGuideThickness(emphasisLevel),
                               guideColor);
        DrawCircleLinesV({apexScreenPosition.x, apexScreenPosition.y}, resolveApexMarkerRadius(emphasisLevel), apexColor);

        const Vector2 labelPosition = resolveApexLabelPosition(apexScreenPosition, labelLane);
        DrawTextEx(GetFontDefault(),
                   TextFormat("h = %.2f m", apexPoint->y),
                   labelPosition,
                   FONT_SIZE,
                   FONT_SPACING,
                   APEX_LABEL_COLOR);
        DrawTextEx(GetFontDefault(),
                   TextFormat("t = %.2f s", apexPoint->time),
                   {labelPosition.x, labelPosition.y + LABEL_LINE_SPACING},
                   FONT_SIZE,
                   FONT_SPACING,
                   APEX_LABEL_COLOR);
    }

    void TrajectoryRenderer::drawFinalRangeMarker(float finalRange,
                                                  const TrajectoryStyle &style,
                                                  int labelLane,
                                                  int emphasisLevel) const
    {
        if (finalRange <= 0.0f)
        {
            return;
        }

        const float rangeLineY = resolveRangeLineY(labelLane);
        const Position launchLinePosition = ProjectileCoordinatesAdapter::toScreenCoordinates({0.0f, rangeLineY});
        const Position landingLinePosition = ProjectileCoordinatesAdapter::toScreenCoordinates({finalRange, rangeLineY});
        const Position launchGroundPosition = ProjectileCoordinatesAdapter::toScreenCoordinates({0.0f, 0.0f});
        const Position landingGroundPosition = ProjectileCoordinatesAdapter::toScreenCoordinates({finalRange, 0.0f});
        const Color rangeColor = toRaylibColor(style, resolveAlpha(style, emphasisLevel));
        const float lineThickness = resolveRangeLineThickness(emphasisLevel);

        DrawLineEx({launchLinePosition.x, launchLinePosition.y},
                   {landingLinePosition.x, landingLinePosition.y},
                   lineThickness,
                   rangeColor);
        DrawLineEx({launchGroundPosition.x, launchGroundPosition.y - RANGE_TICK_HALF_HEIGHT},
                   {launchGroundPosition.x, launchLinePosition.y + RANGE_TICK_HALF_HEIGHT},
                   lineThickness,
                   rangeColor);
        DrawLineEx({landingGroundPosition.x, landingGroundPosition.y - RANGE_TICK_HALF_HEIGHT},
                   {landingGroundPosition.x, landingLinePosition.y + RANGE_TICK_HALF_HEIGHT},
                   lineThickness,
                   rangeColor);

        const char *rangeLabel = TextFormat("range = %.2f m", finalRange);
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
