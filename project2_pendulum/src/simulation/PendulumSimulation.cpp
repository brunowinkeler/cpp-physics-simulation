#include "PendulumSimulation.h"

#include <algorithm>
#include <utility>

namespace physim
{
    namespace
    {
        constexpr float MIN_PHYSICS_TIME_STEP = 0.0001f;
        constexpr float MIN_ANALYSIS_RECORDED_TIME_STEP = 1.0f / 240.0f;
        constexpr std::size_t MAX_ENERGY_HISTORY_SAMPLES = 4096;
        constexpr std::size_t MAX_PHASE_SPACE_HISTORY_SAMPLES = 1024;
        constexpr TrajectoryRetentionPolicy PENDULUM_TRAIL_RETENTION_POLICY{
            1024,
            1.0f / 240.0f,
            TrajectoryRetentionMode::RollingWindow,
        };

        template <typename Sample>
        void compactAnalysisSamples(std::vector<Sample> &samples)
        {
            if (samples.size() <= 2)
            {
                return;
            }

            std::vector<Sample> compactedSamples;
            compactedSamples.reserve((samples.size() / 2) + 1);

            for (std::size_t index = 0; index < samples.size(); index += 2)
            {
                compactedSamples.push_back(samples[index]);
            }

            if (compactedSamples.back().time < samples.back().time)
            {
                compactedSamples.push_back(samples.back());
            }

            samples = std::move(compactedSamples);
        }

        template <typename Sample>
        void discardOldestAnalysisSamples(std::vector<Sample> &samples, std::size_t maxSamples)
        {
            if (samples.size() <= maxSamples)
            {
                return;
            }

            const std::size_t overflow = samples.size() - maxSamples;
            samples.erase(samples.begin(), samples.begin() + static_cast<std::ptrdiff_t>(overflow));
        }

        template <typename Sample>
        void retainAnalysisHistoryWithinBudget(std::vector<Sample> &samples,
                                               std::size_t maxSamples,
                                               TrajectoryRetentionMode retentionMode)
        {
            if (retentionMode == TrajectoryRetentionMode::RollingWindow)
            {
                discardOldestAnalysisSamples(samples, maxSamples);
                return;
            }

            while (samples.size() > maxSamples)
            {
                compactAnalysisSamples(samples);
            }
        }

        template <typename Sample>
        void recordAnalysisSample(std::vector<Sample> &samples,
                                  const Sample &sample,
                                  bool forceSample,
                                  std::size_t maxSamples,
                                  TrajectoryRetentionMode retentionMode)
        {
            if (samples.empty())
            {
                samples.push_back(sample);
                return;
            }

            Sample &lastSample = samples.back();
            if (sample.time <= lastSample.time)
            {
                lastSample = sample;
                return;
            }

            if (!forceSample && ((sample.time - lastSample.time) < MIN_ANALYSIS_RECORDED_TIME_STEP))
            {
                return;
            }

            samples.push_back(sample);
            retainAnalysisHistoryWithinBudget(samples, maxSamples, retentionMode);
        }
    }

    PendulumSimulation::PendulumSimulation(PendulumEnvironment &environment,
                                           SimplePendulum &simplePendulum,
                                           DoublePendulum &doublePendulum)
        : environment{environment},
          simplePendulum{simplePendulum},
                    doublePendulum{doublePendulum},
                    primaryTrail{PENDULUM_TRAIL_RETENTION_POLICY},
                    secondaryTrail{PENDULUM_TRAIL_RETENTION_POLICY}
    {
        reset();
    }

    void PendulumSimulation::start()
    {
        if (state == PendulumSimulationState::Running)
        {
            return;
        }

        if (state == PendulumSimulationState::Idle)
        {
            resetActiveSystem();
            clearTrails();
            clearAnalysisHistory();
            accumulatedFrameTime = 0.0f;
            timeGlobal = 0.0f;
            initialTotalEnergy = getCurrentTotalEnergy();
            recordCurrentState(true);
        }

        state = PendulumSimulationState::Running;
    }

    void PendulumSimulation::stop()
    {
        if (state == PendulumSimulationState::Running)
        {
            state = PendulumSimulationState::Paused;
        }
    }

    void PendulumSimulation::reset()
    {
        resetActiveSystem();
        clearTrails();
        clearAnalysisHistory();
        accumulatedFrameTime = 0.0f;
        timeGlobal = 0.0f;
        initialTotalEnergy = getCurrentTotalEnergy();
        recordCurrentState(true);
        state = PendulumSimulationState::Idle;
    }

    void PendulumSimulation::update(float timeStep)
    {
        if (state != PendulumSimulationState::Running || timeStep <= 0.0f)
        {
            return;
        }

        accumulatedFrameTime += timeStep;
        while (accumulatedFrameTime >= physicsTimeStep)
        {
            if (mode == PendulumMode::Simple)
            {
                simplePendulum.update(physicsTimeStep, environment);
            }
            else
            {
                doublePendulum.update(physicsTimeStep, environment);
            }

            accumulatedFrameTime -= physicsTimeStep;
            timeGlobal += physicsTimeStep;
            recordCurrentState(false);
        }
    }

    void PendulumSimulation::setMode(PendulumMode mode)
    {
        if (state == PendulumSimulationState::Running || this->mode == mode)
        {
            return;
        }

        this->mode = mode;
        reset();
    }

    void PendulumSimulation::setPhysicsTimeStep(float timeStep)
    {
        physicsTimeStep = std::max(timeStep, MIN_PHYSICS_TIME_STEP);
    }

    float PendulumSimulation::getCurrentTotalEnergy() const
    {
        return mode == PendulumMode::Simple ? simplePendulum.getTotalEnergy(environment) : doublePendulum.getTotalEnergy(environment);
    }

    void PendulumSimulation::resetActiveSystem()
    {
        if (mode == PendulumMode::Simple)
        {
            simplePendulum.reset();
        }
        else
        {
            doublePendulum.reset();
        }
    }

    void PendulumSimulation::clearTrails()
    {
        primaryTrail.clear();
        secondaryTrail.clear();
    }

    void PendulumSimulation::clearAnalysisHistory()
    {
        energyHistory.clear();
        primaryPhaseSpaceHistory.clear();
        secondaryPhaseSpaceHistory.clear();
    }

    void PendulumSimulation::recordCurrentState(bool forceSample)
    {
        recordAnalysisSample(energyHistory,
                             {timeGlobal, getCurrentTotalEnergy()},
                             forceSample,
                             MAX_ENERGY_HISTORY_SAMPLES,
                             TrajectoryRetentionMode::CompactHistory);

        if (mode == PendulumMode::Simple)
        {
            const PendulumBob bob = simplePendulum.getBob();
            primaryTrail.record(bob.x, bob.y, timeGlobal, bob.speed, forceSample);
            const SimplePendulumState &state = simplePendulum.getState();
            recordAnalysisSample(primaryPhaseSpaceHistory,
                                 {timeGlobal, static_cast<float>(state.theta), static_cast<float>(state.omega)},
                                 forceSample,
                                 MAX_PHASE_SPACE_HISTORY_SAMPLES,
                                 TrajectoryRetentionMode::RollingWindow);
            return;
        }

        const DoublePendulumBobs bobs = doublePendulum.getBobs();
        const DoublePendulumState &state = doublePendulum.getState();
        primaryTrail.record(bobs.first.x, bobs.first.y, timeGlobal, bobs.first.speed, forceSample);
        secondaryTrail.record(bobs.second.x, bobs.second.y, timeGlobal, bobs.second.speed, forceSample);
        recordAnalysisSample(primaryPhaseSpaceHistory,
                             {timeGlobal, static_cast<float>(state.theta1), static_cast<float>(state.omega1)},
                             forceSample,
                             MAX_PHASE_SPACE_HISTORY_SAMPLES,
                             TrajectoryRetentionMode::RollingWindow);
        recordAnalysisSample(secondaryPhaseSpaceHistory,
                             {timeGlobal, static_cast<float>(state.theta2), static_cast<float>(state.omega2)},
                             forceSample,
                             MAX_PHASE_SPACE_HISTORY_SAMPLES,
                             TrajectoryRetentionMode::RollingWindow);
    }
} // namespace physim
