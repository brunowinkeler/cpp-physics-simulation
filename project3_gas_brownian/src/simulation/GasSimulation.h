#ifndef GAS_SIMULATION_H
#define GAS_SIMULATION_H

#include "core/simulation/TrajectoryRecorder.h"

#include "physics/CollisionGrid.h"
#include "physics/GasCollision.h"
#include "physics/GasEnvironment.h"
#include "physics/GasMode.h"
#include "physics/GasParticle.h"
#include "physics/GasPreset.h"

#include <optional>
#include <vector>

namespace physim
{
    struct PressureHistorySample
    {
        float time;
        float pressure;
    };

    class GasSimulation
    {
    public:
        explicit GasSimulation(GasEnvironment &environment);

        void start();
        void stop();
        void reset();
        void update(float timeStep);

        void setMode(GasSimulationMode mode);
        void setPhysicsTimeStep(float timeStep);
        void setParticleCount(int particleCount);
        void setGasPresetId(GasPresetId gasPresetId);
        void setInitialTemperatureKelvin(float temperatureKelvin);
        void setRandomSeed(unsigned int randomSeed);
        void setBoxSize(float width, float height);
        void setTracerMassMultiplier(float massMultiplier);
        void setTracerRadiusMultiplier(float radiusMultiplier);
        void setTracerTrailEnabled(bool enabled);

        GasSimulationMode getMode() const { return mode; }
        bool isIdealGasMode() const { return mode == GasSimulationMode::IdealGas; }
        bool isBrownianMode() const { return mode == GasSimulationMode::Brownian; }

        GasSimulationState getState() const { return state; }
        bool isIdle() const { return state == GasSimulationState::Idle; }
        bool isRunning() const { return state == GasSimulationState::Running; }
        bool isPaused() const { return state == GasSimulationState::Paused; }

        float getTimeGlobal() const { return timeGlobal; }
        float getPhysicsTimeStep() const { return physicsTimeStep; }
        int getParticleCount() const { return particleCount; }
        GasPresetId getGasPresetId() const { return gasPresetId; }
        const GasPreset &getGasPreset() const;
        float getInitialTemperatureKelvin() const { return initialTemperatureKelvin; }
        unsigned int getRandomSeed() const { return randomSeed; }
        float getTracerMassMultiplier() const { return tracerMassMultiplier; }
        float getTracerRadiusMultiplier() const { return tracerRadiusMultiplier; }
        bool isTracerTrailEnabled() const { return tracerTrailEnabled; }

        float getCurrentPressureInstant() const { return currentPressureInstant; }
        float getCurrentTemperatureKelvin() const { return currentTemperatureKelvin; }
        float getAverageKineticEnergy() const { return averageKineticEnergy; }
        float getAverageSpeed() const { return averageSpeed; }

        const std::vector<GasParticle> &getParticles() const { return particles; }
        const GasParticle *getTracerParticle() const;
        const std::vector<TrajectoryPoint> &getTracerTrailPoints() const { return tracerTrail.getPoints(); }
        const std::vector<PressureHistorySample> &getPressureHistory() const { return pressureHistory; }

    private:
        struct WallImpulseSample
        {
            float time;
            float impulse;
        };

        void regenerateParticles();
        void clearRecordedData();
        void recordTracerState(bool forceSample);
        void updateMacroscopicQuantities(float wallImpulse, bool forceSample);
        void appendPressureHistorySample(bool forceSample);
        void trimWallImpulseWindow();
        float computeGridCellSize() const;
        int computeMinimumParticleCount() const;

        GasEnvironment &environment;
        CollisionGrid collisionGrid;
        std::vector<GasParticle> particles;
        TrajectoryRecorder tracerTrail;
        std::vector<PressureHistorySample> pressureHistory;
        std::vector<WallImpulseSample> wallImpulseWindow;

        float accumulatedFrameTime{0.0f};
        float timeGlobal{0.0f};
        float physicsTimeStep{1.0f / 240.0f};
        float initialTemperatureKelvin{300.0f};
        float tracerMassMultiplier{16.0f};
        float tracerRadiusMultiplier{3.0f};
        float currentPressureInstant{0.0f};
        float currentTemperatureKelvin{0.0f};
        float averageKineticEnergy{0.0f};
        float averageSpeed{0.0f};
        float rollingWallImpulse{0.0f};
        int particleCount{1000};
        int tracerParticleIndex{-1};
        unsigned int randomSeed{42u};
        bool tracerTrailEnabled{true};
        GasPresetId gasPresetId{GasPresetId::Air};
        GasSimulationMode mode{GasSimulationMode::IdealGas};
        GasSimulationState state{GasSimulationState::Idle};
    };
} // namespace physim

#endif // GAS_SIMULATION_H
