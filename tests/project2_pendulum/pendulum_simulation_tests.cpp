#include "test_support.h"

#include "project2_pendulum/src/physics/DoublePendulum.h"
#include "project2_pendulum/src/simulation/PendulumSimulation.h"

#include "gtest/gtest.h"

#include <algorithm>

TEST(PendulumSimulationTest, StateTransitionsAndReset)
{
    physim::PendulumEnvironment environment;
    physim::SimplePendulum simplePendulum;
    physim::DoublePendulum doublePendulum;
    physim::PendulumSimulation simulation{environment, simplePendulum, doublePendulum};

    EXPECT_EQ(simulation.getState(), physim::PendulumSimulationState::Idle);

    simulation.start();
    EXPECT_EQ(simulation.getState(), physim::PendulumSimulationState::Running);

    simulation.stop();
    EXPECT_EQ(simulation.getState(), physim::PendulumSimulationState::Paused);

    simulation.start();
    EXPECT_EQ(simulation.getState(), physim::PendulumSimulationState::Running);

    simulation.reset();
    EXPECT_EQ(simulation.getState(), physim::PendulumSimulationState::Idle);
    EXPECT_NEAR(simulation.getTimeGlobal(), 0.0, 1.0e-6);
}

TEST(PendulumSimulationTest, SimpleModeRecordsPrimaryTrail)
{
    physim::PendulumEnvironment environment;
    physim::SimplePendulum simplePendulum;
    physim::DoublePendulum doublePendulum;
    physim::PendulumSimulation simulation{environment, simplePendulum, doublePendulum};

    simulation.start();
    physim::testsupport::runPendulumSimulationSteps(simulation, 4000, 0.001f);

    EXPECT_GT(simulation.getPrimaryTrailPoints().size(), 2u);
    EXPECT_TRUE(simulation.getSecondaryTrailPoints().empty());
    EXPECT_GT(simulation.getEnergyHistory().size(), 2u);
    EXPECT_GT(simulation.getPrimaryPhaseSpaceHistory().size(), 2u);
    EXPECT_TRUE(simulation.getSecondaryPhaseSpaceHistory().empty());
}

TEST(PendulumSimulationTest, DoubleModeRecordsTwoTrails)
{
    physim::PendulumEnvironment environment;
    physim::SimplePendulum simplePendulum;
    physim::DoublePendulum doublePendulum;
    physim::PendulumSimulation simulation{environment, simplePendulum, doublePendulum};

    simulation.setMode(physim::PendulumMode::Double);
    simulation.start();
    physim::testsupport::runPendulumSimulationSteps(simulation, 4000, 0.001f);

    EXPECT_GT(simulation.getPrimaryTrailPoints().size(), 2u);
    EXPECT_GT(simulation.getSecondaryTrailPoints().size(), 2u);
    EXPECT_GT(simulation.getEnergyHistory().size(), 2u);
    EXPECT_GT(simulation.getPrimaryPhaseSpaceHistory().size(), 2u);
    EXPECT_GT(simulation.getSecondaryPhaseSpaceHistory().size(), 2u);
}

TEST(PendulumSimulationTest, ModeSwitchResetsTimeAndTrails)
{
    physim::PendulumEnvironment environment;
    physim::SimplePendulum simplePendulum;
    physim::DoublePendulum doublePendulum;
    physim::PendulumSimulation simulation{environment, simplePendulum, doublePendulum};

    simulation.start();
    physim::testsupport::runPendulumSimulationSteps(simulation, 2000, 0.001f);
    simulation.stop();

    simulation.setMode(physim::PendulumMode::Double);

    EXPECT_EQ(simulation.getMode(), physim::PendulumMode::Double);
    EXPECT_EQ(simulation.getState(), physim::PendulumSimulationState::Idle);
    EXPECT_NEAR(simulation.getTimeGlobal(), 0.0, 1.0e-6);
    EXPECT_EQ(simulation.getPrimaryTrailPoints().size(), 1u);
    EXPECT_EQ(simulation.getSecondaryTrailPoints().size(), 1u);
    EXPECT_EQ(simulation.getEnergyHistory().size(), 1u);
    EXPECT_EQ(simulation.getPrimaryPhaseSpaceHistory().size(), 1u);
    EXPECT_EQ(simulation.getSecondaryPhaseSpaceHistory().size(), 1u);
}

TEST(PendulumSimulationTest, TrailWindowKeepsRecentSamplesAfterLongRun)
{
    constexpr std::size_t expectedMaxTrailPoints = 1024;

    physim::PendulumEnvironment environment;
    physim::SimplePendulum simplePendulum;
    physim::DoublePendulum doublePendulum;
    physim::PendulumSimulation simulation{environment, simplePendulum, doublePendulum};

    simulation.setMode(physim::PendulumMode::Double);
    simulation.start();
    physim::testsupport::runPendulumSimulationSteps(simulation, 10000, 0.001f);

    const auto &primaryTrail = simulation.getPrimaryTrailPoints();
    const auto &secondaryTrail = simulation.getSecondaryTrailPoints();

    ASSERT_GT(primaryTrail.size(), 2u);
    ASSERT_GT(secondaryTrail.size(), 2u);
    EXPECT_LE(primaryTrail.size(), expectedMaxTrailPoints);
    EXPECT_LE(secondaryTrail.size(), expectedMaxTrailPoints);
    EXPECT_GT(primaryTrail.front().time, 0.0f)
        << "Pendulum trails should drop the oldest points once the retention window is full";
    EXPECT_GT(secondaryTrail.front().time, 0.0f)
        << "Both double-pendulum trails should keep only the recent window";
    float primaryMaxGap = 0.0f;
    float secondaryMaxGap = 0.0f;
    for (std::size_t index = 1; index < primaryTrail.size(); ++index)
    {
        primaryMaxGap = std::max(primaryMaxGap, primaryTrail[index].time - primaryTrail[index - 1].time);
    }
    for (std::size_t index = 1; index < secondaryTrail.size(); ++index)
    {
        secondaryMaxGap = std::max(secondaryMaxGap, secondaryTrail[index].time - secondaryTrail[index - 1].time);
    }

    EXPECT_LE(primaryMaxGap, 0.01f)
        << "Primary trail should preserve a fine temporal resolution inside the retained window";
    EXPECT_LE(secondaryMaxGap, 0.01f)
        << "Secondary trail should preserve a fine temporal resolution inside the retained window";

    const float primaryLag = simulation.getTimeGlobal() - primaryTrail.back().time;
    const float secondaryLag = simulation.getTimeGlobal() - secondaryTrail.back().time;

    EXPECT_GE(primaryLag, 0.0f)
        << "Primary trail samples should not move ahead of simulation time";
    EXPECT_GE(secondaryLag, 0.0f)
        << "Secondary trail samples should not move ahead of simulation time";
    EXPECT_LE(primaryLag, primaryMaxGap + 1.0e-4f)
        << "Primary trail should remain close to the latest simulated state";
    EXPECT_LE(secondaryLag, secondaryMaxGap + 1.0e-4f)
        << "Secondary trail should remain close to the latest simulated state";
}

TEST(PendulumSimulationTest, PhaseWindowKeepsRecentSamplesAfterLongRun)
{
    constexpr std::size_t expectedMaxPhaseSamples = 1024;

    physim::PendulumEnvironment environment;
    physim::SimplePendulum simplePendulum;
    physim::DoublePendulum doublePendulum;
    physim::PendulumSimulation simulation{environment, simplePendulum, doublePendulum};

    simulation.setMode(physim::PendulumMode::Double);
    simulation.start();
    physim::testsupport::runPendulumSimulationSteps(simulation, 10000, 0.001f);

    const auto &primaryPhaseHistory = simulation.getPrimaryPhaseSpaceHistory();
    const auto &secondaryPhaseHistory = simulation.getSecondaryPhaseSpaceHistory();

    ASSERT_GT(primaryPhaseHistory.size(), 2u);
    ASSERT_GT(secondaryPhaseHistory.size(), 2u);
    EXPECT_LE(primaryPhaseHistory.size(), expectedMaxPhaseSamples);
    EXPECT_LE(secondaryPhaseHistory.size(), expectedMaxPhaseSamples);
    EXPECT_GT(primaryPhaseHistory.front().time, 0.0f)
        << "Primary phase history should discard the oldest samples once the rolling window is full";
    EXPECT_GT(secondaryPhaseHistory.front().time, 0.0f)
        << "Secondary phase history should keep only the recent window";

    float primaryMaxGap = 0.0f;
    float secondaryMaxGap = 0.0f;
    for (std::size_t index = 1; index < primaryPhaseHistory.size(); ++index)
    {
        primaryMaxGap = std::max(primaryMaxGap, primaryPhaseHistory[index].time - primaryPhaseHistory[index - 1].time);
    }
    for (std::size_t index = 1; index < secondaryPhaseHistory.size(); ++index)
    {
        secondaryMaxGap = std::max(secondaryMaxGap, secondaryPhaseHistory[index].time - secondaryPhaseHistory[index - 1].time);
    }

    EXPECT_LE(primaryMaxGap, 0.01f)
        << "Primary phase history should preserve a fine temporal resolution inside the retained window";
    EXPECT_LE(secondaryMaxGap, 0.01f)
        << "Secondary phase history should preserve a fine temporal resolution inside the retained window";

    const float primaryLag = simulation.getTimeGlobal() - primaryPhaseHistory.back().time;
    const float secondaryLag = simulation.getTimeGlobal() - secondaryPhaseHistory.back().time;

    EXPECT_GE(primaryLag, 0.0f)
        << "Primary phase samples should not move ahead of simulation time";
    EXPECT_GE(secondaryLag, 0.0f)
        << "Secondary phase samples should not move ahead of simulation time";
    EXPECT_LE(primaryLag, primaryMaxGap + 1.0e-4f)
        << "Primary phase history should remain close to the latest simulated state";
    EXPECT_LE(secondaryLag, secondaryMaxGap + 1.0e-4f)
        << "Secondary phase history should remain close to the latest simulated state";
}
