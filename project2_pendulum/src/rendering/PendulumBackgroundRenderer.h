#ifndef PENDULUM_BACKGROUND_RENDERER_H
#define PENDULUM_BACKGROUND_RENDERER_H

namespace physim
{
    class PendulumSession;

    class PendulumBackgroundRenderer
    {
    public:
        explicit PendulumBackgroundRenderer(const PendulumSession &session)
            : session{session}
        {
        }

        void render() const;

    private:
        float resolveSceneExtent() const;
        void drawGrid(float extent) const;
        void drawAxes(float extent) const;
        void drawPivot() const;

        const PendulumSession &session;
    };
} // namespace physim

#endif // PENDULUM_BACKGROUND_RENDERER_H
