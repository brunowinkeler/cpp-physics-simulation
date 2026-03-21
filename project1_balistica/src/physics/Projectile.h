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

    class Projectile
    {
    public:
        Projectile() = default;
        Projectile(float initialSpeed, float launchAngle);

        Position getPosition() const { return position; }
        float &getRadius() { return radius; }
        float &getMass() { return mass; }
        float &getInitialSpeed() { return initialSpeed; }
        float &getLaunchAngle() { return launchAngle; }

        void update(float timeStep, const Environment &env);
        void reset();

    private:
        void updateSymplecticEuler(float timeStep, const Environment &env);

        Position position{0.0f, 0.0f};
        Velocity velocityVector{0.0f, 0.0f};

        float mass{1.0f};
        float dragCoefficient{0.47f};
        float radius{0.1f};

        float initialSpeed{0.0f};
        float launchAngle{0.0f};

        bool launched{false};
        bool landed{false};
    };
}

#endif // PROJECTILE_H
