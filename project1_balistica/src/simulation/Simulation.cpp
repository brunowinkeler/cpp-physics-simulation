#include "Simulation.h"

#include "raylib.h"

#include <array>
#include <algorithm>
#include <cmath>

namespace physim
{
    namespace
    {
        constexpr std::array<TrajectoryStyle, 8> TRAJECTORY_STYLE_PALETTE{{
            {137, 220, 235, 220},
            {166, 227, 161, 220},
            {255, 179, 135, 220},
            {243, 139, 168, 220},
            {249, 226, 175, 220},
            {180, 190, 254, 220},
            {148, 226, 213, 220},
            {245, 194, 231, 220},
        }};

        constexpr float CONFIGURATION_EPSILON = 1.0e-4f;

        bool nearlyEqual(float left, float right, float epsilon = CONFIGURATION_EPSILON)
        {
            return std::fabs(left - right) <= epsilon;
        }

        bool matchesLaunchConfiguration(const LaunchConfiguration &left, const LaunchConfiguration &right)
        {
            return nearlyEqual(left.projectile.initialSpeed, right.projectile.initialSpeed) &&
                   nearlyEqual(left.projectile.launchAngle, right.projectile.launchAngle) &&
                   nearlyEqual(left.projectile.mass, right.projectile.mass) &&
                   nearlyEqual(left.projectile.radius, right.projectile.radius) &&
                   nearlyEqual(left.projectile.dragCoefficient, right.projectile.dragCoefficient) &&
                   nearlyEqual(left.environment.gravity, right.environment.gravity) &&
                   nearlyEqual(left.environment.airDensity, right.environment.airDensity) &&
                   (left.environment.airResistanceEnabled == right.environment.airResistanceEnabled) &&
                   (left.simulation.integrationMethod == right.simulation.integrationMethod) &&
                   nearlyEqual(left.simulation.physicsTimeStep, right.simulation.physicsTimeStep, 1.0e-6f);
        }
    }

    Simulation::Simulation(Environment &environment, Projectile &projectile)
        : projectile{projectile}, environment{environment} {}

    void Simulation::start()
    {
        if (projectile.isLanded())
        {
            return;
        }

        running = true;
        if (!projectile.isLaunched())
        {
            activeLaunchConfiguration = buildCurrentLaunchConfiguration();
            currentTrajectoryStyle = resolveTrajectoryStyle(*activeLaunchConfiguration);
            trajectoryRecorder.clear();
            projectile.launch();
            trajectoryRecorder.record(projectile.getPosition().x, projectile.getPosition().y,
                                      timeGlobal,
                                      projectile.getCurrentSpeed());
        }
    }

    void Simulation::stop()
    {
        running = false;
    }

    void Simulation::update(float timeStep)
    {
        if (!running || timeStep <= 0.0f)
        {
            return;
        }

        timePerFrame += timeStep;

        while (running && timePerFrame >= timeStepPhysics)
        {
            projectile.update(timeStepPhysics, environment);
            timeGlobal += projectile.getLastUpdateDuration();
            timePerFrame -= timeStepPhysics;

            trajectoryRecorder.record(projectile.getPosition().x, projectile.getPosition().y,
                                      timeGlobal,
                                      projectile.getCurrentSpeed());

            if (projectile.isLanded())
            {
                running = false;
                timePerFrame = 0.0f;
            }
        }
    }

    void Simulation::reset()
    {
        archiveCurrentLaunch();
        projectile.reset();
        trajectoryRecorder.clear();
        timePerFrame = 0.0f;
        timeGlobal = 0.0f;
        running = false;
        activeLaunchConfiguration.reset();
    }

    void Simulation::clearLaunchHistory()
    {
        launchHistory.clear();
        nextHistoryEntryId = 1;
        nextTrajectoryStyleIndex = 0;
    }

    void Simulation::setPhysicsTimeStep(float timeStep)
    {
        timeStepPhysics = std::max(timeStep, 0.0001f);
    }

    Projectile Simulation::getProjectile() const
    {
        return projectile;
    }

    Environment Simulation::getEnvironment() const
    {
        return environment;
    }

    void Simulation::archiveCurrentLaunch()
    {
        if (!activeLaunchConfiguration.has_value() || trajectoryRecorder.getPoints().size() < 2)
        {
            return;
        }

        if (findMatchingLaunchHistoryEntry(*activeLaunchConfiguration).has_value())
        {
            return;
        }

        launchHistory.push_back({
            nextHistoryEntryId++,
            *activeLaunchConfiguration,
            trajectoryRecorder,
            timeGlobal,
            trajectoryRecorder.getPoints().back().x,
            projectile.isLanded(),
            currentTrajectoryStyle,
        });
    }

    LaunchConfiguration Simulation::buildCurrentLaunchConfiguration() const
    {
        return {
            {
                projectile.getInitialSpeed(),
                projectile.getLaunchAngle(),
                projectile.getMass(),
                projectile.getRadius(),
                projectile.getDragCoefficient(),
            },
            {
                environment.gravity,
                environment.airDensity,
                environment.airResistanceEnabled,
            },
            {
                projectile.getIntegrationMethod(),
                timeStepPhysics,
            },
        };
    }

    TrajectoryStyle Simulation::resolveTrajectoryStyle(const LaunchConfiguration &configuration)
    {
        const std::optional<std::size_t> historyEntryIndex = findMatchingLaunchHistoryEntry(configuration);
        if (historyEntryIndex.has_value())
        {
            return launchHistory[*historyEntryIndex].style;
        }

        const TrajectoryStyle style = TRAJECTORY_STYLE_PALETTE[nextTrajectoryStyleIndex % TRAJECTORY_STYLE_PALETTE.size()];
        ++nextTrajectoryStyleIndex;
        return style;
    }

    std::optional<std::size_t> Simulation::findMatchingLaunchHistoryEntry(const LaunchConfiguration &configuration) const
    {
        for (std::size_t index = 0; index < launchHistory.size(); ++index)
        {
            if (matchesLaunchConfiguration(launchHistory[index].configuration, configuration))
            {
                return index;
            }
        }

        return std::nullopt;
    }
}
