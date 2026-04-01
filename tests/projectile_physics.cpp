#include "test_support.h"

#include "gtest/gtest.h"

#include <cmath>
#include <numbers>

namespace testsupport = physim::testsupport;

TEST(ProjectilePhysicsTest, VacuumRangeUsesInterpolatedImpact)
{
    physim::Projectile projectile;
    physim::Environment environment;

    environment.gravity = testsupport::GRAVITY;
    environment.airResistanceEnabled = false;
    projectile.setIntegrationMethod(physim::IntegrationMethod::RungeKutta4);
    projectile.getInitialSpeed() = 50.0f;
    projectile.getLaunchAngle() = 45.0f;

    const double launchAngleRadians = 45.0 * std::numbers::pi_v<double> / 180.0;
    const double expectedRange = (50.0 * 50.0 * std::sin(2.0 * launchAngleRadians)) / testsupport::GRAVITY;

    const double coarseRange = testsupport::simulateRange(projectile, environment, 1.0f);
    const double mediumRange = testsupport::simulateRange(projectile, environment, 0.1f);
    const double fineRange = testsupport::simulateRange(projectile, environment, 0.001f);

    EXPECT_NEAR(coarseRange, expectedRange, 0.02) << "Vacuum RK4 range with dt=1.0";
    EXPECT_NEAR(mediumRange, expectedRange, 0.02) << "Vacuum RK4 range with dt=0.1";
    EXPECT_NEAR(fineRange, expectedRange, 0.02) << "Vacuum RK4 range with dt=0.001";
}

TEST(ProjectilePhysicsTest, DefaultDragRangeStaysStableAcrossTimeSteps)
{
    physim::Projectile projectile;
    physim::Environment environment;

    environment.gravity = testsupport::GRAVITY;
    environment.airResistanceEnabled = true;
    projectile.setIntegrationMethod(physim::IntegrationMethod::RungeKutta4);

    const double coarseRange = testsupport::simulateRange(projectile, environment, 0.01f);
    const double fineRange = testsupport::simulateRange(projectile, environment, 0.001f);

    EXPECT_NEAR(coarseRange, fineRange, 0.25) << "Default drag range stability";
    EXPECT_GT(fineRange, 25.0) << "Default drag range should remain well above a few meters";
}

TEST(ProjectilePhysicsTest, SymplecticEulerRangeRemainsSupported)
{
    physim::Projectile symplecticProjectile;
    physim::Projectile rk4Projectile;
    physim::Environment environment;

    environment.gravity = testsupport::GRAVITY;
    environment.airResistanceEnabled = false;

    symplecticProjectile.setIntegrationMethod(physim::IntegrationMethod::SymplecticEuler);
    symplecticProjectile.getInitialSpeed() = 50.0f;
    symplecticProjectile.getLaunchAngle() = 45.0f;

    rk4Projectile.setIntegrationMethod(physim::IntegrationMethod::RungeKutta4);
    rk4Projectile.getInitialSpeed() = 50.0f;
    rk4Projectile.getLaunchAngle() = 45.0f;

    const double expectedRange = (50.0 * 50.0) / testsupport::GRAVITY;
    const double symplecticRange = testsupport::simulateRange(symplecticProjectile, environment, 0.001f);
    const double rk4Range = testsupport::simulateRange(rk4Projectile, environment, 0.001f);

    EXPECT_NEAR(symplecticRange, expectedRange, 0.35) << "Vacuum Symplectic Euler range";
    EXPECT_NEAR(symplecticRange, rk4Range, 0.1) << "Supported integrators should stay aligned for the same launch";
}
