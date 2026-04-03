#ifndef SIMPLE_PENDULUM_H
#define SIMPLE_PENDULUM_H

#include "core/physics/IntegrationMethod.h"

#include "PendulumEnvironment.h"
#include "PendulumTypes.h"

namespace physim
{
    struct SimplePendulumState
    {
        double theta;
        double omega;
    };

    class SimplePendulum
    {
    public:
        SimplePendulum();

        float getLength() const { return length; }
        float &getLength() { return length; }
        float getMass() const { return mass; }
        float &getMass() { return mass; }
        float getDamping() const { return damping; }
        float &getDamping() { return damping; }
        float getInitialAngleDegrees() const { return initialAngleDegrees; }
        float &getInitialAngleDegrees() { return initialAngleDegrees; }
        float getInitialAngularVelocity() const { return initialAngularVelocity; }
        float &getInitialAngularVelocity() { return initialAngularVelocity; }
        float getAngleDegrees() const;
        float getAngularVelocity() const;
        IntegrationMethod getIntegrationMethod() const { return integrationMethod; }
        void setIntegrationMethod(IntegrationMethod method);

        void reset();
        void update(float timeStep, const PendulumEnvironment &environment);

        PendulumBob getBob() const;
        float getTotalEnergy(const PendulumEnvironment &environment) const;
        const SimplePendulumState &getState() const { return state; }

    private:
        struct Derivative
        {
            double dTheta;
            double dOmega;
        };

        Derivative evaluate(const SimplePendulumState &currentState, const PendulumEnvironment &environment) const;
        double computeAngularAcceleration(const SimplePendulumState &currentState, const PendulumEnvironment &environment) const;
        void updateSymplecticEuler(double timeStep, const PendulumEnvironment &environment);
        void updateRungeKutta4(double timeStep, const PendulumEnvironment &environment);

        SimplePendulumState state{0.0, 0.0};

        float length{2.0f};
        float mass{1.0f};
        float damping{0.02f};
        float initialAngleDegrees{20.0f};
        float initialAngularVelocity{0.0f};
        IntegrationMethod integrationMethod{DEFAULT_INTEGRATION_METHOD};
    };
} // namespace physim

#endif // SIMPLE_PENDULUM_H
