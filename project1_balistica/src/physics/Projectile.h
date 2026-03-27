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
        float x, y, vx, vy;
    };

    struct Derivative
    {
        float dx, dy, dvx, dvy;
    };

    class Projectile
    {
    public:
        Projectile();

        Position getPosition() const { return position; }
        float getRadius() const { return radius; }
        float getCurrentSpeed() const;

        float &getMass() { return mass; }
        float &getInitialSpeed() { return initialSpeed; }
        float &getLaunchAngle() { return launchAngle; }

        void launch();
        bool isLaunched() const { return launched; }
        bool isLanded() const { return landed; }

        void update(float timeStep, const Environment &env);
        void reset();

    private:
        Derivative evaluate(const State &state, const Environment &env);
        void updateRK4(float timeStep, const Environment &env);
        void updateSymplecticEuler(float timeStep, const Environment &env);

        Position position{0.0f, 0.0f};
        Velocity velocityVector{0.0f, 0.0f};

        float mass{1.0f};             // kg
        float dragCoefficient{0.47f}; // dimensionless, for a sphere
        float radius{1.0f};           // meters

        float initialSpeed{50.0f}; // m/s
        float launchAngle{45.0f};  // degrees

        bool launched{false};
        bool landed{false};
    };
}

#endif // PROJECTILE_H
