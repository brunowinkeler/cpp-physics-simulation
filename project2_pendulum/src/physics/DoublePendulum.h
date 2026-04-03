#ifndef DOUBLE_PENDULUM_H
#define DOUBLE_PENDULUM_H

#include "core/physics/IntegrationMethod.h"

#include "PendulumEnvironment.h"
#include "PendulumTypes.h"

namespace physim
{
    struct DoublePendulumState
    {
        double theta1;
        double omega1;
        double theta2;
        double omega2;
    };

    class DoublePendulum
    {
    public:
        DoublePendulum();

        float getLength1() const { return length1; }
        float &getLength1() { return length1; }
        float getLength2() const { return length2; }
        float &getLength2() { return length2; }
        float getMass1() const { return mass1; }
        float &getMass1() { return mass1; }
        float getMass2() const { return mass2; }
        float &getMass2() { return mass2; }
        float getDamping1() const { return damping1; }
        float &getDamping1() { return damping1; }
        float getDamping2() const { return damping2; }
        float &getDamping2() { return damping2; }
        float getInitialAngle1Degrees() const { return initialAngle1Degrees; }
        float &getInitialAngle1Degrees() { return initialAngle1Degrees; }
        float getInitialAngle2Degrees() const { return initialAngle2Degrees; }
        float &getInitialAngle2Degrees() { return initialAngle2Degrees; }
        float getInitialAngularVelocity1() const { return initialAngularVelocity1; }
        float &getInitialAngularVelocity1() { return initialAngularVelocity1; }
        float getInitialAngularVelocity2() const { return initialAngularVelocity2; }
        float &getInitialAngularVelocity2() { return initialAngularVelocity2; }
        float getAngle1Degrees() const;
        float getAngle2Degrees() const;
        float getAngularVelocity1() const;
        float getAngularVelocity2() const;
        IntegrationMethod getIntegrationMethod() const { return integrationMethod; }
        void setIntegrationMethod(IntegrationMethod method);

        void reset();
        void update(float timeStep, const PendulumEnvironment &environment);

        DoublePendulumBobs getBobs() const;
        float getTotalEnergy(const PendulumEnvironment &environment) const;
        const DoublePendulumState &getState() const { return state; }

    private:
        struct Derivative
        {
            double dTheta1;
            double dOmega1;
            double dTheta2;
            double dOmega2;
        };

        Derivative evaluate(const DoublePendulumState &currentState, const PendulumEnvironment &environment) const;
        double computeAngularAcceleration1(const DoublePendulumState &currentState, const PendulumEnvironment &environment) const;
        double computeAngularAcceleration2(const DoublePendulumState &currentState, const PendulumEnvironment &environment) const;
        void updateSymplecticEuler(double timeStep, const PendulumEnvironment &environment);
        void updateRungeKutta4(double timeStep, const PendulumEnvironment &environment);

        DoublePendulumState state{0.0, 0.0, 0.0, 0.0};

        float length1{1.6f};
        float length2{1.4f};
        float mass1{1.0f};
        float mass2{1.0f};
        float damping1{0.0015f};
        float damping2{0.0015f};
        float initialAngle1Degrees{120.0f};
        float initialAngle2Degrees{-15.0f};
        float initialAngularVelocity1{0.0f};
        float initialAngularVelocity2{0.0f};
        IntegrationMethod integrationMethod{DEFAULT_INTEGRATION_METHOD};
    };
} // namespace physim

#endif // DOUBLE_PENDULUM_H
