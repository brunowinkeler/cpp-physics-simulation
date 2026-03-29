#include "UiMenus.h"

#include "simulation/SimulationSession.h"

#include "imgui.h"
#include "rlImGui.h"

#include <algorithm>

namespace
{
    ImVec4 toImGuiColor(const physim::TrajectoryStyle &style)
    {
        return {
            style.r / 255.0f,
            style.g / 255.0f,
            style.b / 255.0f,
            style.a / 255.0f,
        };
    }

    const char *simulationStateLabel(physim::SimulationState simulationState)
    {
        switch (simulationState)
        {
        case physim::SimulationState::Idle:
            return "Idle";
        case physim::SimulationState::Running:
            return "Running";
        case physim::SimulationState::Paused:
            return "Paused";
        case physim::SimulationState::Landed:
            return "Landed";
        default:
            return "Unknown";
        }
    }
}

namespace physim
{
    namespace
    {
        constexpr float MIN_PROJECTILE_MASS = 0.001f;
        constexpr float MAX_PROJECTILE_MASS = 100.0f;
        constexpr float MIN_PROJECTILE_RADIUS = 0.001f;
        constexpr float MAX_PROJECTILE_RADIUS = 1.0f;
        constexpr float MAX_INITIAL_SPEED = 500.0f;
        constexpr float MIN_LAUNCH_ANGLE = 0.0f;
        constexpr float MAX_LAUNCH_ANGLE = 89.9f;
        constexpr float MAX_DRAG_COEFFICIENT = 3.0f;
        constexpr float MAX_GRAVITY = 50.0f;
        constexpr float MAX_AIR_DENSITY = 5.0f;
        constexpr float MIN_TIME_SCALE = 0.1f;
        constexpr float MAX_TIME_SCALE = 20.0f;
        constexpr float MIN_PHYSICS_TIME_STEP = 0.0001f;
        constexpr float MAX_PHYSICS_TIME_STEP = 0.1f;
    }

    UiMenus::UiMenus(SimulationSession &session)
        : session{session}
    {
    }

    void UiMenus::parametersSelectionScreen()
    {
        Environment &environment = session.getEnvironment();
        Projectile &projectile = session.getProjectile();
        Simulation &simulation = session.getSimulation();

        const char *integrationMethods[] = {
            integrationMethodLabel(IntegrationMethod::SymplecticEuler),
            integrationMethodLabel(IntegrationMethod::RungeKutta4)};

        rlImGuiBegin();
        hoveredHistoryEntryId = -1;

        ImGui::Begin("Simulation Controls");

        const SimulationState simulationState = simulation.getState();
        const bool isIdle = simulationState == SimulationState::Idle;
        const bool dragTrajectoryControlsEnabled = isIdle && environment.airResistanceEnabled;

        ImGui::TextDisabled("Projectile Parameters");
        ImGui::BeginDisabled(!isIdle);
        ImGui::InputFloat("Initial Velocity (m/s)", &projectile.getInitialSpeed(), 0.1f, 100.f, "%.1f");
        ImGui::InputFloat("Launch Angle (degrees)", &projectile.getLaunchAngle(), 0.1f, 90.f, "%.1f");
        ImGui::InputFloat("Radius (m)", &projectile.getRadius(), 0.001f, 0.01f, "%.4f");
        ImGui::EndDisabled();

        ImGui::Separator();
        ImGui::Spacing();
        ImGui::TextDisabled("Drag Model");
        ImGui::BeginDisabled(!isIdle);
        ImGui::Checkbox("Enable Air Resistance", &environment.airResistanceEnabled);
        ImGui::EndDisabled();
        ImGui::BeginDisabled(!dragTrajectoryControlsEnabled);
        ImGui::InputFloat("Mass (kg)", &projectile.getMass(), 0.01f, 0.1f, "%.3f");
        ImGui::InputFloat("Drag Coefficient", &projectile.getDragCoefficient(), 0.01f, 0.1f, "%.3f");
        ImGui::InputFloat("Air Density (kg/m^3)", &environment.airDensity, 0.01f, 2.0f, "%.2f");
        ImGui::EndDisabled();
        if (!environment.airResistanceEnabled)
        {
            ImGui::TextDisabled("Mass, drag coefficient and air density affect the flight only when air resistance is enabled.");
        }

        ImGui::Separator();
        ImGui::Spacing();
        ImGui::TextDisabled("Environment Parameters");
        ImGui::BeginDisabled(!isIdle);
        ImGui::InputFloat("Gravity (m/s^2)", &environment.gravity, 0.1f, 30.f, "%.2f");
        ImGui::EndDisabled();

        ImGui::Separator();
        ImGui::Spacing();
        ImGui::TextDisabled("Simulation Parameters");
        ImGui::InputFloat("Time Scale", &environment.timeScale, 0.001f, 5.f, "%.4fx");
        ImGui::BeginDisabled(!isIdle);
        int integrationMethodIndex = static_cast<int>(projectile.getIntegrationMethod());
        if (ImGui::Combo("Integration Method", &integrationMethodIndex, integrationMethods, IM_ARRAYSIZE(integrationMethods)))
        {
            projectile.setIntegrationMethod(static_cast<IntegrationMethod>(integrationMethodIndex));
        }
        float physicsTimeStep = simulation.getPhysicsTimeStep();
        if (ImGui::InputFloat("Physics Time Step (s)", &physicsTimeStep, 0.0001f, 0.001f, "%.4f"))
        {
            simulation.setPhysicsTimeStep(std::clamp(physicsTimeStep, MIN_PHYSICS_TIME_STEP, MAX_PHYSICS_TIME_STEP));
        }
        ImGui::EndDisabled();

        projectile.getInitialSpeed() = std::clamp(projectile.getInitialSpeed(), 0.0f, MAX_INITIAL_SPEED);
        projectile.getLaunchAngle() = std::clamp(projectile.getLaunchAngle(), MIN_LAUNCH_ANGLE, MAX_LAUNCH_ANGLE);
        projectile.getMass() = std::clamp(projectile.getMass(), MIN_PROJECTILE_MASS, MAX_PROJECTILE_MASS);
        projectile.getRadius() = std::clamp(projectile.getRadius(), MIN_PROJECTILE_RADIUS, MAX_PROJECTILE_RADIUS);
        projectile.getDragCoefficient() = std::clamp(projectile.getDragCoefficient(), 0.0f, MAX_DRAG_COEFFICIENT);
        environment.gravity = std::clamp(environment.gravity, 0.0f, MAX_GRAVITY);
        environment.airDensity = std::clamp(environment.airDensity, 0.0f, MAX_AIR_DENSITY);
        environment.timeScale = std::clamp(environment.timeScale, MIN_TIME_SCALE, MAX_TIME_SCALE);

        ImGui::Separator();
        ImGui::Spacing();
        ImGui::TextDisabled("Simulation Status");
        ImGui::Text("State: %s", simulationStateLabel(simulationState));
        ImGui::Text("Simulation Time: %.3f s", simulation.getTimeGlobal());

        const float horizontalRange = projectile.getPosition().x;
        const std::optional<TrajectoryPoint> apexPoint = simulation.getApexPoint();

        switch (simulationState)
        {
        case SimulationState::Landed:
            ImGui::Text("Flight Time: %.3f s", simulation.getTimeGlobal());
            ImGui::Text("Final Range: %.3f m", horizontalRange);
            break;
        case SimulationState::Running:
            ImGui::Text("Flight Time: in progress");
            ImGui::Text("Current Range: %.3f m", horizontalRange);
            break;
        case SimulationState::Paused:
            ImGui::Text("Flight Time: paused at %.3f s", simulation.getTimeGlobal());
            ImGui::Text("Current Range: %.3f m", horizontalRange);
            break;
        case SimulationState::Idle:
        default:
            ImGui::Text("Flight Time: waiting for launch");
            ImGui::Text("Current Range: %.3f m", horizontalRange);
            break;
        }

        if (apexPoint.has_value())
        {
            ImGui::Text("Apex Height: %.3f m", apexPoint->y);
            ImGui::Text("Time To Apex: %.3f s", apexPoint->time);
        }
        else if (simulationState == SimulationState::Running || simulationState == SimulationState::Paused)
        {
            ImGui::Text("Apex Height: waiting");
            ImGui::Text("Time To Apex: waiting");
        }

        ImGui::Separator();
        ImGui::Spacing();
        ImGui::TextDisabled("Controls");
        ImGui::BeginDisabled(simulationState == SimulationState::Running || simulationState == SimulationState::Landed);
        if (ImGui::Button(simulationState == SimulationState::Paused ? "Resume" : "Start"))
        {
            simulation.start();
        }
        ImGui::EndDisabled();
        ImGui::SameLine();
        ImGui::BeginDisabled(simulationState != SimulationState::Running);
        if (ImGui::Button("Stop"))
        {
            simulation.stop();
        }
        ImGui::EndDisabled();
        ImGui::SameLine();
        if (ImGui::Button("Reset"))
        {
            simulation.reset();
        }

        if (simulationState != SimulationState::Idle)
        {
            ImGui::TextDisabled("Reset to edit launch parameters.");
        }

        ImGui::End();

        drawLaunchHistoryWindow();

        rlImGuiEnd();
    }

    void UiMenus::drawLaunchHistoryWindow()
    {
        Simulation &simulation = session.getSimulation();
        const auto &launchHistory = simulation.getLaunchHistory();
        std::optional<int> pendingDeletedLaunchId;

        ImGui::Begin("Launch History");

        if (ImGui::Button("Clear History"))
        {
            simulation.clearLaunchHistory();
            selectedHistoryEntryId = -1;
            hoveredHistoryEntryId = -1;
        }

        if (launchHistory.empty())
        {
            ImGui::Spacing();
            ImGui::TextDisabled("No archived launches yet.");
            ImGui::End();
            return;
        }

        ImGui::Separator();
        ImGui::Spacing();

        for (const LaunchHistoryEntry &launchHistoryEntry : launchHistory)
        {
            ImGui::PushID(launchHistoryEntry.id);

            const bool isSelected = selectedHistoryEntryId == launchHistoryEntry.id;
            ImGui::ColorButton("##trajectory-color",
                               toImGuiColor(launchHistoryEntry.style),
                               ImGuiColorEditFlags_NoTooltip,
                               {12.0f, 12.0f});
            if (ImGui::IsItemHovered())
            {
                hoveredHistoryEntryId = launchHistoryEntry.id;
            }

            ImGui::SameLine();
            ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_OpenOnArrow;
            if (isSelected)
            {
                treeNodeFlags |= ImGuiTreeNodeFlags_Selected;
            }

            const bool isOpen = ImGui::TreeNodeEx(
                "##launch-history-entry",
                treeNodeFlags,
                "Launch %d | %s | range %.2f m | time %.2f s",
                launchHistoryEntry.id,
                launchHistoryEntry.landed ? "landed" : "reset",
                launchHistoryEntry.finalRange,
                launchHistoryEntry.flightTime);

            if (ImGui::IsItemClicked())
            {
                selectedHistoryEntryId = launchHistoryEntry.id;
            }

            if (ImGui::IsItemHovered())
            {
                hoveredHistoryEntryId = launchHistoryEntry.id;
            }

            if (isOpen)
            {
                ImGui::TextDisabled("Projectile");
                ImGui::BulletText("Speed: %.3f m/s", launchHistoryEntry.configuration.projectile.initialSpeed);
                ImGui::BulletText("Angle: %.3f deg", launchHistoryEntry.configuration.projectile.launchAngle);
                ImGui::BulletText("Mass: %.3f kg", launchHistoryEntry.configuration.projectile.mass);
                ImGui::BulletText("Radius: %.4f m", launchHistoryEntry.configuration.projectile.radius);
                ImGui::BulletText("Cd: %.3f", launchHistoryEntry.configuration.projectile.dragCoefficient);

                ImGui::TextDisabled("Environment");
                ImGui::BulletText("Gravity: %.3f m/s^2", launchHistoryEntry.configuration.environment.gravity);
                ImGui::BulletText("Air Resistance: %s", launchHistoryEntry.configuration.environment.airResistanceEnabled ? "enabled" : "disabled");
                ImGui::BulletText("Air Density: %.3f kg/m^3", launchHistoryEntry.configuration.environment.airDensity);

                ImGui::TextDisabled("Simulation");
                ImGui::BulletText("Method: %s", integrationMethodLabel(launchHistoryEntry.configuration.simulation.integrationMethod));
                ImGui::BulletText("Physics dt: %.4f s", launchHistoryEntry.configuration.simulation.physicsTimeStep);

                if (launchHistoryEntry.trajectory.getApexPoint().has_value())
                {
                    const TrajectoryPoint apexPoint = *launchHistoryEntry.trajectory.getApexPoint();
                    ImGui::TextDisabled("Apex");
                    ImGui::BulletText("Height: %.3f m", apexPoint.y);
                    ImGui::BulletText("Time: %.3f s", apexPoint.time);
                }

                if (ImGui::SmallButton("Delete This Launch"))
                {
                    pendingDeletedLaunchId = launchHistoryEntry.id;
                }

                ImGui::TreePop();
            }

            ImGui::PopID();
        }

        if (pendingDeletedLaunchId.has_value())
        {
            simulation.removeLaunchHistoryEntry(*pendingDeletedLaunchId);

            if (selectedHistoryEntryId == *pendingDeletedLaunchId)
            {
                selectedHistoryEntryId = -1;
            }

            if (hoveredHistoryEntryId == *pendingDeletedLaunchId)
            {
                hoveredHistoryEntryId = -1;
            }
        }

        ImGui::End();
    }
}
