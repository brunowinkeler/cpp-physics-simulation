#include "GasSceneRenderer.h"

#include "core/RaylibDefinitions.h"
#include "simulation/GasSession.h"

#include <algorithm>

namespace physim
{
    GasSceneRenderer::GasSceneRenderer(const GasSession &session)
        : session{session}
    {
    }

    void GasSceneRenderer::updateCamera(float frameTime)
    {
        camera.setTrackedBox(session.getEnvironment().boxWidth, session.getEnvironment().boxHeight);
        camera.update(frameTime);
    }

    void GasSceneRenderer::resetCamera()
    {
        camera.setTrackedBox(session.getEnvironment().boxWidth, session.getEnvironment().boxHeight);
        camera.reset();
    }

    void GasSceneRenderer::render() const
    {
        camera.setTrackedBox(session.getEnvironment().boxWidth, session.getEnvironment().boxHeight);

        BeginMode2D(camera.getCamera2D());
        if (session.getVisualizationSettings().showGrid)
        {
            drawGrid();
        }
        drawBox();
        drawTracerTrail();
        drawParticles();
        EndMode2D();
    }

    void GasSceneRenderer::drawGrid() const
    {
        const float boxWidth = session.getEnvironment().boxWidth;
        const float boxHeight = session.getEnvironment().boxHeight;

        for (float x = 0.0f; x <= boxWidth; x += 1.0f)
        {
            DrawLineV(toRenderPosition(x, 0.0f), toRenderPosition(x, boxHeight), Fade(colors::DarkGray, 0.35f));
        }
        for (float y = 0.0f; y <= boxHeight; y += 1.0f)
        {
            DrawLineV(toRenderPosition(0.0f, y), toRenderPosition(boxWidth, y), Fade(colors::DarkGray, 0.35f));
        }
    }

    void GasSceneRenderer::drawBox() const
    {
        const float boxWidth = session.getEnvironment().boxWidth;
        const float boxHeight = session.getEnvironment().boxHeight;

        DrawLineEx(toRenderPosition(0.0f, 0.0f), toRenderPosition(boxWidth, 0.0f), 0.03f, colors::LightGray);
        DrawLineEx(toRenderPosition(boxWidth, 0.0f), toRenderPosition(boxWidth, boxHeight), 0.03f, colors::LightGray);
        DrawLineEx(toRenderPosition(boxWidth, boxHeight), toRenderPosition(0.0f, boxHeight), 0.03f, colors::LightGray);
        DrawLineEx(toRenderPosition(0.0f, boxHeight), toRenderPosition(0.0f, 0.0f), 0.03f, colors::LightGray);
    }

    void GasSceneRenderer::drawTracerTrail() const
    {
        const GasSimulation &simulation = session.getSimulation();
        if (!simulation.isTracerTrailEnabled())
        {
            return;
        }

        const std::vector<TrajectoryPoint> &points = simulation.getTracerTrailPoints();
        if (points.size() < 2)
        {
            return;
        }

        for (std::size_t index = 1; index < points.size(); ++index)
        {
            DrawLineEx(toRenderPosition(points[index - 1].x, points[index - 1].y),
                       toRenderPosition(points[index].x, points[index].y),
                       0.02f,
                       Fade(colors::LightPink, 0.8f));
        }
    }

    void GasSceneRenderer::drawParticles() const
    {
        for (const GasParticle &particle : session.getSimulation().getParticles())
        {
            const Color color = particle.isTracer ? colors::Orange : colors::LightBlue;
            DrawCircleV(toRenderPosition(particle.position.x, particle.position.y), particle.radius, color);
        }
    }

    Vector2 GasSceneRenderer::toRenderPosition(float x, float y)
    {
        return {x, -y};
    }
} // namespace physim
