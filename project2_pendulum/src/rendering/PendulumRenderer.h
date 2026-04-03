#ifndef PENDULUM_RENDERER_H
#define PENDULUM_RENDERER_H

namespace physim
{
    class PendulumSession;

    class PendulumRenderer
    {
    public:
        explicit PendulumRenderer(const PendulumSession &session)
            : session{session}
        {
        }

        void render() const;

    private:
        void renderSimplePendulum() const;
        void renderDoublePendulum() const;

        const PendulumSession &session;
    };
} // namespace physim

#endif // PENDULUM_RENDERER_H
