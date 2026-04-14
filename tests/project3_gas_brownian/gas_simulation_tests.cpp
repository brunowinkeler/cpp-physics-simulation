#include "test_support.h"

#include "project3_gas_brownian/src/simulation/GasSimulation.h"

#include "gtest/gtest.h"

#include <algorithm>
#include <cmath>

TEST(GasSimulationTest, StateTransitionsAndReset)
{
    physim::GasEnvironment environment;
    physim::GasSimulation simulation{environment};

    EXPECT_EQ(simulation.getState(), physim::GasSimulationState::Idle);

    simulation.start();
    EXPECT_EQ(simulation.getState(), physim::GasSimulationState::Running);

    simulation.stop();
    EXPECT_EQ(simulation.getState(), physim::GasSimulationState::Paused);

    simulation.start();
    EXPECT_EQ(simulation.getState(), physim::GasSimulationState::Running);

    simulation.reset();
    EXPECT_EQ(simulation.getState(), physim::GasSimulationState::Idle);
    EXPECT_NEAR(simulation.getTimeGlobal(), 0.0f, 1.0e-6f);
}

TEST(GasSimulationTest, BrownianModeCreatesExactlyOneTracerParticle)
{
    physim::GasEnvironment environment;
    physim::GasSimulation simulation{environment};

    simulation.setMode(physim::GasSimulationMode::Brownian);

    int tracerCount = 0;
    for (const physim::GasParticle &particle : simulation.getParticles())
    {
        if (particle.isTracer)
        {
            ++tracerCount;
        }
    }

    EXPECT_EQ(tracerCount, 1);
    EXPECT_NE(simulation.getTracerParticle(), nullptr);
}

TEST(GasSimulationTest, PressureHistoryKeepsRecentSamplesAfterLongRun)
{
    constexpr std::size_t expectedMaxPressureSamples = 1024;

    physim::GasEnvironment environment;
    physim::GasSimulation simulation{environment};

    simulation.setParticleCount(100);
    simulation.setPhysicsTimeStep(0.05f);
    simulation.start();
    physim::testsupport::runGasSimulationSteps(simulation, 250, 0.25f);

    const auto &pressureHistory = simulation.getPressureHistory();

    ASSERT_GT(pressureHistory.size(), 8u);
    EXPECT_LE(pressureHistory.size(), expectedMaxPressureSamples);
    EXPECT_GT(pressureHistory.front().time, 0.0f);
    EXPECT_TRUE(std::isfinite(simulation.getCurrentPressureInstant()));

    const bool hasPositivePressureSample = std::any_of(
        pressureHistory.begin(),
        pressureHistory.end(),
        [](const physim::PressureHistorySample &sample)
        {
            return sample.pressure > 0.0f;
        });
    EXPECT_TRUE(hasPositivePressureSample);

    float maximumGap = 0.0f;
    for (std::size_t index = 1; index < pressureHistory.size(); ++index)
    {
        maximumGap = std::max(maximumGap, pressureHistory[index].time - pressureHistory[index - 1].time);
    }

    const float lag = simulation.getTimeGlobal() - pressureHistory.back().time;

    EXPECT_LE(maximumGap, 0.051f);
    EXPECT_GE(lag, 0.0f);
    EXPECT_LE(lag, maximumGap + 1.0e-4f);
}

TEST(GasSimulationTest, TracerTrailToggleStopsRecording)
{
    physim::GasEnvironment environment;
    physim::GasSimulation simulation{environment};

    simulation.setMode(physim::GasSimulationMode::Brownian);
    simulation.setParticleCount(300);
    simulation.setTracerTrailEnabled(false);
    simulation.start();
    physim::testsupport::runGasSimulationSteps(simulation, 1000, 0.01f);

    EXPECT_TRUE(simulation.getTracerTrailPoints().empty());

    simulation.stop();
    simulation.setTracerTrailEnabled(true);
    simulation.reset();
    simulation.start();
    physim::testsupport::runGasSimulationSteps(simulation, 1000, 0.01f);

    EXPECT_GT(simulation.getTracerTrailPoints().size(), 2u);
}

TEST(GasSimulationTest, ParticleCountCanScaleToThousands)
{
    physim::GasEnvironment environment;
    physim::GasSimulation simulation{environment};

    simulation.setParticleCount(2000);

    EXPECT_EQ(simulation.getParticles().size(), 2000u);
}