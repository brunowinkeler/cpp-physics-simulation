#include "UiMenus.h"

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

    const char *integrationMethodLabel(physim::IntegrationMethod integrationMethod)
    {
        switch (integrationMethod)
        {
        case physim::IntegrationMethod::SymplecticEuler:
            return "Symplectic Euler";
        case physim::IntegrationMethod::RungeKutta4:
        default:
            return "Runge-Kutta 4";
        }
    }
}

namespace physim
{
    namespace
    {
        constexpr float MIN_PROJECTILE_MASS = 0.001f;
        constexpr float MIN_PROJECTILE_RADIUS = 0.001f;
        constexpr float MIN_TIME_SCALE = 0.1f;
    }

    UiMenus::UiMenus(Environment &env, Projectile &proj, Simulation &sim)
        : environment{env}, projectile{proj}, simulation{sim}
    {
    }

    void UiMenus::parametersSelectionScreen()
    {
        constexpr const char *integrationMethods[] = {
            "Symplectic Euler",
            "Runge-Kutta 4"};

        rlImGuiBegin();
        hoveredHistoryEntryId = -1;

        ImGui::Begin("Simulation Controls");

        ImGui::TextDisabled("Projectile Parameters");
        ImGui::InputFloat("Initial Velocity (m/s)", &projectile.getInitialSpeed(), 0.1f, 100.f, "%.1f");
        ImGui::InputFloat("Launch Angle (degrees)", &projectile.getLaunchAngle(), 0.1f, 90.f, "%.1f");
        ImGui::InputFloat("Mass (kg)", &projectile.getMass(), 0.01f, 0.1f, "%.3f");
        ImGui::InputFloat("Radius (m)", &projectile.getRadius(), 0.001f, 0.01f, "%.4f");
        ImGui::InputFloat("Drag Coefficient", &projectile.getDragCoefficient(), 0.01f, 0.1f, "%.3f");
        ImGui::Separator();
        ImGui::Spacing();
        ImGui::TextDisabled("Environment Parameters");
        ImGui::InputFloat("Gravity (m/s^2)", &environment.gravity, 0.1f, 30.f, "%.2f");
        ImGui::Checkbox("Enable Air Resistance", &environment.airResistanceEnabled);
        ImGui::BeginDisabled(!environment.airResistanceEnabled);
        ImGui::InputFloat("Air Density (kg/m^3)", &environment.airDensity, 0.01f, 2.0f, "%.2f");
        ImGui::EndDisabled();
        ImGui::Separator();
        ImGui::Spacing();
        ImGui::TextDisabled("Simulation Parameters");
        ImGui::InputFloat("Time Scale", &environment.timeScale, 0.001f, 5.f, "%.4fx");
        int integrationMethodIndex = static_cast<int>(projectile.getIntegrationMethod());
        if (ImGui::Combo("Integration Method", &integrationMethodIndex, integrationMethods, IM_ARRAYSIZE(integrationMethods)))
        {
            projectile.setIntegrationMethod(static_cast<IntegrationMethod>(integrationMethodIndex));
        }
        float physicsTimeStep = simulation.getPhysicsTimeStep();
        if (ImGui::InputFloat("Physics Time Step (s)", &physicsTimeStep, 0.0001f, 0.001f, "%.4f"))
        {
            simulation.setPhysicsTimeStep(physicsTimeStep);
        }

        projectile.getInitialSpeed() = std::max(projectile.getInitialSpeed(), 0.0f);
        projectile.getMass() = std::max(projectile.getMass(), MIN_PROJECTILE_MASS);
        projectile.getRadius() = std::max(projectile.getRadius(), MIN_PROJECTILE_RADIUS);
        projectile.getDragCoefficient() = std::max(projectile.getDragCoefficient(), 0.0f);
        environment.gravity = std::max(environment.gravity, 0.0f);
        environment.airDensity = std::max(environment.airDensity, 0.0f);
        environment.timeScale = std::max(environment.timeScale, MIN_TIME_SCALE);

        ImGui::Separator();
        ImGui::Spacing();
        ImGui::TextDisabled("Simulation Status");
        ImGui::Text("Simulation Time: %.3f s", simulation.getTimeGlobal());

        const float horizontalRange = projectile.getPosition().x;
        const std::optional<TrajectoryPoint> apexPoint = simulation.getApexPoint();

        if (projectile.isLanded())
        {
            ImGui::Text("Flight Time: %.3f s", simulation.getTimeGlobal());
            ImGui::Text("Final Range: %.3f m", horizontalRange);
        }
        else if (simulation.isRunning())
        {
            ImGui::Text("Flight Time: in progress");
            ImGui::Text("Current Range: %.3f m", horizontalRange);
        }
        else if (projectile.isLaunched())
        {
            ImGui::Text("Flight Time: paused at %.3f s", simulation.getTimeGlobal());
            ImGui::Text("Current Range: %.3f m", horizontalRange);
        }
        else
        {
            ImGui::Text("Flight Time: waiting for launch");
            ImGui::Text("Current Range: %.3f m", horizontalRange);
        }

        if (apexPoint.has_value())
        {
            ImGui::Text("Apex Height: %.3f m", apexPoint->y);
            ImGui::Text("Time To Apex: %.3f s", apexPoint->time);
        }
        else if (projectile.isLaunched())
        {
            ImGui::Text("Apex Height: waiting");
            ImGui::Text("Time To Apex: waiting");
        }

        ImGui::Separator();
        ImGui::Spacing();
        ImGui::TextDisabled("Controls");
        if (ImGui::Button("Start"))
        {
            simulation.start();
        }
        ImGui::SameLine();
        if (ImGui::Button("Stop"))
        {
            simulation.stop();
        }
        ImGui::SameLine();
        if (ImGui::Button("Reset"))
        {
            simulation.reset();
        }

        ImGui::End();

        drawLaunchHistoryWindow();

        rlImGuiEnd();
    }

    void UiMenus::drawLaunchHistoryWindow()
    {
        const auto &launchHistory = simulation.getLaunchHistory();

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

                ImGui::TreePop();
            }

            ImGui::PopID();
        }

        ImGui::End();
    }
}
