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
        ImGui::SliderFloat("Initial Velocity (m/s)", &projectile.getInitialSpeed(), 0.f, 100.f, "%.1f");
        ImGui::SliderFloat("Launch Angle (degrees)", &projectile.getLaunchAngle(), 0.f, 90.f, "%.1f");
        ImGui::SliderFloat("Mass (kg)", &projectile.getMass(), 1.f, 10.f, "%.1f");
        ImGui::Separator();
        ImGui::Spacing();
        ImGui::TextDisabled("Environment Parameters");
        ImGui::SliderFloat("Gravity (m/s^2)", &environment.gravity, 0.f, 30.f, "%.2f");
        ImGui::SliderFloat("Air Density (kg/m^3)", &environment.airDensity, 0.f, 2.0f, "%.2f");
        ImGui::Separator();
        ImGui::Spacing();
        ImGui::TextDisabled("Simulation Parameters");
        ImGui::SliderFloat("Time Scale", &environment.timeScale, 0.1f, 5.f, "%.1fx");
        ImGui::Separator();
        ImGui::Spacing();
        ImGui::TextDisabled("Controls");
        ImGui::Checkbox("Show ImGui Demo Window", &showDemoWindow); // For testing and demonstration purposes
        if (showDemoWindow)
        {
            ImGui::ShowDemoWindow();
        }
        if (ImGui::Button("Start Simulation"))
        {
            simulation.start();
        }
        ImGui::SameLine();
        if (ImGui::Button("Stop Simulation"))
        {
            simulation.stop();
        }
        ImGui::SameLine();
        if (ImGui::Button("Reset Simulation"))
        {
            simulation.reset();
        }

        ImGui::End();

        rlImGuiEnd();
    }
}
