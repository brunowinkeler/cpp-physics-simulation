#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "Environment.h"

namespace physim
{
    struct Position
    {
        double x;
        double y;
    };

    class Projectile
    {
    public:
        Projectile() = default;
        Projectile(float velocity, float angle);

        void update(float timeStep, const Environment &env);
        void reset();

        float &getVelocity() { return velocity; }
        float &getAngle() { return angle; }
        Position &getPosition() { return position; }
        float &getMass() { return mass; }

        void setVelocity(const float v) { velocity = v; }
        void setAngle(const float a) { angle = a; }
        void setMass(const float m) { mass = m; }
        void setPosition(const Position &pos) { position = pos; }

    private:
        float velocity{0.0f};
        float angle{0.0f};
        Position position{0.0, 0.0};
        float mass{1.0f};
    };
}

#endif // PROJECTILE_H
