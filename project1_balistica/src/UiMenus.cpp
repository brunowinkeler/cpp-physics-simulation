#include "UiMenus.h"

#include "imgui.h"
#include "rlImGui.h"

namespace physim
{
    UiMenus::UiMenus(Environment &env, Projectile &proj)
        : environment{env}, projectile{proj}
    {
    }

    void UiMenus::parametersSelectionScreen()
    {
        rlImGuiBegin();

        ImGui::SetNextWindowPos({620, 20}, ImGuiCond_Once);
        ImGui::SetNextWindowSize({260, 180}, ImGuiCond_Once);

        ImGui::Begin("Simulation Controls");

        ImGui::TextDisabled("Projectile Parameters");
        ImGui::SliderFloat("Initial Velocity (m/s)", &projectile.getVelocity(), 0.f, 100.f, "%.1f");
        ImGui::SliderFloat("Launch Angle (degrees)", &projectile.getAngle(), 0.f, 90.f, "%.1f");
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

        ImGui::End();

        rlImGuiEnd();
    }
}
