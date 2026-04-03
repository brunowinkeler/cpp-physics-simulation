#include "PendulumUiMenus.h"

#include "simulation/PendulumSession.h"

#include "imgui.h"
#include "rlImGui.h"

#include <algorithm>
#include <cmath>
#include <numbers>

namespace
{
    const char *simulationStateLabel(physim::PendulumSimulationState state)
    {
        switch (state)
        {
        case physim::PendulumSimulationState::Running:
            return "Running";
        case physim::PendulumSimulationState::Paused:
            return "Paused";
        case physim::PendulumSimulationState::Idle:
        default:
            return "Idle";
        }
    }
}

namespace physim
{
    namespace
    {
        constexpr float MIN_GRAVITY = 0.0f;
        constexpr float MAX_GRAVITY = 50.0f;
        constexpr float MIN_TIME_SCALE = 0.05f;
        constexpr float MAX_TIME_SCALE = 20.0f;
        constexpr float MIN_PHYSICS_TIME_STEP = 0.0001f;
        constexpr float MAX_PHYSICS_TIME_STEP = 0.05f;
        constexpr float MIN_LENGTH = 0.1f;
        constexpr float MAX_LENGTH = 5.0f;
        constexpr float MIN_MASS = 0.05f;
        constexpr float MAX_MASS = 25.0f;
        constexpr float MIN_DAMPING = 0.0f;
        constexpr float MAX_DAMPING = 2.0f;
        constexpr float MIN_INITIAL_ANGLE = -179.0f;
        constexpr float MAX_INITIAL_ANGLE = 179.0f;
        constexpr float MIN_INITIAL_ANGULAR_VELOCITY = -20.0f;
        constexpr float MAX_INITIAL_ANGULAR_VELOCITY = 20.0f;
    }

    PendulumUiMenus::PendulumUiMenus(PendulumSession &session)
        : session{session}
    {
    }

    void PendulumUiMenus::render()
    {
        const PendulumSimulation &simulation = session.getSimulation();
        const bool editableInitialConditions = simulation.isIdle();

        rlImGuiBegin();

        ImGui::Begin("Pendulum Lab Controls");
        drawModeSection(editableInitialConditions);
        drawEnvironmentSection(editableInitialConditions);
        drawVisualizationSection();

        ImGui::Separator();
        ImGui::Spacing();

        if (simulation.isSimpleMode())
        {
            drawSimplePendulumSection(editableInitialConditions);
        }
        else
        {
            drawDoublePendulumSection(editableInitialConditions);
        }

        drawStatusSection();
        drawControlSection();

        ImGui::End();
        rlImGuiEnd();
    }

    void PendulumUiMenus::drawModeSection(bool editableInitialConditions)
    {
        PendulumSimulation &simulation = session.getSimulation();

        const char *modeLabels[] = {
            "Simple Pendulum",
            "Double Pendulum",
        };

        ImGui::TextDisabled("Model");
        ImGui::BeginDisabled(!editableInitialConditions);
        int modeIndex = static_cast<int>(simulation.getMode());
        if (ImGui::Combo("Active Model", &modeIndex, modeLabels, IM_ARRAYSIZE(modeLabels)))
        {
            simulation.setMode(static_cast<PendulumMode>(modeIndex));
        }
        ImGui::EndDisabled();
    }

    void PendulumUiMenus::drawEnvironmentSection(bool editableInitialConditions)
    {
        PendulumEnvironment &environment = session.getEnvironment();
        PendulumSimulation &simulation = session.getSimulation();

        ImGui::Separator();
        ImGui::Spacing();
        ImGui::TextDisabled("World And Integration");

        ImGui::BeginDisabled(!editableInitialConditions);
        ImGui::InputFloat("Gravity (m/s^2)", &environment.gravity, 0.1f, 1.0f, "%.3f");
        ImGui::EndDisabled();

        ImGui::InputFloat("Time Scale", &environment.timeScale, 0.05f, 0.5f, "%.3fx");

        ImGui::BeginDisabled(!editableInitialConditions);
        float physicsTimeStep = simulation.getPhysicsTimeStep();
        if (ImGui::InputFloat("Physics Time Step (s)", &physicsTimeStep, 0.0001f, 0.001f, "%.4f"))
        {
            simulation.setPhysicsTimeStep(std::clamp(physicsTimeStep, MIN_PHYSICS_TIME_STEP, MAX_PHYSICS_TIME_STEP));
        }

        const char *integrationMethods[] = {
            integrationMethodLabel(IntegrationMethod::SymplecticEuler),
            integrationMethodLabel(IntegrationMethod::RungeKutta4),
        };
        int integrationMethodIndex = simulation.isSimpleMode()
                                         ? static_cast<int>(session.getSimplePendulum().getIntegrationMethod())
                                         : static_cast<int>(session.getDoublePendulum().getIntegrationMethod());

        if (ImGui::Combo("Integration Method", &integrationMethodIndex, integrationMethods, IM_ARRAYSIZE(integrationMethods)))
        {
            if (simulation.isSimpleMode())
            {
                session.getSimplePendulum().setIntegrationMethod(static_cast<IntegrationMethod>(integrationMethodIndex));
            }
            else
            {
                session.getDoublePendulum().setIntegrationMethod(static_cast<IntegrationMethod>(integrationMethodIndex));
            }
        }
        ImGui::EndDisabled();

        environment.gravity = std::clamp(environment.gravity, MIN_GRAVITY, MAX_GRAVITY);
        environment.timeScale = std::clamp(environment.timeScale, MIN_TIME_SCALE, MAX_TIME_SCALE);
    }

    void PendulumUiMenus::drawVisualizationSection()
    {
        PendulumVisualizationSettings &visualizationSettings = session.getVisualizationSettings();

        ImGui::Separator();
        ImGui::Spacing();
        ImGui::TextDisabled("Visualization");
        ImGui::Checkbox("Show Grid", &visualizationSettings.showGrid);
        ImGui::Checkbox("Show Axes", &visualizationSettings.showAxes);
        ImGui::Checkbox("Show Primary Trail", &visualizationSettings.showPrimaryTrail);
        ImGui::BeginDisabled(session.getSimulation().isSimpleMode());
        ImGui::Checkbox("Show Secondary Trail", &visualizationSettings.showSecondaryTrail);
        ImGui::EndDisabled();
    }

    void PendulumUiMenus::drawSimplePendulumSection(bool editableInitialConditions)
    {
        SimplePendulum &simplePendulum = session.getSimplePendulum();

        ImGui::TextDisabled("Simple Pendulum Parameters");
        ImGui::BeginDisabled(!editableInitialConditions);
        ImGui::InputFloat("Length (m)", &simplePendulum.getLength(), 0.05f, 0.25f, "%.3f");
        ImGui::InputFloat("Mass (kg)", &simplePendulum.getMass(), 0.05f, 0.25f, "%.3f");
        ImGui::InputFloat("Damping", &simplePendulum.getDamping(), 0.001f, 0.01f, "%.4f");
        ImGui::InputFloat("Initial Angle (deg)", &simplePendulum.getInitialAngleDegrees(), 0.5f, 5.0f, "%.2f");
        ImGui::InputFloat("Initial Angular Velocity (rad/s)", &simplePendulum.getInitialAngularVelocity(), 0.05f, 0.5f, "%.3f");
        ImGui::EndDisabled();

        simplePendulum.getLength() = std::clamp(simplePendulum.getLength(), MIN_LENGTH, MAX_LENGTH);
        simplePendulum.getMass() = std::clamp(simplePendulum.getMass(), MIN_MASS, MAX_MASS);
        simplePendulum.getDamping() = std::clamp(simplePendulum.getDamping(), MIN_DAMPING, MAX_DAMPING);
        simplePendulum.getInitialAngleDegrees() = std::clamp(simplePendulum.getInitialAngleDegrees(), MIN_INITIAL_ANGLE, MAX_INITIAL_ANGLE);
        simplePendulum.getInitialAngularVelocity() = std::clamp(simplePendulum.getInitialAngularVelocity(), MIN_INITIAL_ANGULAR_VELOCITY, MAX_INITIAL_ANGULAR_VELOCITY);

        ImGui::TextDisabled("Small-angle period approximation: %.3f s",
                            (2.0f * static_cast<float>(std::numbers::pi_v<double>) * std::sqrt(simplePendulum.getLength() / std::max(session.getEnvironment().gravity, 0.01f))));
    }

    void PendulumUiMenus::drawDoublePendulumSection(bool editableInitialConditions)
    {
        DoublePendulum &doublePendulum = session.getDoublePendulum();

        ImGui::TextDisabled("Double Pendulum Parameters");
        ImGui::BeginDisabled(!editableInitialConditions);
        ImGui::InputFloat("Length 1 (m)", &doublePendulum.getLength1(), 0.05f, 0.25f, "%.3f");
        ImGui::InputFloat("Length 2 (m)", &doublePendulum.getLength2(), 0.05f, 0.25f, "%.3f");
        ImGui::InputFloat("Mass 1 (kg)", &doublePendulum.getMass1(), 0.05f, 0.25f, "%.3f");
        ImGui::InputFloat("Mass 2 (kg)", &doublePendulum.getMass2(), 0.05f, 0.25f, "%.3f");
        ImGui::InputFloat("Damping 1", &doublePendulum.getDamping1(), 0.0005f, 0.005f, "%.4f");
        ImGui::InputFloat("Damping 2", &doublePendulum.getDamping2(), 0.0005f, 0.005f, "%.4f");
        ImGui::InputFloat("Initial Angle 1 (deg)", &doublePendulum.getInitialAngle1Degrees(), 0.5f, 5.0f, "%.2f");
        ImGui::InputFloat("Initial Angle 2 (deg)", &doublePendulum.getInitialAngle2Degrees(), 0.5f, 5.0f, "%.2f");
        ImGui::InputFloat("Initial Angular Velocity 1 (rad/s)", &doublePendulum.getInitialAngularVelocity1(), 0.05f, 0.5f, "%.3f");
        ImGui::InputFloat("Initial Angular Velocity 2 (rad/s)", &doublePendulum.getInitialAngularVelocity2(), 0.05f, 0.5f, "%.3f");
        ImGui::EndDisabled();

        doublePendulum.getLength1() = std::clamp(doublePendulum.getLength1(), MIN_LENGTH, MAX_LENGTH);
        doublePendulum.getLength2() = std::clamp(doublePendulum.getLength2(), MIN_LENGTH, MAX_LENGTH);
        doublePendulum.getMass1() = std::clamp(doublePendulum.getMass1(), MIN_MASS, MAX_MASS);
        doublePendulum.getMass2() = std::clamp(doublePendulum.getMass2(), MIN_MASS, MAX_MASS);
        doublePendulum.getDamping1() = std::clamp(doublePendulum.getDamping1(), MIN_DAMPING, MAX_DAMPING);
        doublePendulum.getDamping2() = std::clamp(doublePendulum.getDamping2(), MIN_DAMPING, MAX_DAMPING);
        doublePendulum.getInitialAngle1Degrees() = std::clamp(doublePendulum.getInitialAngle1Degrees(), MIN_INITIAL_ANGLE, MAX_INITIAL_ANGLE);
        doublePendulum.getInitialAngle2Degrees() = std::clamp(doublePendulum.getInitialAngle2Degrees(), MIN_INITIAL_ANGLE, MAX_INITIAL_ANGLE);
        doublePendulum.getInitialAngularVelocity1() = std::clamp(doublePendulum.getInitialAngularVelocity1(), MIN_INITIAL_ANGULAR_VELOCITY, MAX_INITIAL_ANGULAR_VELOCITY);
        doublePendulum.getInitialAngularVelocity2() = std::clamp(doublePendulum.getInitialAngularVelocity2(), MIN_INITIAL_ANGULAR_VELOCITY, MAX_INITIAL_ANGULAR_VELOCITY);

        ImGui::TextDisabled("Small changes in initial conditions can diverge quickly in double mode.");
    }

    void PendulumUiMenus::drawStatusSection() const
    {
        const PendulumSimulation &simulation = session.getSimulation();

        ImGui::Separator();
        ImGui::Spacing();
        ImGui::TextDisabled("Simulation Status");
        ImGui::Text("State: %s", simulationStateLabel(simulation.getState()));
        ImGui::Text("Simulation Time: %.3f s", simulation.getTimeGlobal());
        ImGui::Text("Total Energy: %.4f J", simulation.getCurrentTotalEnergy());
        ImGui::Text("Energy Delta: %+0.4f J", simulation.getEnergyDelta());

        if (simulation.isSimpleMode())
        {
            const SimplePendulum &simplePendulum = session.getSimplePendulum();
            const PendulumBob bob = simplePendulum.getBob();
            ImGui::Text("Angle: %.3f deg", simplePendulum.getAngleDegrees());
            ImGui::Text("Angular Velocity: %.3f rad/s", simplePendulum.getAngularVelocity());
            ImGui::Text("Tip Speed: %.3f m/s", bob.speed);
        }
        else
        {
            const DoublePendulum &doublePendulum = session.getDoublePendulum();
            const DoublePendulumBobs bobs = doublePendulum.getBobs();
            ImGui::Text("Theta1: %.3f deg", doublePendulum.getAngle1Degrees());
            ImGui::Text("Theta2: %.3f deg", doublePendulum.getAngle2Degrees());
            ImGui::Text("Omega1: %.3f rad/s", doublePendulum.getAngularVelocity1());
            ImGui::Text("Omega2: %.3f rad/s", doublePendulum.getAngularVelocity2());
            ImGui::Text("Bob 1 Speed: %.3f m/s", bobs.first.speed);
            ImGui::Text("Bob 2 Speed: %.3f m/s", bobs.second.speed);
        }
    }

    void PendulumUiMenus::drawControlSection()
    {
        PendulumSimulation &simulation = session.getSimulation();

        ImGui::Separator();
        ImGui::Spacing();
        ImGui::TextDisabled("Controls");

        ImGui::BeginDisabled(simulation.isRunning());
        if (ImGui::Button(simulation.isPaused() ? "Resume" : "Start"))
        {
            simulation.start();
        }
        ImGui::EndDisabled();

        ImGui::SameLine();
        ImGui::BeginDisabled(!simulation.isRunning());
        if (ImGui::Button("Pause"))
        {
            simulation.stop();
        }
        ImGui::EndDisabled();

        ImGui::SameLine();
        if (ImGui::Button("Reset"))
        {
            simulation.reset();
        }

        if (!simulation.isIdle())
        {
            ImGui::TextDisabled("Reset to apply edits to initial conditions.");
        }
    }
} // namespace physim
