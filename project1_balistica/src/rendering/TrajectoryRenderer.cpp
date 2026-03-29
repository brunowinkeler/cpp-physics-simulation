#include "TrajectoryRenderer.h"

#include "core/RaylibDefinitions.h"

#include "imgui.h"
#include "raylib.h"

#include <algorithm>
#include <limits>

#include "ProjectileCoordinatesAdapter.h"

namespace physim
{
    namespace
    {
        constexpr int CURRENT_TRAJECTORY_ID = -1;
        constexpr int NO_TRAJECTORY_ID = 0;
        constexpr float HOVER_DISTANCE_TOLERANCE = 8.0f;
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

        Vector2 resolveApexLabelPosition(const Vector2 &apexRenderPosition, int labelLane)
        {
            const float labelOffsetX = BASE_LABEL_OFFSET_X + (LABEL_OFFSET_X_SPACING * static_cast<float>(labelLane % 3));
            const float labelOffsetY = BASE_LABEL_OFFSET_Y - (LABEL_OFFSET_Y_SPACING * static_cast<float>(labelLane % 4));

            return {apexRenderPosition.x + labelOffsetX, apexRenderPosition.y + labelOffsetY};
        }

        float distancePointToSegmentSquared(Vector2 point, Vector2 segmentStart, Vector2 segmentEnd)
        {
            const Vector2 segment = {segmentEnd.x - segmentStart.x, segmentEnd.y - segmentStart.y};
            const Vector2 startToPoint = {point.x - segmentStart.x, point.y - segmentStart.y};
            const float segmentLengthSquared = (segment.x * segment.x) + (segment.y * segment.y);

            if (segmentLengthSquared <= 0.0f)
            {
                return (startToPoint.x * startToPoint.x) + (startToPoint.y * startToPoint.y);
            }

            const float projection = std::clamp(
                ((startToPoint.x * segment.x) + (startToPoint.y * segment.y)) / segmentLengthSquared,
                0.0f,
                1.0f);
            const Vector2 closestPoint = {
                segmentStart.x + (segment.x * projection),
                segmentStart.y + (segment.y * projection),
            };
            const Vector2 pointOffset = {point.x - closestPoint.x, point.y - closestPoint.y};
            return (pointOffset.x * pointOffset.x) + (pointOffset.y * pointOffset.y);
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

    void TrajectoryRenderer::render(const Camera2D &camera2D) const
    {
        const int hoveredTrajectoryId = resolveHoveredTrajectoryId(camera2D);
        renderHistoricalTrajectories(hoveredTrajectoryId);
        renderCurrentTrajectory(hoveredTrajectoryId);
    }

    int TrajectoryRenderer::resolveHoveredTrajectoryId(const Camera2D &camera2D) const
    {
        if (ImGui::GetIO().WantCaptureMouse)
        {
            return NO_TRAJECTORY_ID;
        }

        float closestDistanceSquared = HOVER_DISTANCE_TOLERANCE * HOVER_DISTANCE_TOLERANCE;
        int hoveredTrajectoryId = NO_TRAJECTORY_ID;

        for (const LaunchHistoryEntry &launchHistoryEntry : simulation.getLaunchHistory())
        {
            const float distanceSquared = calculateTrajectoryHoverDistanceSquared(
                launchHistoryEntry.trajectory.getPoints(),
                camera2D);
            if (distanceSquared < closestDistanceSquared)
            {
                closestDistanceSquared = distanceSquared;
                hoveredTrajectoryId = launchHistoryEntry.id;
            }
        }

        const float currentDistanceSquared = calculateTrajectoryHoverDistanceSquared(
            simulation.getTrajectoryPoints(),
            camera2D);
        if (currentDistanceSquared < closestDistanceSquared)
        {
            hoveredTrajectoryId = CURRENT_TRAJECTORY_ID;
        }

        return hoveredTrajectoryId;
    }

    float TrajectoryRenderer::calculateTrajectoryHoverDistanceSquared(const std::vector<TrajectoryPoint> &trajectoryPoints,
                                                                      const Camera2D &camera2D) const
    {
        if (trajectoryPoints.size() < 2)
        {
            return std::numeric_limits<float>::max();
        }

        const Vector2 mousePosition = GetMousePosition();
        float closestDistanceSquared = std::numeric_limits<float>::max();

        for (std::size_t index = 1; index < trajectoryPoints.size(); ++index)
        {
            const Vector2 segmentStart = ProjectileCoordinatesAdapter::toScreenPosition(trajectoryPoints[index - 1].x, trajectoryPoints[index - 1].y, camera2D);
            const Vector2 segmentEnd = ProjectileCoordinatesAdapter::toScreenPosition(trajectoryPoints[index].x, trajectoryPoints[index].y, camera2D);
            closestDistanceSquared = std::min(
                closestDistanceSquared,
                distancePointToSegmentSquared(mousePosition, segmentStart, segmentEnd));
        }

        return closestDistanceSquared;
    }

    void TrajectoryRenderer::renderHistoricalTrajectories(int hoveredTrajectoryId) const
    {
        const auto &launchHistory = simulation.getLaunchHistory();
        for (int pass = 0; pass < 2; ++pass)
        {
            for (std::size_t index = 0; index < launchHistory.size(); ++index)
            {
                const LaunchHistoryEntry &launchHistoryEntry = launchHistory[index];
                const bool isViewportHovered = launchHistoryEntry.id == hoveredTrajectoryId;
                const int emphasisLevel = (isViewportHovered || (launchHistoryEntry.id == hoveredLaunchId)) ? 2 : (launchHistoryEntry.id == selectedLaunchId ? 1 : 0);
                const bool shouldRenderInCurrentPass = (pass == 0) ? (emphasisLevel == 0) : (emphasisLevel > 0);
                if (!shouldRenderInCurrentPass)
                {
                    continue;
                }

                renderTrajectoryRecord(
                    launchHistoryEntry.trajectory.getPoints(),
                    launchHistoryEntry.trajectory.getApexPoint(),
                    launchHistoryEntry.finalRange,
                    launchHistoryEntry.style,
                    static_cast<int>(index),
                    emphasisLevel,
                    isViewportHovered);
            }
        }
    }

    void TrajectoryRenderer::renderCurrentTrajectory(int hoveredTrajectoryId) const
    {
        renderTrajectoryRecord(
            simulation.getTrajectoryPoints(),
            simulation.getApexPoint(),
            projectile.getPosition().x,
            simulation.getCurrentTrajectoryStyle(),
            static_cast<int>(simulation.getLaunchHistory().size()),
            hoveredTrajectoryId == CURRENT_TRAJECTORY_ID ? 2 : 0,
            hoveredTrajectoryId == CURRENT_TRAJECTORY_ID);
    }

    void TrajectoryRenderer::renderTrajectoryRecord(const std::vector<TrajectoryPoint> &trajectoryPoints,
                                                    const std::optional<TrajectoryPoint> &apexPoint,
                                                    float finalRange,
                                                    const TrajectoryStyle &style,
                                                    int labelLane,
                                                    int emphasisLevel,
                                                    bool showLabels) const
    {
        if (trajectoryPoints.empty())
        {
            return;
        }

        drawTrajectoryPath(trajectoryPoints, style, emphasisLevel);
        drawApexMarker(apexPoint, style, labelLane, emphasisLevel, showLabels);
        drawFinalRangeMarker(finalRange, style, labelLane, emphasisLevel, showLabels);
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
            const Vector2 startPoint = ProjectileCoordinatesAdapter::toRenderPosition(trajectoryPoints[index - 1].x, trajectoryPoints[index - 1].y);
            const Vector2 endPoint = ProjectileCoordinatesAdapter::toRenderPosition(trajectoryPoints[index].x, trajectoryPoints[index].y);

            DrawLineEx(startPoint, endPoint, lineThickness, trajectoryColor);
        }
    }

    void TrajectoryRenderer::drawApexMarker(const std::optional<TrajectoryPoint> &apexPoint,
                                            const TrajectoryStyle &style,
                                            int labelLane,
                                            int emphasisLevel,
                                            bool showLabels) const
    {
        if (!apexPoint.has_value())
        {
            return;
        }

        const Vector2 apexRenderPosition = ProjectileCoordinatesAdapter::toRenderPosition(apexPoint->x, apexPoint->y);
        const Vector2 groundRenderPosition = ProjectileCoordinatesAdapter::toRenderPosition(apexPoint->x, 0.0f);
        const Color apexColor = toRaylibColor(style, resolveAlpha(style, emphasisLevel));
        const Color guideColor = toRaylibColor(style, static_cast<unsigned char>(std::min<int>(255, resolveAlpha(style, emphasisLevel) - 40)));

        drawDashedVerticalLine(apexRenderPosition.x,
                               apexRenderPosition.y,
                               groundRenderPosition.y,
                               resolveApexGuideThickness(emphasisLevel),
                               guideColor);
        DrawCircleLinesV(apexRenderPosition, resolveApexMarkerRadius(emphasisLevel), apexColor);

        if (!showLabels)
        {
            return;
        }

        const Vector2 labelPosition = resolveApexLabelPosition(apexRenderPosition, labelLane);
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
                                                  int emphasisLevel,
                                                  bool showLabel) const
    {
        if (finalRange <= 0.0f)
        {
            return;
        }

        const float rangeLineY = resolveRangeLineY(labelLane);
        const Vector2 launchLinePosition = ProjectileCoordinatesAdapter::toRenderPosition(0.0f, rangeLineY);
        const Vector2 landingLinePosition = ProjectileCoordinatesAdapter::toRenderPosition(finalRange, rangeLineY);
        const Vector2 launchGroundPosition = ProjectileCoordinatesAdapter::toRenderPosition(0.0f, 0.0f);
        const Vector2 landingGroundPosition = ProjectileCoordinatesAdapter::toRenderPosition(finalRange, 0.0f);
        const Color rangeColor = toRaylibColor(style, resolveAlpha(style, emphasisLevel));
        const float lineThickness = resolveRangeLineThickness(emphasisLevel);

        DrawLineEx(launchLinePosition,
               landingLinePosition,
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

        if (!showLabel)
        {
            return;
        }

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
