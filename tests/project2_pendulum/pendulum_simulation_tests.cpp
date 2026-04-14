#include "test_support.h"

#include "project2_pendulum/src/physics/DoublePendulum.h"
#include "project2_pendulum/src/simulation/PendulumSimulation.h"

#include "gtest/gtest.h"

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
