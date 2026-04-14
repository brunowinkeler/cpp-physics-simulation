#include "GasCollision.h"

#include <algorithm>
#include <cmath>

namespace physim
{
    namespace
    {
        constexpr float MIN_MASS = 1.0e-30f;
        constexpr float MIN_DISTANCE = 1.0e-6f;
    }

    bool resolveWallCollision(GasParticle &particle, const GasBoxBounds &boxBounds, float &wallImpulseAccumulator)
    {
        bool collided = false;

        if (particle.position.x - particle.radius < 0.0f)
        {
            particle.position.x = particle.radius;
            if (particle.velocity.x < 0.0f)
            {
                wallImpulseAccumulator += 2.0f * particle.mass * std::abs(particle.velocity.x);
                particle.velocity.x = -particle.velocity.x;
            }
            collided = true;
        }
        else if (particle.position.x + particle.radius > boxBounds.width)
        {
            particle.position.x = boxBounds.width - particle.radius;
            if (particle.velocity.x > 0.0f)
            {
                wallImpulseAccumulator += 2.0f * particle.mass * std::abs(particle.velocity.x);
                particle.velocity.x = -particle.velocity.x;
            }
            collided = true;
        }

        if (particle.position.y - particle.radius < 0.0f)
        {
            particle.position.y = particle.radius;
            if (particle.velocity.y < 0.0f)
            {
                wallImpulseAccumulator += 2.0f * particle.mass * std::abs(particle.velocity.y);
                particle.velocity.y = -particle.velocity.y;
            }
            collided = true;
        }
        else if (particle.position.y + particle.radius > boxBounds.height)
        {
            particle.position.y = boxBounds.height - particle.radius;
            if (particle.velocity.y > 0.0f)
            {
                wallImpulseAccumulator += 2.0f * particle.mass * std::abs(particle.velocity.y);
                particle.velocity.y = -particle.velocity.y;
            }
            collided = true;
        }

        return collided;
    }

    bool resolveParticleCollision(GasParticle &first, GasParticle &second)
    {
        const GasVector2 delta = second.position - first.position;
        const float minSeparation = first.radius + second.radius;
        const float distanceSquared = gasLengthSquared(delta);
        if (distanceSquared > (minSeparation * minSeparation))
        {
            return false;
        }

        const float distance = std::sqrt(std::max(distanceSquared, MIN_DISTANCE));
        const GasVector2 normal = distance > MIN_DISTANCE ? (delta / distance) : GasVector2{1.0f, 0.0f};

        const float firstMass = std::max(first.mass, MIN_MASS);
        const float secondMass = std::max(second.mass, MIN_MASS);
        const float firstInverseMass = 1.0f / firstMass;
        const float secondInverseMass = 1.0f / secondMass;
        const float totalInverseMass = firstInverseMass + secondInverseMass;

        const float overlap = std::max(minSeparation - distance, 0.0f);
        if (overlap > 0.0f)
        {
            const GasVector2 correction = normal * (overlap / std::max(totalInverseMass, MIN_DISTANCE));
            first.position -= correction * firstInverseMass;
            second.position += correction * secondInverseMass;
        }

        const GasVector2 relativeVelocity = second.velocity - first.velocity;
        const float normalVelocity = gasDot(relativeVelocity, normal);
        if (normalVelocity >= 0.0f)
        {
            return true;
        }

        const float impulseMagnitude = -(2.0f * normalVelocity) / totalInverseMass;
        const GasVector2 impulse = normal * impulseMagnitude;
        first.velocity -= impulse * firstInverseMass;
        second.velocity += impulse * secondInverseMass;
        return true;
    }
} // namespace physim
