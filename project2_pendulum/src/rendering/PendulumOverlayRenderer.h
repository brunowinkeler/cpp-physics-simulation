#ifndef PENDULUM_OVERLAY_RENDERER_H
#define PENDULUM_OVERLAY_RENDERER_H

namespace physim
{
    class PendulumSession;

    class PendulumOverlayRenderer
    {
    public:
        explicit PendulumOverlayRenderer(const PendulumSession &session)
            : session{session}
        {
        }

        void render() const;

    private:
        void renderStatus() const;
        void renderInteractionHint() const;

        const PendulumSession &session;
    };
} // namespace physim

#endif // PENDULUM_OVERLAY_RENDERER_H
