#ifndef PENDULUM_ENVIRONMENT_H
#define PENDULUM_ENVIRONMENT_H

namespace physim
{
    struct PendulumEnvironment
    {
        float gravity = 9.81f;
        float timeScale = 1.0f;
    };
} // namespace physim

#endif // PENDULUM_ENVIRONMENT_H
