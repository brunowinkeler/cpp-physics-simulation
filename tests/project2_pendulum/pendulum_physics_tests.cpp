#include "test_support.h"

#include "project2_pendulum/src/physics/DoublePendulum.h"

#include "gtest/gtest.h"

#include <cmath>
#include <numbers>

namespace testsupport = physim::testsupport;

TEST(SimplePendulumTest, ResetRestoresConfiguredInitialConditions)
{
    physim::SimplePendulum pendulum;
    pendulum.getInitialAngleDegrees() = 35.0f;
    pendulum.getInitialAngularVelocity() = 1.25f;

    pendulum.reset();

    EXPECT_NEAR(pendulum.getAngleDegrees(), 35.0, 1.0e-5);
    EXPECT_NEAR(pendulum.getAngularVelocity(), 1.25, 1.0e-6);
}

TEST(SimplePendulumTest, SmallAnglePeriodMatchesTheory)
{
    physim::PendulumEnvironment environment;
    physim::SimplePendulum pendulum;

    environment.gravity = 9.81f;
    pendulum.getLength() = 2.0f;
    pendulum.getDamping() = 0.0f;
    pendulum.getInitialAngleDegrees() = 5.0f;
    pendulum.getInitialAngularVelocity() = 0.0f;
    pendulum.setIntegrationMethod(physim::IntegrationMethod::RungeKutta4);

    const double expectedPeriod = 2.0 * std::numbers::pi_v<double> * std::sqrt(pendulum.getLength() / environment.gravity);
    const double measuredPeriod = testsupport::estimateSimplePendulumPeriod(pendulum, environment, 0.001f, 20000);

    ASSERT_TRUE(std::isfinite(measuredPeriod)) << "Failed to estimate the simple pendulum period";
    EXPECT_NEAR(measuredPeriod, expectedPeriod, 0.05);
}

TEST(SimplePendulumTest, DampingReducesEnergy)
{
    physim::PendulumEnvironment environment;
    physim::SimplePendulum pendulum;

    pendulum.getDamping() = 0.2f;
    pendulum.getInitialAngleDegrees() = 40.0f;
    pendulum.getInitialAngularVelocity() = 0.0f;
    pendulum.setIntegrationMethod(physim::IntegrationMethod::RungeKutta4);
    pendulum.reset();

    const double initialEnergy = pendulum.getTotalEnergy(environment);
    for (int step = 0; step < 4000; ++step)
    {
        pendulum.update(0.001f, environment);
    }

    EXPECT_LT(pendulum.getTotalEnergy(environment), initialEnergy);
}

TEST(DoublePendulumTest, ZeroAnglesRemainStable)
{
    physim::PendulumEnvironment environment;
    physim::DoublePendulum pendulum;

    pendulum.getInitialAngle1Degrees() = 0.0f;
    pendulum.getInitialAngle2Degrees() = 0.0f;
    pendulum.getInitialAngularVelocity1() = 0.0f;
    pendulum.getInitialAngularVelocity2() = 0.0f;
    pendulum.getDamping1() = 0.0f;
    pendulum.getDamping2() = 0.0f;
    pendulum.setIntegrationMethod(physim::IntegrationMethod::RungeKutta4);
    pendulum.reset();

    for (int step = 0; step < 1000; ++step)
    {
        pendulum.update(0.001f, environment);
    }

    EXPECT_NEAR(pendulum.getAngle1Degrees(), 0.0, 1.0e-5);
    EXPECT_NEAR(pendulum.getAngle2Degrees(), 0.0, 1.0e-5);
    EXPECT_NEAR(pendulum.getAngularVelocity1(), 0.0, 1.0e-6);
    EXPECT_NEAR(pendulum.getAngularVelocity2(), 0.0, 1.0e-6);
}

TEST(DoublePendulumTest, StateRemainsFiniteAfterManySteps)
{
    physim::PendulumEnvironment environment;
    physim::DoublePendulum pendulum;

    pendulum.setIntegrationMethod(physim::IntegrationMethod::RungeKutta4);
    pendulum.reset();

    for (int step = 0; step < 20000; ++step)
    {
        pendulum.update(0.001f, environment);
    }

    const physim::DoublePendulumState &state = pendulum.getState();

    EXPECT_TRUE(std::isfinite(state.theta1));
    EXPECT_TRUE(std::isfinite(state.theta2));
    EXPECT_TRUE(std::isfinite(state.omega1));
    EXPECT_TRUE(std::isfinite(state.omega2));
    EXPECT_TRUE(std::isfinite(pendulum.getTotalEnergy(environment)));
}
