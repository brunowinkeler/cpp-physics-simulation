#include "PendulumRenderer.h"

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
        constexpr Color PRIMARY_ROD_COLOR{180, 190, 254, 255};
        constexpr Color SECONDARY_ROD_COLOR{249, 226, 175, 255};
        constexpr Color PRIMARY_BOB_COLOR{137, 220, 235, 255};
        constexpr Color SECONDARY_BOB_COLOR{255, 179, 135, 255};
        constexpr float ROD_THICKNESS = 0.045f;
        constexpr float BOB_MIN_RADIUS = 0.08f;
        constexpr float BOB_MAX_RADIUS = 0.18f;

        float bobRadiusFromMass(float mass)
        {
            return std::clamp(0.07f + (0.03f * std::sqrt(std::max(mass, 0.05f))), BOB_MIN_RADIUS, BOB_MAX_RADIUS);
        }
    }

    void PendulumRenderer::render() const
    {
        if (session.getSimulation().isDoubleMode())
        {
            renderDoublePendulum();
            return;
        }

        renderSimplePendulum();
    }

    void PendulumRenderer::renderSimplePendulum() const
    {
        const PendulumBob bob = session.getSimplePendulum().getBob();
        const Vector2 pivot = PendulumCoordinatesAdapter::toRenderPosition(0.0f, 0.0f);
        const Vector2 bobPosition = PendulumCoordinatesAdapter::toRenderPosition(bob.x, bob.y);

        DrawLineEx(pivot, bobPosition, ROD_THICKNESS, PRIMARY_ROD_COLOR);
        DrawCircleV(bobPosition, bobRadiusFromMass(session.getSimplePendulum().getMass()), PRIMARY_BOB_COLOR);
        DrawCircleLinesV(bobPosition, bobRadiusFromMass(session.getSimplePendulum().getMass()), colors::White);
    }

    void PendulumRenderer::renderDoublePendulum() const
    {
        const DoublePendulumBobs bobs = session.getDoublePendulum().getBobs();
        const Vector2 pivot = PendulumCoordinatesAdapter::toRenderPosition(0.0f, 0.0f);
        const Vector2 firstBobPosition = PendulumCoordinatesAdapter::toRenderPosition(bobs.first.x, bobs.first.y);
        const Vector2 secondBobPosition = PendulumCoordinatesAdapter::toRenderPosition(bobs.second.x, bobs.second.y);

        DrawLineEx(pivot, firstBobPosition, ROD_THICKNESS, PRIMARY_ROD_COLOR);
        DrawLineEx(firstBobPosition, secondBobPosition, ROD_THICKNESS, SECONDARY_ROD_COLOR);

        const float firstRadius = bobRadiusFromMass(session.getDoublePendulum().getMass1());
        const float secondRadius = bobRadiusFromMass(session.getDoublePendulum().getMass2());

        DrawCircleV(firstBobPosition, firstRadius, PRIMARY_BOB_COLOR);
        DrawCircleLinesV(firstBobPosition, firstRadius, colors::White);

        DrawCircleV(secondBobPosition, secondRadius, SECONDARY_BOB_COLOR);
        DrawCircleLinesV(secondBobPosition, secondRadius, colors::White);
    }
} // namespace physim
