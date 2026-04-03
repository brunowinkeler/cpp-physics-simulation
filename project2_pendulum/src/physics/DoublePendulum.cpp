#include "DoublePendulum.h"

#include "core/GlobalDefinitions.h"

#include <algorithm>
#include <cmath>

namespace physim
{
    namespace
    {
        constexpr double MIN_PENDULUM_LENGTH = 0.05;
        constexpr double MIN_PENDULUM_MASS = 0.001;
        constexpr double MIN_DENOMINATOR = 1.0e-8;

        double sanitizeDenominator(double value)
        {
            if (std::abs(value) >= MIN_DENOMINATOR)
            {
                return value;
            }

            return value < 0.0 ? -MIN_DENOMINATOR : MIN_DENOMINATOR;
        }
    }

    DoublePendulum::DoublePendulum()
    {
        reset();
    }

    float DoublePendulum::getAngle1Degrees() const
    {
        return static_cast<float>(state.theta1 * static_cast<double>(constants::RAD_TO_DEG));
    }

    float DoublePendulum::getAngle2Degrees() const
    {
        return static_cast<float>(state.theta2 * static_cast<double>(constants::RAD_TO_DEG));
    }

    float DoublePendulum::getAngularVelocity1() const
    {
        return static_cast<float>(state.omega1);
    }

    float DoublePendulum::getAngularVelocity2() const
    {
        return static_cast<float>(state.omega2);
    }

    void DoublePendulum::setIntegrationMethod(IntegrationMethod method)
    {
        integrationMethod = isSupportedIntegrationMethod(method) ? method : DEFAULT_INTEGRATION_METHOD;
    }

    void DoublePendulum::reset()
    {
        state.theta1 = static_cast<double>(initialAngle1Degrees) * static_cast<double>(constants::DEG_TO_RAD);
        state.theta2 = static_cast<double>(initialAngle2Degrees) * static_cast<double>(constants::DEG_TO_RAD);
        state.omega1 = static_cast<double>(initialAngularVelocity1);
        state.omega2 = static_cast<double>(initialAngularVelocity2);
    }

    void DoublePendulum::update(float timeStep, const PendulumEnvironment &environment)
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

    DoublePendulumBobs DoublePendulum::getBobs() const
    {
        const double effectiveLength1 = std::max(static_cast<double>(length1), MIN_PENDULUM_LENGTH);
        const double effectiveLength2 = std::max(static_cast<double>(length2), MIN_PENDULUM_LENGTH);

        const double x1 = effectiveLength1 * std::sin(state.theta1);
        const double y1 = -effectiveLength1 * std::cos(state.theta1);

        const double x2 = x1 + (effectiveLength2 * std::sin(state.theta2));
        const double y2 = y1 - (effectiveLength2 * std::cos(state.theta2));

        const double vx1 = effectiveLength1 * state.omega1 * std::cos(state.theta1);
        const double vy1 = effectiveLength1 * state.omega1 * std::sin(state.theta1);

        const double vx2 = vx1 + (effectiveLength2 * state.omega2 * std::cos(state.theta2));
        const double vy2 = vy1 + (effectiveLength2 * state.omega2 * std::sin(state.theta2));

        return {{
                    static_cast<float>(x1),
                    static_cast<float>(y1),
                    static_cast<float>(std::hypot(vx1, vy1)),
                },
                {
                    static_cast<float>(x2),
                    static_cast<float>(y2),
                    static_cast<float>(std::hypot(vx2, vy2)),
                }};
    }

    float DoublePendulum::getTotalEnergy(const PendulumEnvironment &environment) const
    {
        const double effectiveLength1 = std::max(static_cast<double>(length1), MIN_PENDULUM_LENGTH);
        const double effectiveLength2 = std::max(static_cast<double>(length2), MIN_PENDULUM_LENGTH);
        const double effectiveMass1 = std::max(static_cast<double>(mass1), MIN_PENDULUM_MASS);
        const double effectiveMass2 = std::max(static_cast<double>(mass2), MIN_PENDULUM_MASS);

        const double vx1 = effectiveLength1 * state.omega1 * std::cos(state.theta1);
        const double vy1 = effectiveLength1 * state.omega1 * std::sin(state.theta1);

        const double vx2 = vx1 + (effectiveLength2 * state.omega2 * std::cos(state.theta2));
        const double vy2 = vy1 + (effectiveLength2 * state.omega2 * std::sin(state.theta2));

        const double kinetic = (0.5 * effectiveMass1 * ((vx1 * vx1) + (vy1 * vy1))) +
                               (0.5 * effectiveMass2 * ((vx2 * vx2) + (vy2 * vy2)));

        const double potential = (effectiveMass1 * static_cast<double>(environment.gravity) * effectiveLength1 * (1.0 - std::cos(state.theta1))) +
                                 (effectiveMass2 * static_cast<double>(environment.gravity) *
                                  ((effectiveLength1 * (1.0 - std::cos(state.theta1))) +
                                   (effectiveLength2 * (1.0 - std::cos(state.theta2)))));

        return static_cast<float>(kinetic + potential);
    }

    DoublePendulum::Derivative DoublePendulum::evaluate(const DoublePendulumState &currentState, const PendulumEnvironment &environment) const
    {
        return {
            currentState.omega1,
            computeAngularAcceleration1(currentState, environment),
            currentState.omega2,
            computeAngularAcceleration2(currentState, environment),
        };
    }

    double DoublePendulum::computeAngularAcceleration1(const DoublePendulumState &currentState, const PendulumEnvironment &environment) const
    {
        const double effectiveLength1 = std::max(static_cast<double>(length1), MIN_PENDULUM_LENGTH);
        const double effectiveLength2 = std::max(static_cast<double>(length2), MIN_PENDULUM_LENGTH);
        const double effectiveMass1 = std::max(static_cast<double>(mass1), MIN_PENDULUM_MASS);
        const double effectiveMass2 = std::max(static_cast<double>(mass2), MIN_PENDULUM_MASS);
        const double delta = currentState.theta1 - currentState.theta2;
        const double denominator = sanitizeDenominator(
            effectiveLength1 * ((2.0 * effectiveMass1) + effectiveMass2 - (effectiveMass2 * std::cos(2.0 * delta))));

        const double numerator =
            (-static_cast<double>(environment.gravity) * ((2.0 * effectiveMass1) + effectiveMass2) * std::sin(currentState.theta1)) -
            (effectiveMass2 * static_cast<double>(environment.gravity) * std::sin(currentState.theta1 - (2.0 * currentState.theta2))) -
            (2.0 * std::sin(delta) * effectiveMass2 *
             ((currentState.omega2 * currentState.omega2 * effectiveLength2) +
              (currentState.omega1 * currentState.omega1 * effectiveLength1 * std::cos(delta))));

        return (numerator / denominator) - (static_cast<double>(damping1) * currentState.omega1);
    }

    double DoublePendulum::computeAngularAcceleration2(const DoublePendulumState &currentState, const PendulumEnvironment &environment) const
    {
        const double effectiveLength1 = std::max(static_cast<double>(length1), MIN_PENDULUM_LENGTH);
        const double effectiveLength2 = std::max(static_cast<double>(length2), MIN_PENDULUM_LENGTH);
        const double effectiveMass1 = std::max(static_cast<double>(mass1), MIN_PENDULUM_MASS);
        const double effectiveMass2 = std::max(static_cast<double>(mass2), MIN_PENDULUM_MASS);
        const double delta = currentState.theta1 - currentState.theta2;
        const double denominator = sanitizeDenominator(
            effectiveLength2 * ((2.0 * effectiveMass1) + effectiveMass2 - (effectiveMass2 * std::cos(2.0 * delta))));

        const double numerator = 2.0 * std::sin(delta) *
                                 ((currentState.omega1 * currentState.omega1 * effectiveLength1 * (effectiveMass1 + effectiveMass2)) +
                                  (static_cast<double>(environment.gravity) * (effectiveMass1 + effectiveMass2) * std::cos(currentState.theta1)) +
                                  (currentState.omega2 * currentState.omega2 * effectiveLength2 * effectiveMass2 * std::cos(delta)));

        return (numerator / denominator) - (static_cast<double>(damping2) * currentState.omega2);
    }

    void DoublePendulum::updateSymplecticEuler(double timeStep, const PendulumEnvironment &environment)
    {
        const double alpha1 = computeAngularAcceleration1(state, environment);
        const double alpha2 = computeAngularAcceleration2(state, environment);

        state.omega1 += alpha1 * timeStep;
        state.omega2 += alpha2 * timeStep;
        state.theta1 += state.omega1 * timeStep;
        state.theta2 += state.omega2 * timeStep;
    }

    void DoublePendulum::updateRungeKutta4(double timeStep, const PendulumEnvironment &environment)
    {
        const DoublePendulumState currentState = state;

        const Derivative k1 = evaluate(currentState, environment);

        const DoublePendulumState state2{
            currentState.theta1 + (0.5 * timeStep * k1.dTheta1),
            currentState.omega1 + (0.5 * timeStep * k1.dOmega1),
            currentState.theta2 + (0.5 * timeStep * k1.dTheta2),
            currentState.omega2 + (0.5 * timeStep * k1.dOmega2),
        };
        const Derivative k2 = evaluate(state2, environment);

        const DoublePendulumState state3{
            currentState.theta1 + (0.5 * timeStep * k2.dTheta1),
            currentState.omega1 + (0.5 * timeStep * k2.dOmega1),
            currentState.theta2 + (0.5 * timeStep * k2.dTheta2),
            currentState.omega2 + (0.5 * timeStep * k2.dOmega2),
        };
        const Derivative k3 = evaluate(state3, environment);

        const DoublePendulumState state4{
            currentState.theta1 + (timeStep * k3.dTheta1),
            currentState.omega1 + (timeStep * k3.dOmega1),
            currentState.theta2 + (timeStep * k3.dTheta2),
            currentState.omega2 + (timeStep * k3.dOmega2),
        };
        const Derivative k4 = evaluate(state4, environment);

        state.theta1 += (timeStep / 6.0) * (k1.dTheta1 + (2.0 * k2.dTheta1) + (2.0 * k3.dTheta1) + k4.dTheta1);
        state.omega1 += (timeStep / 6.0) * (k1.dOmega1 + (2.0 * k2.dOmega1) + (2.0 * k3.dOmega1) + k4.dOmega1);
        state.theta2 += (timeStep / 6.0) * (k1.dTheta2 + (2.0 * k2.dTheta2) + (2.0 * k3.dTheta2) + k4.dTheta2);
        state.omega2 += (timeStep / 6.0) * (k1.dOmega2 + (2.0 * k2.dOmega2) + (2.0 * k3.dOmega2) + k4.dOmega2);
    }
} // namespace physim
