#include "project1_balistica/src/physics/Projectile.h"
#include "project1_balistica/src/simulation/Simulation.h"

#include <cmath>
#include <iostream>
#include <limits>
#include <numbers>

namespace
{
    constexpr float GRAVITY = 9.81f;
    constexpr float TIME_LIMIT = 60.0f;

    bool expectNear(double actual, double expected, double tolerance, const char *label)
    {
        if (std::abs(actual - expected) <= tolerance)
        {
            return true;
        }

        std::cerr << label << " failed: expected " << expected << ", got " << actual
                  << " (tolerance " << tolerance << ")\n";
        return false;
    }

    bool expectTrue(bool condition, const char *label, double value)
    {
        if (condition)
        {
            return true;
        }

        std::cerr << label << " failed: got " << value << "\n";
        return false;
    }

    double simulateRange(physim::Projectile projectile, physim::Environment environment,
                         float timeStep)
    {
        projectile.launch();

        float elapsedTime = 0.0f;
        while (!projectile.isLanded() && elapsedTime < TIME_LIMIT)
        {
            projectile.update(timeStep, environment);
            elapsedTime += timeStep;
        }

        if (!projectile.isLanded())
        {
            return std::numeric_limits<double>::quiet_NaN();
        }

        return static_cast<double>(projectile.getPosition().x);
    }

    bool testVacuumRangeUsesInterpolatedImpact()
    {
        physim::Projectile projectile;
        physim::Environment environment;

        environment.gravity = GRAVITY;
        environment.airResistanceEnabled = false;
        projectile.setIntegrationMethod(physim::IntegrationMethod::RungeKutta4);
        projectile.getInitialSpeed() = 50.0f;
        projectile.getLaunchAngle() = 45.0f;

        const double launchAngleRadians = 45.0 * std::numbers::pi_v<double> / 180.0;
        const double expectedRange = (50.0 * 50.0 * std::sin(2.0 * launchAngleRadians)) / GRAVITY;

        const double coarseRange = simulateRange(projectile, environment, 1.0f);
        const double mediumRange = simulateRange(projectile, environment, 0.1f);
        const double fineRange = simulateRange(projectile, environment, 0.001f);

        return expectNear(coarseRange, expectedRange, 0.02, "Vacuum RK4 range with dt=1.0") &&
               expectNear(mediumRange, expectedRange, 0.02, "Vacuum RK4 range with dt=0.1") &&
               expectNear(fineRange, expectedRange, 0.02, "Vacuum RK4 range with dt=0.001");
    }

    bool testDefaultDragRangeStaysStableAcrossTimeSteps()
    {
        physim::Projectile projectile;
        physim::Environment environment;

        environment.gravity = GRAVITY;
        environment.airResistanceEnabled = true;
        projectile.setIntegrationMethod(physim::IntegrationMethod::RungeKutta4);

        const double coarseRange = simulateRange(projectile, environment, 0.01f);
        const double fineRange = simulateRange(projectile, environment, 0.001f);

        return expectNear(coarseRange, fineRange, 0.25, "Default drag range stability") &&
               expectTrue(fineRange > 25.0, "Default drag range should remain well above a few meters", fineRange);
    }

    bool testSimulationTimeStopsAtExactLanding()
    {
        physim::Projectile projectile;
        physim::Environment environment;
        physim::Simulation simulation{environment, projectile};

        environment.gravity = GRAVITY;
        environment.airResistanceEnabled = false;
        projectile.setIntegrationMethod(physim::IntegrationMethod::RungeKutta4);
        projectile.getInitialSpeed() = 50.0f;
        projectile.getLaunchAngle() = 45.0f;
        simulation.setPhysicsTimeStep(1.0f);
        simulation.start();

        for (int step = 0; step < 16 && !projectile.isLanded(); ++step)
        {
            simulation.update(1.0f);
        }

        const double expectedFlightTime = (2.0 * 50.0 * std::sin(45.0 * std::numbers::pi_v<double> / 180.0)) / GRAVITY;
        const double landingTime = simulation.getTimeGlobal();

        simulation.update(1.0f);

        return expectTrue(projectile.isLanded(), "Projectile should have landed during simulation time test", landingTime) &&
               expectNear(landingTime, expectedFlightTime, 0.02, "Simulation landing time") &&
               expectNear(simulation.getTimeGlobal(), landingTime, 0.0001, "Simulation time should stop after landing");
    }
}

int main()
{
    bool success = true;

    success = testVacuumRangeUsesInterpolatedImpact() && success;
    success = testDefaultDragRangeStaysStableAcrossTimeSteps() && success;
    success = testSimulationTimeStopsAtExactLanding() && success;

    if (!success)
    {
        return 1;
    }

    std::cout << "ProjectilePhysicsTests passed\n";
    return 0;
}
