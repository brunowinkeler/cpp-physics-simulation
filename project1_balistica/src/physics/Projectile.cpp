#include "Projectile.h"
#include "core/GlobalDefinitions.h"

#include <cmath>

namespace physim
{
    Projectile::Projectile()
        : position{0.0, 0.0}, mass{1.0f}
    {
    }

    float Projectile::getCurrentSpeed() const
    {
        return std::sqrt(velocityVector.vx * velocityVector.vx + velocityVector.vy * velocityVector.vy);
    }

    void Projectile::update(float timeStep, const Environment &env)
    {
        if (!landed)
        {
            updateRK4(timeStep, env);

            if (position.y < 0.0f)
            {
                float prevY = position.y - velocityVector.vy * timeStep; // posição anterior (era > 0)
                float fraction = prevY / (prevY - position.y);
                position.y = 0.0f;
                position.x = (position.x - velocityVector.vx * timeStep) + velocityVector.vx * timeStep * fraction;
                velocityVector = {0.0f, 0.0f};
                landed = true;
            }
        }
    }

    void Projectile::launch()
    {
        float radianAngle = launchAngle * constants::DEG_TO_RAD;
        velocityVector.vx = initialSpeed * std::cos(radianAngle);
        velocityVector.vy = initialSpeed * std::sin(radianAngle);
        launched = true;
    }

    void Projectile::reset()
    {
        position = {0.0, 0.0};
        velocityVector = {0.0f, 0.0f};
        launched = false;
        landed = false;
    }

    void Projectile::updateSymplecticEuler(float timeStep, const Environment &env)
    {
        float ax = 0.0f;
        float ay = -env.gravity;

        velocityVector.vx += ax * timeStep;
        velocityVector.vy += ay * timeStep;

        position.x += velocityVector.vx * timeStep;
        position.y += velocityVector.vy * timeStep;
    }

    Derivative Projectile::evaluate(const State &state, const Environment &env)
    {
        float speed = std::sqrt(state.vx * state.vx + state.vy * state.vy);

        float ax = 0.0f;
        float ay = -env.gravity;

        return {state.vx, state.vy, ax, ay};
    }

    void Projectile::updateRK4(float timeStep, const Environment &env)
    {
        State current = {position.x, position.y, velocityVector.vx, velocityVector.vy};

        Derivative k1 = evaluate(current, env);

        State s2 = {current.x + k1.dx * timeStep * 0.5f,
                    current.y + k1.dy * timeStep * 0.5f,
                    current.vx + k1.dvx * timeStep * 0.5f,
                    current.vy + k1.dvy * timeStep * 0.5f};
        Derivative k2 = evaluate(s2, env);

        State s3 = {current.x + k2.dx * timeStep * 0.5f,
                    current.y + k2.dy * timeStep * 0.5f,
                    current.vx + k2.dvx * timeStep * 0.5f,
                    current.vy + k2.dvy * timeStep * 0.5f};
        Derivative k3 = evaluate(s3, env);

        State s4 = {current.x + k3.dx * timeStep,
                    current.y + k3.dy * timeStep,
                    current.vx + k3.dvx * timeStep,
                    current.vy + k3.dvy * timeStep};
        Derivative k4 = evaluate(s4, env);

        // Média ponderada
        position.x += (timeStep / 6.0f) * (k1.dx + 2 * k2.dx + 2 * k3.dx + k4.dx);
        position.y += (timeStep / 6.0f) * (k1.dy + 2 * k2.dy + 2 * k3.dy + k4.dy);
        velocityVector.vx += (timeStep / 6.0f) * (k1.dvx + 2 * k2.dvx + 2 * k3.dvx + k4.dvx);
        velocityVector.vy += (timeStep / 6.0f) * (k1.dvy + 2 * k2.dvy + 2 * k3.dvy + k4.dvy);
    }
} // namespace physim
