#include "Projectile.h"
#include "core/GlobalDefinitions.h"

#include <cmath>

namespace physim
{
    Projectile::Projectile(float initialSpeed, float launchAngle)
        : initialSpeed{initialSpeed}, launchAngle{launchAngle}, position{0.0, 0.0}, mass{1.0f} {}

    void Projectile::update(float timeStep, const Environment &env)
    {
        if (!launched)
        {
            // Initialize velocity vector based on initial speed and launch angle
            float radianAngle = launchAngle * constants::DEG_TO_RAD;
            velocityVector.vx = initialSpeed * std::cos(radianAngle);
            velocityVector.vy = initialSpeed * std::sin(radianAngle);
            launched = true;
        }

        if (!landed)
        {
            updateSymplecticEuler(timeStep, env);

            // Check for landing
            if (position.y <= 0.0f)
            {
                position.y = 0.0f; // Ensure it doesn't go below ground
                landed = true;
            }
        }
    }

    void Projectile::reset()
    {
        position = {0.0, 0.0};
        velocityVector = {0.0f, 0.0f};
        initialSpeed = 0.0f;
        launchAngle = 0.0f;
        mass = 1.0f;
        launched = false;
        landed = false;
    }

    void Projectile::updateSymplecticEuler(float timeStep, const Environment &env)
    {
        // Calculate acceleration due to gravity
        float ax = 0.0f;
        float ay = -env.gravity;

        // Update velocity
        velocityVector.vx += ax * timeStep;
        velocityVector.vy += ay * timeStep;

        // Update position
        position.x += velocityVector.vx * timeStep;
        position.y += velocityVector.vy * timeStep;
    }
} // namespace physim
