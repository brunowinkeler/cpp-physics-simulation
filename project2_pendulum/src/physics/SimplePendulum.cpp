#include "SimplePendulum.h"

#include "core/GlobalDefinitions.h"

#include <algorithm>
#include <cmath>

namespace physim
{
    namespace
    {
        constexpr double MIN_PENDULUM_LENGTH = 0.05;
        constexpr double MIN_PENDULUM_MASS = 0.001;
    }

    SimplePendulum::SimplePendulum()
    {
        reset();
    }

    float SimplePendulum::getAngleDegrees() const
    {
        return static_cast<float>(state.theta * static_cast<double>(constants::RAD_TO_DEG));
    }

    float SimplePendulum::getAngularVelocity() const
    {
        return static_cast<float>(state.omega);
    }

    void SimplePendulum::setIntegrationMethod(IntegrationMethod method)
    {
        integrationMethod = isSupportedIntegrationMethod(method) ? method : DEFAULT_INTEGRATION_METHOD;
    }

    void SimplePendulum::reset()
    {
        state.theta = static_cast<double>(initialAngleDegrees) * static_cast<double>(constants::DEG_TO_RAD);
        state.omega = static_cast<double>(initialAngularVelocity);
    }

    void SimplePendulum::update(float timeStep, const PendulumEnvironment &environment)
    {
        if (timeStep <= 0.0f)
        {
            return;
        }

        const double effectiveTimeStep = static_cast<double>(timeStep);
        switch (integrationMethod)
        {
        case IntegrationMethod::SymplecticEuler:
            updateSymplecticEuler(effectiveTimeStep, environment);
            break;
        case IntegrationMethod::RungeKutta4:
        default:
            updateRungeKutta4(effectiveTimeStep, environment);
            break;
        }
    }

    PendulumBob SimplePendulum::getBob() const
    {
        const double effectiveLength = std::max(static_cast<double>(length), MIN_PENDULUM_LENGTH);
        const double x = effectiveLength * std::sin(state.theta);
        const double y = -effectiveLength * std::cos(state.theta);
        const double speed = std::abs(state.omega) * effectiveLength;

        return {
            static_cast<float>(x),
            static_cast<float>(y),
            static_cast<float>(speed),
        };
    }

    float SimplePendulum::getTotalEnergy(const PendulumEnvironment &environment) const
    {
        const double effectiveLength = std::max(static_cast<double>(length), MIN_PENDULUM_LENGTH);
        const double effectiveMass = std::max(static_cast<double>(mass), MIN_PENDULUM_MASS);
        const double kinetic = 0.5 * effectiveMass * effectiveLength * effectiveLength * state.omega * state.omega;
        const double potential = effectiveMass * static_cast<double>(environment.gravity) * effectiveLength * (1.0 - std::cos(state.theta));
        return static_cast<float>(kinetic + potential);
    }

    SimplePendulum::Derivative SimplePendulum::evaluate(const SimplePendulumState &currentState, const PendulumEnvironment &environment) const
    {
        return {currentState.omega, computeAngularAcceleration(currentState, environment)};
    }

    double SimplePendulum::computeAngularAcceleration(const SimplePendulumState &currentState, const PendulumEnvironment &environment) const
    {
        const double effectiveLength = std::max(static_cast<double>(length), MIN_PENDULUM_LENGTH);
        return -((static_cast<double>(environment.gravity) / effectiveLength) * std::sin(currentState.theta)) -
               (static_cast<double>(damping) * currentState.omega);
    }

    void SimplePendulum::updateSymplecticEuler(double timeStep, const PendulumEnvironment &environment)
    {
        state.omega += computeAngularAcceleration(state, environment) * timeStep;
        state.theta += state.omega * timeStep;
    }

    void SimplePendulum::updateRungeKutta4(double timeStep, const PendulumEnvironment &environment)
    {
        const SimplePendulumState currentState = state;

        const Derivative k1 = evaluate(currentState, environment);

        const SimplePendulumState state2{
            currentState.theta + (0.5 * timeStep * k1.dTheta),
            currentState.omega + (0.5 * timeStep * k1.dOmega),
        };
        const Derivative k2 = evaluate(state2, environment);

        const SimplePendulumState state3{
            currentState.theta + (0.5 * timeStep * k2.dTheta),
            currentState.omega + (0.5 * timeStep * k2.dOmega),
        };
        const Derivative k3 = evaluate(state3, environment);

        const SimplePendulumState state4{
            currentState.theta + (timeStep * k3.dTheta),
            currentState.omega + (timeStep * k3.dOmega),
        };
        const Derivative k4 = evaluate(state4, environment);

        state.theta += (timeStep / 6.0) * (k1.dTheta + (2.0 * k2.dTheta) + (2.0 * k3.dTheta) + k4.dTheta);
        state.omega += (timeStep / 6.0) * (k1.dOmega + (2.0 * k2.dOmega) + (2.0 * k3.dOmega) + k4.dOmega);
    }
} // namespace physim
