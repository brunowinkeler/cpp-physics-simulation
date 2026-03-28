#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "Environment.h"

namespace physim
{
    struct Position
    {
        float x;
        float y;
    };

    struct Velocity
    {
        float vx;
        float vy;
    };

    struct State
    {
        double x;
        double y;
        double vx;
        double vy;
    };

    struct Derivative
    {
        double dx;
        double dy;
        double dvx;
        double dvy;
    };

    enum class IntegrationMethod
    {
        SymplecticEuler = 0,
        RungeKutta4
    };

    class Projectile
    {
    public:
        Projectile();

        Position getPosition() const
        {
            return {static_cast<float>(state.x), static_cast<float>(state.y)};
        }
        float getRadius() const { return radius; }
        float &getRadius() { return radius; }
        float getDragCoefficient() const { return dragCoefficient; }
        float &getDragCoefficient() { return dragCoefficient; }
        float getCurrentSpeed() const;

        float &getMass() { return mass; }
        float &getInitialSpeed() { return initialSpeed; }
        float &getLaunchAngle() { return launchAngle; }
        IntegrationMethod getIntegrationMethod() const { return integrationMethod; }
        void setIntegrationMethod(IntegrationMethod method) { integrationMethod = method; }

        void launch();
        bool isLaunched() const { return launched; }
        bool isLanded() const { return landed; }

        void update(float timeStep, const Environment &env);
        void reset();

    private:
        Derivative evaluate(const State &state, const Environment &env);
        void updateRK4(double timeStep, const Environment &env);
        void updateSymplecticEuler(double timeStep, const Environment &env);
        void resolveLanding(double timeStep, const State &previousState);

        State state{0.0, 0.0, 0.0, 0.0};

        float mass{0.145f};           // kg
        float dragCoefficient{0.47f}; // dimensionless, for a sphere
        float radius{0.0366f};        // meters

        float initialSpeed{50.0f}; // m/s
        float launchAngle{45.0f};  // degrees

        IntegrationMethod integrationMethod{IntegrationMethod::RungeKutta4};

        bool launched{false};
        bool landed{false};
    };
}

#endif // PROJECTILE_H
