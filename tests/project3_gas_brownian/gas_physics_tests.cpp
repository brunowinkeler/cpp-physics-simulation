#include "project3_gas_brownian/src/physics/GasCollision.h"

#include "gtest/gtest.h"

#include <cmath>

TEST(GasPhysicsTest, WallCollisionReflectsVelocityAndAccumulatesImpulse)
{
    physim::GasParticle particle{
        .position = {0.02f, 1.0f},
        .velocity = {-3.0f, 0.5f},
        .mass = 2.0f,
        .radius = 0.05f,
        .isTracer = false,
    };
    const physim::GasBoxBounds boxBounds{4.0f, 3.0f};
    float wallImpulse = 0.0f;

    const bool collided = physim::resolveWallCollision(particle, boxBounds, wallImpulse);

    EXPECT_TRUE(collided);
    EXPECT_NEAR(particle.position.x, particle.radius, 1.0e-6f);
    EXPECT_NEAR(particle.velocity.x, 3.0f, 1.0e-6f);
    EXPECT_NEAR(particle.velocity.y, 0.5f, 1.0e-6f);
    EXPECT_NEAR(wallImpulse, 12.0f, 1.0e-6f);
}

TEST(GasPhysicsTest, EqualMassHeadOnCollisionSwapsVelocities)
{
    physim::GasParticle first{
        .position = {1.0f, 1.0f},
        .velocity = {1.0f, 0.0f},
        .mass = 1.0f,
        .radius = 0.1f,
        .isTracer = false,
    };
    physim::GasParticle second{
        .position = {1.19f, 1.0f},
        .velocity = {-1.0f, 0.0f},
        .mass = 1.0f,
        .radius = 0.1f,
        .isTracer = false,
    };

    const bool collided = physim::resolveParticleCollision(first, second);

    EXPECT_TRUE(collided);
    EXPECT_NEAR(first.velocity.x, -1.0f, 1.0e-5f);
    EXPECT_NEAR(second.velocity.x, 1.0f, 1.0e-5f);
    EXPECT_NEAR(first.velocity.y, 0.0f, 1.0e-6f);
    EXPECT_NEAR(second.velocity.y, 0.0f, 1.0e-6f);
}

TEST(GasPhysicsTest, UnequalMassCollisionConservesMomentumAndEnergy)
{
    physim::GasParticle first{
        .position = {1.0f, 1.0f},
        .velocity = {2.0f, 0.0f},
        .mass = 2.0f,
        .radius = 0.1f,
        .isTracer = false,
    };
    physim::GasParticle second{
        .position = {1.19f, 1.0f},
        .velocity = {-1.0f, 0.0f},
        .mass = 1.0f,
        .radius = 0.1f,
        .isTracer = false,
    };

    const float initialMomentum = (first.mass * first.velocity.x) + (second.mass * second.velocity.x);
    const float initialEnergy = (0.5f * first.mass * first.velocity.x * first.velocity.x) +
                                (0.5f * second.mass * second.velocity.x * second.velocity.x);

    const bool collided = physim::resolveParticleCollision(first, second);
    const float finalMomentum = (first.mass * first.velocity.x) + (second.mass * second.velocity.x);
    const float finalEnergy = (0.5f * first.mass * first.velocity.x * first.velocity.x) +
                              (0.5f * second.mass * second.velocity.x * second.velocity.x);

    EXPECT_TRUE(collided);
    EXPECT_NEAR(finalMomentum, initialMomentum, 1.0e-4f);
    EXPECT_NEAR(finalEnergy, initialEnergy, 1.0e-4f);
}
