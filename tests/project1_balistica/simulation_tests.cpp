#include "test_support.h"

#include "gtest/gtest.h"

#include <cmath>
#include <numbers>

namespace testsupport = physim::testsupport;

TEST(Project1SimulationTest, TimeStopsAtExactLanding)
{
    physim::Projectile projectile;
    physim::Environment environment;
    physim::Simulation simulation{environment, projectile};

    environment.gravity = testsupport::GRAVITY;
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

    const double expectedFlightTime = (2.0 * 50.0 * std::sin(45.0 * std::numbers::pi_v<double> / 180.0)) / testsupport::GRAVITY;
    const double landingTime = simulation.getTimeGlobal();

    simulation.update(1.0f);

    EXPECT_TRUE(projectile.isLanded()) << "Projectile should have landed during simulation time test";
    EXPECT_NEAR(landingTime, expectedFlightTime, 0.02) << "Simulation landing time";
    EXPECT_NEAR(simulation.getTimeGlobal(), landingTime, 0.0001) << "Simulation time should stop after landing";
}

TEST(Project1SimulationTest, StateTransitions)
{
    physim::Projectile projectile;
    physim::Environment environment;
    physim::Simulation simulation{environment, projectile};

    environment.gravity = testsupport::GRAVITY;
    environment.airResistanceEnabled = false;
    projectile.setIntegrationMethod(physim::IntegrationMethod::RungeKutta4);
    projectile.getInitialSpeed() = 50.0f;
    projectile.getLaunchAngle() = 45.0f;
    simulation.setPhysicsTimeStep(1.0f);

    EXPECT_EQ(simulation.getState(), physim::SimulationState::Idle) << "Simulation should start idle";

    simulation.start();
    EXPECT_EQ(simulation.getState(), physim::SimulationState::Running) << "Simulation should enter running state after start";

    simulation.stop();
    EXPECT_EQ(simulation.getState(), physim::SimulationState::Paused) << "Simulation should enter paused state after stop";

    simulation.start();
    EXPECT_EQ(simulation.getState(), physim::SimulationState::Running) << "Simulation should resume from paused state";

    testsupport::runSimulationUntilStopped(simulation, 16, 1.0f);
    EXPECT_EQ(simulation.getState(), physim::SimulationState::Landed) << "Simulation should enter landed state after impact";

    simulation.reset();
    EXPECT_EQ(simulation.getState(), physim::SimulationState::Idle) << "Reset should return simulation to idle state";
}

TEST(Project1SimulationTest, AccessorsExposeConstViews)
{
    physim::Projectile projectile;
    physim::Environment environment;
    physim::Simulation simulation{environment, projectile};

    EXPECT_EQ(&simulation.getProjectile(), &projectile)
        << "Simulation projectile accessor should expose the original projectile";
    EXPECT_EQ(&simulation.getEnvironment(), &environment)
        << "Simulation environment accessor should expose the original environment";
}

TEST(Project1SimulationTest, ApexPointReportsExpectedHeightAndTime)
{
    physim::Projectile projectile;
    physim::Environment environment;
    physim::Simulation simulation{environment, projectile};

    environment.gravity = testsupport::GRAVITY;
    environment.airResistanceEnabled = false;
    projectile.setIntegrationMethod(physim::IntegrationMethod::RungeKutta4);
    projectile.getInitialSpeed() = 50.0f;
    projectile.getLaunchAngle() = 45.0f;
    simulation.setPhysicsTimeStep(1.0f);
    simulation.start();

    for (int step = 0; step < 8 && !simulation.getApexPoint().has_value(); ++step)
    {
        simulation.update(1.0f);
    }

    const std::optional<physim::TrajectoryPoint> apexPoint = simulation.getApexPoint();
    ASSERT_TRUE(apexPoint.has_value()) << "Apex point was not detected";

    const double expectedApexTime = (50.0 * std::sin(45.0 * std::numbers::pi_v<double> / 180.0)) / testsupport::GRAVITY;
    const double expectedApexHeight = (50.0 * 50.0 * 0.5) / (2.0 * testsupport::GRAVITY);
    const double expectedApexX = 50.0 * std::cos(45.0 * std::numbers::pi_v<double> / 180.0) * expectedApexTime;

    EXPECT_NEAR(apexPoint->time, expectedApexTime, 0.02) << "Apex time";
    EXPECT_NEAR(apexPoint->y, expectedApexHeight, 0.05) << "Apex height";
    EXPECT_NEAR(apexPoint->x, expectedApexX, 0.05) << "Apex horizontal position";
}

TEST(Project1SimulationTest, ResetArchivesTrajectoryHistoryAndDeduplicates)
{
    physim::Projectile projectile;
    physim::Environment environment;
    physim::Simulation simulation{environment, projectile};

    environment.gravity = testsupport::GRAVITY;
    environment.airResistanceEnabled = false;
    projectile.setIntegrationMethod(physim::IntegrationMethod::RungeKutta4);
    projectile.getInitialSpeed() = 50.0f;
    projectile.getLaunchAngle() = 45.0f;
    simulation.setPhysicsTimeStep(1.0f);

    simulation.start();
    testsupport::runSimulationUntilStopped(simulation, 16, 1.0f);
    simulation.reset();

    const auto &firstHistory = simulation.getLaunchHistory();
    ASSERT_EQ(firstHistory.size(), 1u) << "First reset should archive one launch";
    EXPECT_TRUE(simulation.getTrajectoryPoints().empty()) << "Current trajectory should be cleared after reset";
    EXPECT_TRUE(firstHistory.front().landed) << "Archived launch should be marked as landed";
    EXPECT_EQ(firstHistory.front().configuration.simulation.integrationMethod, physim::IntegrationMethod::RungeKutta4)
        << "Archived launch should preserve the selected integration method";

    simulation.start();
    testsupport::runSimulationUntilStopped(simulation, 16, 1.0f);
    simulation.reset();

    const auto &duplicateHistory = simulation.getLaunchHistory();
    EXPECT_EQ(duplicateHistory.size(), 1u) << "Duplicate configuration should not be archived twice";

    projectile.getLaunchAngle() = 35.0f;
    simulation.start();
    testsupport::runSimulationUntilStopped(simulation, 16, 1.0f);
    simulation.reset();

    const auto &secondUniqueHistory = simulation.getLaunchHistory();
    ASSERT_EQ(secondUniqueHistory.size(), 2u) << "Unique configuration was not archived";

    const physim::TrajectoryStyle &firstStyle = secondUniqueHistory[0].style;
    const physim::TrajectoryStyle &secondStyle = secondUniqueHistory[1].style;
    const bool stylesDiffer = (firstStyle.r != secondStyle.r) ||
                              (firstStyle.g != secondStyle.g) ||
                              (firstStyle.b != secondStyle.b) ||
                              (firstStyle.a != secondStyle.a);

    const int removedLaunchId = secondUniqueHistory.front().id;
    const int remainingLaunchId = secondUniqueHistory.back().id;
    simulation.removeLaunchHistoryEntry(removedLaunchId);

    const auto &historyAfterSingleRemoval = simulation.getLaunchHistory();
    ASSERT_EQ(historyAfterSingleRemoval.size(), 1u)
        << "Deleting one archived launch should keep the others";
    EXPECT_EQ(historyAfterSingleRemoval.front().id, remainingLaunchId)
        << "Wrong launch remained after single deletion";

    simulation.clearLaunchHistory();
    EXPECT_TRUE(simulation.getLaunchHistory().empty()) << "Clear history should remove all archived launches";
    EXPECT_TRUE(stylesDiffer) << "Unique launches should receive different trajectory colors";
}
