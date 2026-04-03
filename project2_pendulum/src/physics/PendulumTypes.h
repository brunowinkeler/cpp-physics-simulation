#ifndef PENDULUM_TYPES_H
#define PENDULUM_TYPES_H

namespace physim
{
    struct PendulumBob
    {
        float x;
        float y;
        float speed;
    };

    struct DoublePendulumBobs
    {
        PendulumBob first;
        PendulumBob second;
    };
} // namespace physim

#endif // PENDULUM_TYPES_H
