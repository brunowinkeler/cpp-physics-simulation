#include "Projectile.h"
#include "core/GlobalDefinitions.h"

#include <algorithm>
#include <cmath>

namespace physim
{
    namespace
    {
        using Scalar = double;

        constexpr Scalar MIN_EFFECTIVE_MASS = 0.001;
        constexpr int LANDING_ROOT_ITERATIONS = 48;

        struct Acceleration
        {
            Scalar ax;
            Scalar ay;
        };

        Acceleration computeDragAcceleration(Scalar vx, Scalar vy, const Environment &env,
                                             Scalar mass, Scalar dragCoefficient, Scalar radius)
        {
            if (!env.airResistanceEnabled || env.airDensity <= 0.0f || dragCoefficient <= 0.0 || radius <= 0.0)
            {
                return {0.0, 0.0};
            }

            const Scalar speedSquared = (vx * vx) + (vy * vy);
            if (speedSquared <= 0.0)
            {
                return {0.0, 0.0};
            }

            const Scalar speed = std::sqrt(speedSquared);
            const Scalar effectiveMass = std::max(mass, MIN_EFFECTIVE_MASS);
            const Scalar crossSectionArea = static_cast<Scalar>(constants::PI) * radius * radius;
            const Scalar dragScale = (0.5 * static_cast<Scalar>(env.airDensity) * dragCoefficient * crossSectionArea) / effectiveMass;

            return {
                -dragScale * speed * vx,
                -dragScale * speed * vy};
        }

        Scalar evaluateCubicHermite(Scalar startValue, Scalar startTangent,
                                    Scalar endValue, Scalar endTangent,
                                    Scalar fraction)
        {
            const Scalar fractionSquared = fraction * fraction;
            const Scalar fractionCubed = fractionSquared * fraction;

            return ((2.0 * fractionCubed) - (3.0 * fractionSquared) + 1.0) * startValue +
                   (fractionCubed - (2.0 * fractionSquared) + fraction) * startTangent +
                   ((-2.0 * fractionCubed) + (3.0 * fractionSquared)) * endValue +
                   (fractionCubed - fractionSquared) * endTangent;
        }

        Scalar evaluateCubicHermiteDerivative(Scalar startValue, Scalar startTangent,
                                              Scalar endValue, Scalar endTangent,
                                              Scalar fraction)
        {
            const Scalar fractionSquared = fraction * fraction;

            return ((6.0 * fractionSquared) - (6.0 * fraction)) * startValue +
                   ((3.0 * fractionSquared) - (4.0 * fraction) + 1.0) * startTangent +
                   ((-6.0 * fractionSquared) + (6.0 * fraction)) * endValue +
                   ((3.0 * fractionSquared) - (2.0 * fraction)) * endTangent;
        }

        State interpolateState(const State &startState, const State &endState,
                               Scalar timeStep, Scalar fraction)
        {
            const Scalar startTangentX = timeStep * startState.vx;
            const Scalar startTangentY = timeStep * startState.vy;
            const Scalar endTangentX = timeStep * endState.vx;
            const Scalar endTangentY = timeStep * endState.vy;

            return {
                evaluateCubicHermite(startState.x, startTangentX, endState.x, endTangentX, fraction),
                evaluateCubicHermite(startState.y, startTangentY, endState.y, endTangentY, fraction),
                evaluateCubicHermiteDerivative(startState.x, startTangentX, endState.x, endTangentX, fraction) / timeStep,
                evaluateCubicHermiteDerivative(startState.y, startTangentY, endState.y, endTangentY, fraction) / timeStep};
        }

        Scalar findLandingFraction(const State &startState, const State &endState, Scalar timeStep)
        {
            if (startState.y <= 0.0)
            {
                return 0.0;
            }

            Scalar low = 0.0;
            Scalar high = 1.0;
            for (int iteration = 0; iteration < LANDING_ROOT_ITERATIONS; ++iteration)
            {
                const Scalar mid = (low + high) * 0.5;
                const Scalar y = interpolateState(startState, endState, timeStep, mid).y;
                if (y > 0.0)
                {
                    low = mid;
                }
                else
                {
                    high = mid;
                }
            }

            return (low + high) * 0.5;
        }
    }

    Projectile::Projectile() = default;

    float Projectile::getCurrentSpeed() const
    {
        return static_cast<float>(std::hypot(state.vx, state.vy));
    }

    void Projectile::update(float timeStep, const Environment &env)
    {
        if (landed || timeStep <= 0.0f)
        {
            lastUpdateDuration = 0.0;
            return;
        }

        const Scalar effectiveTimeStep = static_cast<Scalar>(timeStep);
        const State previousState = state;
        lastUpdateDuration = effectiveTimeStep;

        switch (integrationMethod)
        {
        case IntegrationMethod::SymplecticEuler:
            updateSymplecticEuler(effectiveTimeStep, env);
            break;
        case IntegrationMethod::RungeKutta4:
        default:
            updateRK4(effectiveTimeStep, env);
            break;
        }

        if (previousState.y >= 0.0 && state.y < 0.0)
        {
            lastUpdateDuration = resolveLanding(effectiveTimeStep, previousState);
        }
    }

    void Projectile::launch()
    {
        const Scalar radianAngle = static_cast<Scalar>(launchAngle) * static_cast<Scalar>(constants::DEG_TO_RAD);
        state.vx = static_cast<Scalar>(initialSpeed) * std::cos(radianAngle);
        state.vy = static_cast<Scalar>(initialSpeed) * std::sin(radianAngle);
        lastUpdateDuration = 0.0;
        launched = true;
        landed = false;
    }

    void Projectile::reset()
    {
        state = {0.0, 0.0, 0.0, 0.0};
        lastUpdateDuration = 0.0;
        launched = false;
        landed = false;
    }

    Derivative Projectile::evaluate(const State &state, const Environment &env)
    {
        Scalar ax = 0.0;
        Scalar ay = -static_cast<Scalar>(env.gravity);

        const Acceleration dragAcceleration = computeDragAcceleration(
            state.vx,
            state.vy,
            env,
            static_cast<Scalar>(mass),
            static_cast<Scalar>(dragCoefficient),
            static_cast<Scalar>(radius));
        ax += dragAcceleration.ax;
        ay += dragAcceleration.ay;

        return {state.vx, state.vy, ax, ay};
    }

    void Projectile::updateSymplecticEuler(double timeStep, const Environment &env)
    {
        Scalar ax = 0.0;
        Scalar ay = -static_cast<Scalar>(env.gravity);

        const Acceleration dragAcceleration = computeDragAcceleration(
            state.vx,
            state.vy,
            env,
            static_cast<Scalar>(mass),
            static_cast<Scalar>(dragCoefficient),
            static_cast<Scalar>(radius));
        ax += dragAcceleration.ax;
        ay += dragAcceleration.ay;

        state.vx += ax * timeStep;
        state.vy += ay * timeStep;

        state.x += state.vx * timeStep;
        state.y += state.vy * timeStep;
    }

    void Projectile::updateRK4(double timeStep, const Environment &env)
    {
        const State current = state;

        const Derivative k1 = evaluate(current, env);

        const State s2 = {current.x + k1.dx * timeStep * 0.5,
                          current.y + k1.dy * timeStep * 0.5,
                          current.vx + k1.dvx * timeStep * 0.5,
                          current.vy + k1.dvy * timeStep * 0.5};
        const Derivative k2 = evaluate(s2, env);

        const State s3 = {current.x + k2.dx * timeStep * 0.5,
                          current.y + k2.dy * timeStep * 0.5,
                          current.vx + k2.dvx * timeStep * 0.5,
                          current.vy + k2.dvy * timeStep * 0.5};
        const Derivative k3 = evaluate(s3, env);

        const State s4 = {current.x + k3.dx * timeStep,
                          current.y + k3.dy * timeStep,
                          current.vx + k3.dvx * timeStep,
                          current.vy + k3.dvy * timeStep};
        const Derivative k4 = evaluate(s4, env);

        state.x += (timeStep / 6.0) * (k1.dx + (2.0 * k2.dx) + (2.0 * k3.dx) + k4.dx);
        state.y += (timeStep / 6.0) * (k1.dy + (2.0 * k2.dy) + (2.0 * k3.dy) + k4.dy);
        state.vx += (timeStep / 6.0) * (k1.dvx + (2.0 * k2.dvx) + (2.0 * k3.dvx) + k4.dvx);
        state.vy += (timeStep / 6.0) * (k1.dvy + (2.0 * k2.dvy) + (2.0 * k3.dvy) + k4.dvy);
    }

    double Projectile::resolveLanding(double timeStep, const State &previousState)
    {
        const Scalar landingFraction = findLandingFraction(previousState, state, timeStep);
        const State landingState = interpolateState(previousState, state, timeStep, landingFraction);

        state.x = landingState.x;
        state.y = 0.0;
        state.vx = 0.0;
        state.vy = 0.0;
        landed = true;

        return timeStep * landingFraction;
    }
} // namespace physim
