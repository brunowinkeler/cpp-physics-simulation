#include "project1_balistica/src/physics/Projectile.h"
#include "project1_balistica/src/simulation/Simulation.h"

#include <cmath>
#include <cstddef>
#include <iostream>
#include <limits>
#include <numbers>

namespace
{
    constexpr float GRAVITY = 9.81f;
    constexpr float TIME_LIMIT = 60.0f;

    void runSimulationUntilStopped(physim::Simulation &simulation, int maxSteps, float frameStep)
    {
        for (int step = 0; step < maxSteps && simulation.isRunning(); ++step)
        {
            simulation.update(frameStep);
        }
    }

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

    bool expectState(physim::SimulationState actual, physim::SimulationState expected, const char *label)
    {
        return expectTrue(actual == expected, label, static_cast<double>(static_cast<int>(actual)));
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

    bool testSymplecticEulerRangeRemainsSupported()
    {
        physim::Projectile symplecticProjectile;
        physim::Projectile rk4Projectile;
        physim::Environment environment;

        environment.gravity = GRAVITY;
        environment.airResistanceEnabled = false;

        symplecticProjectile.setIntegrationMethod(physim::IntegrationMethod::SymplecticEuler);
        symplecticProjectile.getInitialSpeed() = 50.0f;
        symplecticProjectile.getLaunchAngle() = 45.0f;

        rk4Projectile.setIntegrationMethod(physim::IntegrationMethod::RungeKutta4);
        rk4Projectile.getInitialSpeed() = 50.0f;
        rk4Projectile.getLaunchAngle() = 45.0f;

        const double expectedRange = (50.0 * 50.0) / GRAVITY;
        const double symplecticRange = simulateRange(symplecticProjectile, environment, 0.001f);
        const double rk4Range = simulateRange(rk4Projectile, environment, 0.001f);

        return expectNear(symplecticRange, expectedRange, 0.35, "Vacuum Symplectic Euler range") &&
               expectNear(symplecticRange, rk4Range, 0.1, "Supported integrators should stay aligned for the same launch");
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

    bool testSimulationStateTransitions()
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

        const bool idleStateValid = expectState(simulation.getState(), physim::SimulationState::Idle, "Simulation should start idle");

        simulation.start();
        const bool runningStateValid = expectState(simulation.getState(), physim::SimulationState::Running, "Simulation should enter running state after start");

        simulation.stop();
        const bool pausedStateValid = expectState(simulation.getState(), physim::SimulationState::Paused, "Simulation should enter paused state after stop");

        simulation.start();
        const bool resumedStateValid = expectState(simulation.getState(), physim::SimulationState::Running, "Simulation should resume from paused state");

        runSimulationUntilStopped(simulation, 16, 1.0f);
        const bool landedStateValid = expectState(simulation.getState(), physim::SimulationState::Landed, "Simulation should enter landed state after impact");

        simulation.reset();
        const bool resetStateValid = expectState(simulation.getState(), physim::SimulationState::Idle, "Reset should return simulation to idle state");

        return idleStateValid &&
               runningStateValid &&
               pausedStateValid &&
               resumedStateValid &&
               landedStateValid &&
               resetStateValid;
    }

    bool testSimulationAccessorsExposeConstViews()
    {
        physim::Projectile projectile;
        physim::Environment environment;
        physim::Simulation simulation{environment, projectile};

        return expectTrue(&simulation.getProjectile() == &projectile,
                          "Simulation projectile accessor should expose the original projectile",
                          (&simulation.getProjectile() == &projectile) ? 1.0 : 0.0) &&
               expectTrue(&simulation.getEnvironment() == &environment,
                          "Simulation environment accessor should expose the original environment",
                          (&simulation.getEnvironment() == &environment) ? 1.0 : 0.0);
    }

    bool testTrajectoryRecorderRetentionPolicyCapsGrowth()
    {
        physim::TrajectoryRecorder trajectoryRecorder;
        const physim::TrajectoryRetentionPolicy retentionPolicy = trajectoryRecorder.getRetentionPolicy();
        const std::size_t totalSamples = retentionPolicy.maxRecordedPoints * 8;
        constexpr float sampleTimeStep = 0.001f;

        for (std::size_t index = 0; index < totalSamples; ++index)
        {
            const float time = static_cast<float>(index) * sampleTimeStep;
            const float x = static_cast<float>(index) * 0.25f;
            const float y = std::sin(time) * 20.0f;
            const bool forceSample = index == (totalSamples - 1);
            trajectoryRecorder.record(x, y, time, 10.0f + y, forceSample);
        }

        const auto &points = trajectoryRecorder.getPoints();
        bool strictlyIncreasingTime = true;
        for (std::size_t index = 1; index < points.size(); ++index)
        {
            if (points[index].time <= points[index - 1].time)
            {
                strictlyIncreasingTime = false;
                break;
            }
        }

        const double expectedFinalTime = static_cast<double>(totalSamples - 1) * sampleTimeStep;

        return expectTrue(points.size() <= retentionPolicy.maxRecordedPoints,
                          "Trajectory recorder should stay within its configured point budget",
                          static_cast<double>(points.size())) &&
               expectTrue(points.size() > 2,
                          "Trajectory recorder should keep enough samples to draw a curve",
                          static_cast<double>(points.size())) &&
               expectNear(points.front().time, 0.0, 1.0e-6, "Trajectory recorder should preserve the first sample") &&
               expectNear(points.back().time, expectedFinalTime, 1.0e-6, "Trajectory recorder should preserve the latest sample") &&
               expectTrue(strictlyIncreasingTime,
                          "Trajectory recorder should keep samples ordered after compaction",
                          strictlyIncreasingTime ? 1.0 : 0.0);
    }

    bool testTrajectoryRecorderSamplingAccumulatesAcrossSkippedSteps()
    {
        physim::TrajectoryRecorder trajectoryRecorder;
        const float minRecordedTimeStep = trajectoryRecorder.getRetentionPolicy().minRecordedTimeStep;

        trajectoryRecorder.record(0.0f, 0.0f, 0.0f, 0.0f, true);
        trajectoryRecorder.record(1.0f, 1.0f, minRecordedTimeStep * 0.45f, 1.0f);
        const bool firstSkipValid = expectTrue(trajectoryRecorder.getPoints().size() == 1,
                                               "Trajectory recorder should skip samples below the minimum interval",
                                               static_cast<double>(trajectoryRecorder.getPoints().size()));

        trajectoryRecorder.record(2.0f, 2.0f, minRecordedTimeStep * 1.10f, 2.0f);
        const bool firstAppendValid = expectTrue(trajectoryRecorder.getPoints().size() == 2,
                                                 "Trajectory recorder should append once the minimum interval is reached",
                                                 static_cast<double>(trajectoryRecorder.getPoints().size()));

        trajectoryRecorder.record(3.0f, 3.0f, minRecordedTimeStep * 1.55f, 3.0f);
        const bool secondSkipValid = expectTrue(trajectoryRecorder.getPoints().size() == 2,
                                                "Trajectory recorder should keep the last committed point when skipping",
                                                static_cast<double>(trajectoryRecorder.getPoints().size()));

        trajectoryRecorder.record(4.0f, 4.0f, minRecordedTimeStep * 2.20f, 4.0f);
        const auto &points = trajectoryRecorder.getPoints();

        return firstSkipValid &&
               firstAppendValid &&
               secondSkipValid &&
               expectTrue(points.size() == 3,
                          "Trajectory recorder should accumulate elapsed time across skipped samples",
                          static_cast<double>(points.size())) &&
               expectNear(points.back().time, static_cast<double>(minRecordedTimeStep * 2.20f), 1.0e-6,
                          "Trajectory recorder should append the next eligible sample");
    }

    bool testApexPointReportsExpectedHeightAndTime()
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

        for (int step = 0; step < 8 && !simulation.getApexPoint().has_value(); ++step)
        {
            simulation.update(1.0f);
        }

        const std::optional<physim::TrajectoryPoint> apexPoint = simulation.getApexPoint();
        if (!apexPoint.has_value())
        {
            std::cerr << "Apex point was not detected\n";
            return false;
        }

        const double expectedApexTime = (50.0 * std::sin(45.0 * std::numbers::pi_v<double> / 180.0)) / GRAVITY;
        const double expectedApexHeight = (50.0 * 50.0 * 0.5) / (2.0 * GRAVITY);
        const double expectedApexX = 50.0 * std::cos(45.0 * std::numbers::pi_v<double> / 180.0) * expectedApexTime;

        return expectNear(apexPoint->time, expectedApexTime, 0.02, "Apex time") &&
               expectNear(apexPoint->y, expectedApexHeight, 0.05, "Apex height") &&
               expectNear(apexPoint->x, expectedApexX, 0.05, "Apex horizontal position");
    }

    bool testResetArchivesTrajectoryHistoryAndDeduplicates()
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
        runSimulationUntilStopped(simulation, 16, 1.0f);
        simulation.reset();

        const auto &firstHistory = simulation.getLaunchHistory();
        const bool firstArchiveValid = expectTrue(firstHistory.size() == 1, "First reset should archive one launch", static_cast<double>(firstHistory.size())) &&
                                       expectTrue(simulation.getTrajectoryPoints().empty(), "Current trajectory should be cleared after reset", static_cast<double>(simulation.getTrajectoryPoints().size())) &&
                                       expectTrue(firstHistory.front().landed, "Archived launch should be marked as landed", firstHistory.front().landed ? 1.0 : 0.0) &&
                                       expectTrue(firstHistory.front().configuration.simulation.integrationMethod == physim::IntegrationMethod::RungeKutta4,
                                                  "Archived launch should preserve the selected integration method",
                                                  static_cast<double>(static_cast<int>(firstHistory.front().configuration.simulation.integrationMethod)));

        simulation.start();
        runSimulationUntilStopped(simulation, 16, 1.0f);
        simulation.reset();

        const auto &duplicateHistory = simulation.getLaunchHistory();
        const bool duplicateArchiveValid = expectTrue(duplicateHistory.size() == 1, "Duplicate configuration should not be archived twice", static_cast<double>(duplicateHistory.size()));

        projectile.getLaunchAngle() = 35.0f;
        simulation.start();
        runSimulationUntilStopped(simulation, 16, 1.0f);
        simulation.reset();

        const auto &secondUniqueHistory = simulation.getLaunchHistory();
        if (secondUniqueHistory.size() != 2)
        {
            std::cerr << "Unique configuration was not archived\n";
            return false;
        }

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
        const bool singleRemovalValid = expectTrue(historyAfterSingleRemoval.size() == 1, "Deleting one archived launch should keep the others", static_cast<double>(historyAfterSingleRemoval.size())) &&
                                        expectTrue(historyAfterSingleRemoval.front().id == remainingLaunchId, "Wrong launch remained after single deletion", static_cast<double>(historyAfterSingleRemoval.front().id));

        simulation.clearLaunchHistory();
        const bool clearHistoryValid = expectTrue(simulation.getLaunchHistory().empty(), "Clear history should remove all archived launches", static_cast<double>(simulation.getLaunchHistory().size()));

        return firstArchiveValid &&
               duplicateArchiveValid &&
               expectTrue(stylesDiffer, "Unique launches should receive different trajectory colors", stylesDiffer ? 1.0 : 0.0) &&
               singleRemovalValid &&
               clearHistoryValid;
    }
}

int main()
{
    bool success = true;

    success = testVacuumRangeUsesInterpolatedImpact() && success;
    success = testDefaultDragRangeStaysStableAcrossTimeSteps() && success;
    success = testSymplecticEulerRangeRemainsSupported() && success;
    success = testSimulationTimeStopsAtExactLanding() && success;
    success = testSimulationStateTransitions() && success;
    success = testSimulationAccessorsExposeConstViews() && success;
    success = testTrajectoryRecorderRetentionPolicyCapsGrowth() && success;
    success = testTrajectoryRecorderSamplingAccumulatesAcrossSkippedSteps() && success;
    success = testApexPointReportsExpectedHeightAndTime() && success;
    success = testResetArchivesTrajectoryHistoryAndDeduplicates() && success;

    if (!success)
    {
        return 1;
    }

    std::cout << "ProjectilePhysicsTests passed\n";
    return 0;
}
