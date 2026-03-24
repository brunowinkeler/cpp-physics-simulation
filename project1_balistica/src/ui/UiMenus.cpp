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
        ImGui::InputFloat("Air Density (kg/m^3)", &environment.airDensity, 0.01f, 2.0f, "%.2f");
        ImGui::Separator();
        ImGui::Spacing();
        ImGui::TextDisabled("Simulation Parameters");
        ImGui::InputFloat("Time Scale", &environment.timeScale, 0.1f, 5.f, "%.1fx");
        ImGui::Separator();
        ImGui::Spacing();
        ImGui::TextDisabled("Controls");
        ImGui::Checkbox("Show ImGui Demo Window", &showDemoWindow); // For testing and demonstration purposes
        if (showDemoWindow)
        {
            ImGui::ShowDemoWindow();
        }
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
