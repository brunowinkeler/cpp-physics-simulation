#ifndef SIMULATION_H
#define SIMULATION_H

#include "physics/Projectile.h"
#include "physics/Environment.h"
#include "TrajectoryRecorder.h"

#include <optional>
#include <vector>

namespace physim
{
    struct TrajectoryStyle
    {
        unsigned char r;
        unsigned char g;
        unsigned char b;
        unsigned char a;
    };

    struct LaunchProjectileSnapshot
    {
        float initialSpeed;
        float launchAngle;
        float mass;
        float radius;
        float dragCoefficient;
    };

    struct LaunchEnvironmentSnapshot
    {
        float gravity;
        float airDensity;
        bool airResistanceEnabled;
    };

    struct LaunchSimulationSnapshot
    {
        IntegrationMethod integrationMethod;
        float physicsTimeStep;
    };

    struct LaunchConfiguration
    {
        LaunchProjectileSnapshot projectile;
        LaunchEnvironmentSnapshot environment;
        LaunchSimulationSnapshot simulation;
    };

    struct LaunchHistoryEntry
    {
        int id;
        LaunchConfiguration configuration;
        TrajectoryRecorder trajectory;
        float flightTime;
        float finalRange;
        bool landed;
        TrajectoryStyle style;
    };

    class Simulation
    {
    public:
        Simulation(Environment &environment, Projectile &projectile);

        void start();
        void stop();
        void update(float timeStep);
        void reset();

        bool isRunning() const { return running; }
        Projectile getProjectile() const;
        Environment getEnvironment() const;
        const std::vector<TrajectoryPoint> &getTrajectoryPoints() const { return trajectoryRecorder.getPoints(); }
        std::optional<TrajectoryPoint> getApexPoint() const { return trajectoryRecorder.getApexPoint(); }
        const std::vector<LaunchHistoryEntry> &getLaunchHistory() const { return launchHistory; }
        TrajectoryStyle getCurrentTrajectoryStyle() const { return currentTrajectoryStyle; }
        void clearLaunchHistory();
        void removeLaunchHistoryEntry(int launchId);

        float getTimeGlobal() const { return timeGlobal; }
        float getPhysicsTimeStep() const { return timeStepPhysics; }
        void setPhysicsTimeStep(float timeStep);

    private:
        void archiveCurrentLaunch();
        LaunchConfiguration buildCurrentLaunchConfiguration() const;
        TrajectoryStyle resolveTrajectoryStyle(const LaunchConfiguration &configuration);
        std::optional<std::size_t> findMatchingLaunchHistoryEntry(const LaunchConfiguration &configuration) const;

        Projectile &projectile;
        Environment &environment;
        TrajectoryRecorder trajectoryRecorder;
        std::vector<LaunchHistoryEntry> launchHistory;
        std::optional<LaunchConfiguration> activeLaunchConfiguration;

        float timePerFrame{0.0f};
        float timeGlobal{0.0f};
        float timeStepPhysics{0.001f};
        TrajectoryStyle currentTrajectoryStyle{255, 255, 255, 255};
        int nextHistoryEntryId{1};
        std::size_t nextTrajectoryStyleIndex{0};

        bool running{false};
    };
}

#endif // SIMULATION_H
