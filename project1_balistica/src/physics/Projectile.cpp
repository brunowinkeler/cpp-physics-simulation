#include "Projectile.h"
#include "core/GlobalDefinitions.h"

#include <cmath>

namespace physim
{
    namespace
    {
        constexpr float MIN_EFFECTIVE_MASS = 0.001f;

        Velocity computeDragAcceleration(float vx, float vy, const Environment &env,
                                         float mass, float dragCoefficient, float radius)
        {
            if (!env.airResistanceEnabled || env.airDensity <= 0.0f)
            {
                return {0.0f, 0.0f};
            }

            float speedSquared = (vx * vx) + (vy * vy);
            if (speedSquared <= 0.0f)
            {
                return {0.0f, 0.0f};
            }

            float speed = std::sqrt(speedSquared);
            float effectiveMass = std::fmax(mass, MIN_EFFECTIVE_MASS);
            float crossSectionArea = constants::PI * radius * radius;
            float dragScale = (0.5f * env.airDensity * dragCoefficient * crossSectionArea) / effectiveMass;

            return {
                -dragScale * speed * vx,
                -dragScale * speed * vy};
        }
    }

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
            Position previousPosition = position;

            switch (integrationMethod)
            {
            case IntegrationMethod::SymplecticEuler:
                updateSymplecticEuler(timeStep, env);
                break;
            case IntegrationMethod::RungeKutta4:
            default:
                updateRK4(timeStep, env);
                break;
            }

            if (previousPosition.y >= 0.0f && position.y < 0.0f)
            {
                float fraction = previousPosition.y / (previousPosition.y - position.y);
                position.x = previousPosition.x + ((position.x - previousPosition.x) * fraction);
                position.y = 0.0f;
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

    Derivative Projectile::evaluate(const State &state, const Environment &env)
    {
        float ax = 0.0f;
        float ay = -env.gravity;

        Velocity dragAcceleration = computeDragAcceleration(state.vx, state.vy, env, mass, dragCoefficient, radius);
        ax += dragAcceleration.vx;
        ay += dragAcceleration.vy;

        return {state.vx, state.vy, ax, ay};
    }

    void Projectile::updateSymplecticEuler(float timeStep, const Environment &env)
    {
        float ax = 0.0f;
        float ay = -env.gravity;

        Velocity dragAcceleration = computeDragAcceleration(velocityVector.vx, velocityVector.vy, env, mass, dragCoefficient, radius);
        ax += dragAcceleration.vx;
        ay += dragAcceleration.vy;

        velocityVector.vx += ax * timeStep;
        velocityVector.vy += ay * timeStep;

        position.x += velocityVector.vx * timeStep;
        position.y += velocityVector.vy * timeStep;
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
