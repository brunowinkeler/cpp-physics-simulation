#include "Simulation.h"

#include <algorithm>
#include <array>
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
        if (state == SimulationState::Running || state == SimulationState::Landed)
        {
            return;
        }

        if (projectile.isLanded())
        {
            state = SimulationState::Landed;
            return;
        }

        if (state == SimulationState::Idle)
        {
            activeLaunchConfiguration = buildCurrentLaunchConfiguration();
            currentTrajectoryStyle = resolveTrajectoryStyle(*activeLaunchConfiguration);
            trajectoryRecorder.clear();
            timePerFrame = 0.0f;
            projectile.launch();
            trajectoryRecorder.record(projectile.getPosition().x, projectile.getPosition().y,
                                      timeGlobal,
                                      projectile.getCurrentSpeed());
        }

        state = SimulationState::Running;
    }

    void Simulation::stop()
    {
        if (state == SimulationState::Running)
        {
            state = projectile.isLanded() ? SimulationState::Landed : SimulationState::Paused;
        }
    }

    void Simulation::update(float timeStep)
    {
        if (state != SimulationState::Running || timeStep <= 0.0f)
        {
            return;
        }

        timePerFrame += timeStep;

        while (state == SimulationState::Running && timePerFrame >= timeStepPhysics)
        {
            projectile.update(timeStepPhysics, environment);
            timeGlobal += projectile.getLastUpdateDuration();
            timePerFrame -= timeStepPhysics;

            trajectoryRecorder.record(projectile.getPosition().x, projectile.getPosition().y,
                                      timeGlobal,
                                      projectile.getCurrentSpeed());

            if (projectile.isLanded())
            {
                state = SimulationState::Landed;
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
        state = SimulationState::Idle;
        activeLaunchConfiguration.reset();
    }

    void Simulation::clearLaunchHistory()
    {
        launchHistory.clear();
        nextHistoryEntryId = 1;
        nextTrajectoryStyleIndex = 0;
    }

    void Simulation::removeLaunchHistoryEntry(int launchId)
    {
        launchHistory.erase(
            std::remove_if(
                launchHistory.begin(),
                launchHistory.end(),
                [launchId](const LaunchHistoryEntry &launchHistoryEntry)
                {
                    return launchHistoryEntry.id == launchId;
                }),
            launchHistory.end());
    }

    void Simulation::setPhysicsTimeStep(float timeStep)
    {
        timeStepPhysics = std::max(timeStep, 0.0001f);
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
            state == SimulationState::Landed,
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
