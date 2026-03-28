#include "UiMenus.h"

#include "imgui.h"
#include "rlImGui.h"

namespace physim
{
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

        ImGui::Begin("Simulation Controls");

        ImGui::TextDisabled("Projectile Parameters");
        ImGui::InputFloat("Initial Velocity (m/s)", &projectile.getInitialSpeed(), 0.1f, 100.f, "%.1f");
        ImGui::InputFloat("Launch Angle (degrees)", &projectile.getLaunchAngle(), 0.1f, 90.f, "%.1f");
        ImGui::InputFloat("Mass (kg)", &projectile.getMass(), 0.1f, 10.f, "%.1f");
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
        ImGui::InputFloat("Time Scale", &environment.timeScale, 0.1f, 5.f, "%.1fx");
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

        rlImGuiEnd();
    }
}
