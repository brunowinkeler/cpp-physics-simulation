#include "GasSimulation.h"

#include "core/GlobalDefinitions.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <random>

namespace physim
{
    namespace
    {
        constexpr float MIN_PHYSICS_TIME_STEP = 1.0e-4f;
        constexpr float MIN_BOX_WIDTH = 4.0f;
        constexpr float MAX_BOX_WIDTH = 30.0f;
        constexpr float MIN_BOX_HEIGHT = 2.0f;
        constexpr float MAX_BOX_HEIGHT = 20.0f;
        constexpr int MIN_PARTICLE_COUNT = 100;
        constexpr int MAX_PARTICLE_COUNT = 10000;
        constexpr float MIN_TEMPERATURE = 20.0f;
        constexpr float MAX_TEMPERATURE = 600.0f;
        constexpr float MIN_TRACER_MASS_MULTIPLIER = 1.0f;
        constexpr float MAX_TRACER_MASS_MULTIPLIER = 64.0f;
        constexpr float MIN_TRACER_RADIUS_MULTIPLIER = 1.0f;
        constexpr float MAX_TRACER_RADIUS_MULTIPLIER = 8.0f;
        constexpr float EFFECTIVE_BOLTZMANN_CONSTANT = 2.0e-27f;
        constexpr float PRESSURE_WINDOW_DURATION = 3.0f;
        constexpr float MIN_PRESSURE_SAMPLE_TIME_STEP = 1.0f / 60.0f;
        constexpr std::size_t MAX_PRESSURE_HISTORY_SAMPLES = 1024;
        constexpr TrajectoryRetentionPolicy TRACER_TRAIL_RETENTION_POLICY{
            1024,
            1.0f / 240.0f,
            TrajectoryRetentionMode::RollingWindow,
        };

        float clampTemperature(float temperatureKelvin)
        {
            return std::clamp(temperatureKelvin, MIN_TEMPERATURE, MAX_TEMPERATURE);
        }

        template <typename Sample>
        void discardOldestSamples(std::vector<Sample> &samples, std::size_t maxSamples)
        {
            if (samples.size() <= maxSamples)
            {
                return;
            }

            const std::size_t overflow = samples.size() - maxSamples;
            samples.erase(samples.begin(), samples.begin() + static_cast<std::ptrdiff_t>(overflow));
        }
    }

    GasSimulation::GasSimulation(GasEnvironment &environment)
        : environment{environment},
          tracerTrail{TRACER_TRAIL_RETENTION_POLICY}
    {
        reset();
    }

    void GasSimulation::start()
    {
        if (state == GasSimulationState::Running)
        {
            return;
        }

        state = GasSimulationState::Running;
    }

    void GasSimulation::stop()
    {
        if (state == GasSimulationState::Running)
        {
            state = GasSimulationState::Paused;
        }
    }

    void GasSimulation::reset()
    {
        regenerateParticles();
        clearRecordedData();
        accumulatedFrameTime = 0.0f;
        timeGlobal = 0.0f;
        updateMacroscopicQuantities(0.0f, true);
        recordTracerState(true);
        state = GasSimulationState::Idle;
    }

    void GasSimulation::update(float timeStep)
    {
        if (state != GasSimulationState::Running || timeStep <= 0.0f)
        {
            return;
        }

        accumulatedFrameTime += timeStep;
        const GasBoxBounds boxBounds{environment.boxWidth, environment.boxHeight};

        while (accumulatedFrameTime >= physicsTimeStep)
        {
            for (GasParticle &particle : particles)
            {
                particle.position += particle.velocity * physicsTimeStep;
            }

            float wallImpulse = 0.0f;
            for (GasParticle &particle : particles)
            {
                resolveWallCollision(particle, boxBounds, wallImpulse);
            }

            collisionGrid.configure(environment.boxWidth, environment.boxHeight, computeGridCellSize());
            for (std::size_t index = 0; index < particles.size(); ++index)
            {
                collisionGrid.insert(particles[index].position, static_cast<int>(index));
            }

            for (std::size_t index = 0; index < particles.size(); ++index)
            {
                const auto [column, row] = collisionGrid.getCellCoordinates(particles[index].position);
                for (int neighborRow = row - 1; neighborRow <= row + 1; ++neighborRow)
                {
                    for (int neighborColumn = column - 1; neighborColumn <= column + 1; ++neighborColumn)
                    {
                        const std::vector<int> &cell = collisionGrid.getCell(neighborColumn, neighborRow);
                        for (int otherIndex : cell)
                        {
                            if (otherIndex <= static_cast<int>(index))
                            {
                                continue;
                            }

                            resolveParticleCollision(particles[index], particles[static_cast<std::size_t>(otherIndex)]);
                        }
                    }
                }
            }

            accumulatedFrameTime -= physicsTimeStep;
            timeGlobal += physicsTimeStep;
            updateMacroscopicQuantities(wallImpulse, false);
            recordTracerState(false);
        }
    }

    void GasSimulation::setMode(GasSimulationMode mode)
    {
        if (state == GasSimulationState::Running || this->mode == mode)
        {
            return;
        }

        this->mode = mode;
        reset();
    }

    void GasSimulation::setPhysicsTimeStep(float timeStep)
    {
        if (state == GasSimulationState::Running)
        {
            return;
        }

        physicsTimeStep = std::max(timeStep, MIN_PHYSICS_TIME_STEP);
    }

    void GasSimulation::setParticleCount(int particleCount)
    {
        if (state == GasSimulationState::Running)
        {
            return;
        }

        const int clampedParticleCount = std::clamp(particleCount, MIN_PARTICLE_COUNT, MAX_PARTICLE_COUNT);
        if (this->particleCount == clampedParticleCount)
        {
            return;
        }

        this->particleCount = clampedParticleCount;
        reset();
    }

    void GasSimulation::setGasPresetId(GasPresetId gasPresetId)
    {
        if (state == GasSimulationState::Running || this->gasPresetId == gasPresetId)
        {
            return;
        }

        this->gasPresetId = gasPresetId;
        initialTemperatureKelvin = physim::getGasPreset(gasPresetId).defaultTemperatureKelvin;
        reset();
    }

    void GasSimulation::setInitialTemperatureKelvin(float temperatureKelvin)
    {
        if (state == GasSimulationState::Running)
        {
            return;
        }

        const float clampedTemperature = clampTemperature(temperatureKelvin);
        if (std::abs(initialTemperatureKelvin - clampedTemperature) <= 1.0e-6f)
        {
            return;
        }

        initialTemperatureKelvin = clampedTemperature;
        reset();
    }

    void GasSimulation::setRandomSeed(unsigned int randomSeed)
    {
        if (state == GasSimulationState::Running || this->randomSeed == randomSeed)
        {
            return;
        }

        this->randomSeed = randomSeed;
        reset();
    }

    void GasSimulation::setBoxSize(float width, float height)
    {
        if (state == GasSimulationState::Running)
        {
            return;
        }

        const float clampedWidth = std::clamp(width, MIN_BOX_WIDTH, MAX_BOX_WIDTH);
        const float clampedHeight = std::clamp(height, MIN_BOX_HEIGHT, MAX_BOX_HEIGHT);
        if (std::abs(environment.boxWidth - clampedWidth) <= 1.0e-6f &&
            std::abs(environment.boxHeight - clampedHeight) <= 1.0e-6f)
        {
            return;
        }

        environment.boxWidth = clampedWidth;
        environment.boxHeight = clampedHeight;
        reset();
    }

    void GasSimulation::setTracerMassMultiplier(float massMultiplier)
    {
        if (state == GasSimulationState::Running)
        {
            return;
        }

        const float clampedMassMultiplier = std::clamp(massMultiplier, MIN_TRACER_MASS_MULTIPLIER, MAX_TRACER_MASS_MULTIPLIER);
        if (std::abs(tracerMassMultiplier - clampedMassMultiplier) <= 1.0e-6f)
        {
            return;
        }

        tracerMassMultiplier = clampedMassMultiplier;
        reset();
    }

    void GasSimulation::setTracerRadiusMultiplier(float radiusMultiplier)
    {
        if (state == GasSimulationState::Running)
        {
            return;
        }

        const float clampedRadiusMultiplier = std::clamp(radiusMultiplier, MIN_TRACER_RADIUS_MULTIPLIER, MAX_TRACER_RADIUS_MULTIPLIER);
        if (std::abs(tracerRadiusMultiplier - clampedRadiusMultiplier) <= 1.0e-6f)
        {
            return;
        }

        tracerRadiusMultiplier = clampedRadiusMultiplier;
        reset();
    }

    void GasSimulation::setTracerTrailEnabled(bool enabled)
    {
        if (tracerTrailEnabled == enabled)
        {
            return;
        }

        tracerTrailEnabled = enabled;
        tracerTrail.clear();
        recordTracerState(true);
    }

    const GasPreset &GasSimulation::getGasPreset() const
    {
        return physim::getGasPreset(gasPresetId);
    }

    const GasParticle *GasSimulation::getTracerParticle() const
    {
        if (tracerParticleIndex < 0 || tracerParticleIndex >= static_cast<int>(particles.size()))
        {
            return nullptr;
        }

        return &particles[static_cast<std::size_t>(tracerParticleIndex)];
    }

    void GasSimulation::regenerateParticles()
    {
        particles.clear();

        const GasPreset &preset = getGasPreset();
        const int totalParticleCount = std::max(particleCount, computeMinimumParticleCount());
        const float bathRadius = preset.effectiveParticleRadius;
        const float tracerRadius = bathRadius * tracerRadiusMultiplier;
        const float maxRadius = mode == GasSimulationMode::Brownian ? std::max(bathRadius, tracerRadius) : bathRadius;

        const int columnCount = std::max(1, static_cast<int>(std::ceil(std::sqrt(static_cast<float>(totalParticleCount) * environment.boxWidth / environment.boxHeight))));
        const int rowCount = std::max(1, static_cast<int>(std::ceil(static_cast<float>(totalParticleCount) / static_cast<float>(columnCount))));
        const float stepX = environment.boxWidth / static_cast<float>(columnCount + 1);
        const float stepY = environment.boxHeight / static_cast<float>(rowCount + 1);

        std::mt19937 generator{randomSeed};
        std::uniform_real_distribution<float> angleDistribution{0.0f, 2.0f * constants::PI};
        std::uniform_real_distribution<float> speedJitterDistribution{0.85f, 1.15f};
        std::uniform_real_distribution<float> jitterDistribution{-1.0f, 1.0f};

        tracerParticleIndex = mode == GasSimulationMode::Brownian ? 0 : -1;

        for (int index = 0; index < totalParticleCount; ++index)
        {
            const bool isTracer = index == tracerParticleIndex;
            const int column = index % columnCount;
            const int row = index / columnCount;

            const float radius = isTracer ? tracerRadius : bathRadius;
            const float mass = isTracer ? (preset.particleMassKg * tracerMassMultiplier) : preset.particleMassKg;
            const float baseX = stepX * static_cast<float>(column + 1);
            const float baseY = stepY * static_cast<float>(row + 1);
            const float jitterX = std::max(0.0f, (stepX - (2.2f * radius)) * 0.35f);
            const float jitterY = std::max(0.0f, (stepY - (2.2f * radius)) * 0.35f);

            GasParticle particle{
                .position = {
                    std::clamp(baseX + (jitterDistribution(generator) * jitterX), radius, environment.boxWidth - radius),
                    std::clamp(baseY + (jitterDistribution(generator) * jitterY), radius, environment.boxHeight - radius),
                },
                .velocity = {0.0f, 0.0f},
                .mass = mass,
                .radius = radius,
                .isTracer = isTracer,
            };

            const float effectiveTemperature = clampTemperature(initialTemperatureKelvin);
            const float speedMagnitude = std::sqrt((2.0f * EFFECTIVE_BOLTZMANN_CONSTANT * effectiveTemperature) / std::max(mass, 1.0e-30f));
            const float angle = angleDistribution(generator);
            const float speed = speedMagnitude * speedJitterDistribution(generator);
            particle.velocity = {std::cos(angle) * speed, std::sin(angle) * speed};

            particles.push_back(particle);
        }

        if (!particles.empty())
        {
            particles.front().radius = std::max(particles.front().radius, maxRadius);
        }
    }

    void GasSimulation::clearRecordedData()
    {
        tracerTrail.clear();
        pressureHistory.clear();
        wallImpulseWindow.clear();
        rollingWallImpulse = 0.0f;
        currentPressureInstant = 0.0f;
    }

    void GasSimulation::recordTracerState(bool forceSample)
    {
        if (!tracerTrailEnabled)
        {
            return;
        }

        const GasParticle *tracerParticle = getTracerParticle();
        if (tracerParticle == nullptr)
        {
            return;
        }

        tracerTrail.record(tracerParticle->position.x,
                           tracerParticle->position.y,
                           timeGlobal,
                           gasLength(tracerParticle->velocity),
                           forceSample);
    }

    void GasSimulation::updateMacroscopicQuantities(float wallImpulse, bool forceSample)
    {
        if (!forceSample)
        {
            wallImpulseWindow.push_back({timeGlobal, wallImpulse});
            rollingWallImpulse += wallImpulse;
            trimWallImpulseWindow();
        }

        const float perimeter = std::max(2.0f * (environment.boxWidth + environment.boxHeight), 1.0e-6f);
        float effectiveWindowDuration = physicsTimeStep;
        if (!wallImpulseWindow.empty())
        {
            effectiveWindowDuration = std::max(timeGlobal - wallImpulseWindow.front().time, physicsTimeStep);
        }

        currentPressureInstant = rollingWallImpulse / (effectiveWindowDuration * perimeter);

        float totalKineticEnergy = 0.0f;
        float totalSpeed = 0.0f;
        for (const GasParticle &particle : particles)
        {
            const float speedSquared = gasLengthSquared(particle.velocity);
            totalKineticEnergy += 0.5f * particle.mass * speedSquared;
            totalSpeed += std::sqrt(speedSquared);
        }

        if (particles.empty())
        {
            averageKineticEnergy = 0.0f;
            averageSpeed = 0.0f;
            currentTemperatureKelvin = 0.0f;
        }
        else
        {
            averageKineticEnergy = totalKineticEnergy / static_cast<float>(particles.size());
            averageSpeed = totalSpeed / static_cast<float>(particles.size());
            currentTemperatureKelvin = averageKineticEnergy / EFFECTIVE_BOLTZMANN_CONSTANT;
        }

        appendPressureHistorySample(forceSample);
    }

    void GasSimulation::appendPressureHistorySample(bool forceSample)
    {
        if (!forceSample && !pressureHistory.empty() && ((timeGlobal - pressureHistory.back().time) < MIN_PRESSURE_SAMPLE_TIME_STEP))
        {
            return;
        }

        if (!pressureHistory.empty() && timeGlobal <= pressureHistory.back().time)
        {
            pressureHistory.back() = {timeGlobal, currentPressureInstant};
            return;
        }

        pressureHistory.push_back({timeGlobal, currentPressureInstant});
        discardOldestSamples(pressureHistory, MAX_PRESSURE_HISTORY_SAMPLES);
    }

    void GasSimulation::trimWallImpulseWindow()
    {
        const float oldestAllowedTime = std::max(0.0f, timeGlobal - PRESSURE_WINDOW_DURATION);
        while (!wallImpulseWindow.empty() && wallImpulseWindow.front().time < oldestAllowedTime)
        {
            rollingWallImpulse -= wallImpulseWindow.front().impulse;
            wallImpulseWindow.erase(wallImpulseWindow.begin());
        }
    }

    float GasSimulation::computeGridCellSize() const
    {
        const float bathRadius = getGasPreset().effectiveParticleRadius;
        const float tracerRadius = bathRadius * tracerRadiusMultiplier;
        const float maxRadius = mode == GasSimulationMode::Brownian ? std::max(bathRadius, tracerRadius) : bathRadius;
        return std::max(8.0f * maxRadius, 0.25f);
    }

    int GasSimulation::computeMinimumParticleCount() const
    {
        return mode == GasSimulationMode::Brownian ? 2 : 1;
    }
} // namespace physim
