#include "PendulumUiMenus.h"

#include "simulation/PendulumSession.h"

#include "imgui.h"
#include "rlImGui.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <numbers>

namespace
{
    constexpr ImVec2 ANALYSIS_PLOT_SIZE{0.0f, 150.0f};
    constexpr std::size_t MAX_RENDERED_ANALYSIS_POINTS = 512;
    constexpr int ANALYSIS_GRID_SUBDIVISIONS = 4;
    constexpr ImU32 ANALYSIS_BACKGROUND_COLOR = IM_COL32(18, 18, 28, 220);
    constexpr ImU32 ANALYSIS_BORDER_COLOR = IM_COL32(145, 150, 170, 255);
    constexpr ImU32 ANALYSIS_GRID_COLOR = IM_COL32(80, 82, 95, 180);
    constexpr ImU32 ANALYSIS_AXIS_COLOR = IM_COL32(210, 214, 230, 220);
    constexpr ImU32 PRIMARY_PHASE_COLOR = IM_COL32(137, 220, 235, 255);
    constexpr ImU32 SECONDARY_PHASE_COLOR = IM_COL32(255, 179, 135, 255);

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

    bool isFiniteScalarHistorySample(const physim::ScalarHistorySample &sample)
    {
        return std::isfinite(sample.time) && std::isfinite(sample.value);
    }

    bool isFinitePhaseSpaceSample(const physim::PhaseSpaceSample &sample)
    {
        return std::isfinite(sample.time) && std::isfinite(sample.angle) && std::isfinite(sample.angularVelocity);
    }

    std::size_t computeRenderedPointCount(std::size_t sampleCount, float availableWidth)
    {
        if (sampleCount <= 1)
        {
            return sampleCount;
        }

        const std::size_t widthBudget = static_cast<std::size_t>(std::max(2.0f, std::ceil(availableWidth)));
        return std::min(sampleCount, std::min(widthBudget, MAX_RENDERED_ANALYSIS_POINTS));
    }

    std::size_t mapRenderedSampleIndex(std::size_t sampleCount, std::size_t renderedPointCount, std::size_t renderedIndex)
    {
        if (sampleCount <= 1 || renderedPointCount <= 1)
        {
            return 0;
        }

        return (renderedIndex * (sampleCount - 1)) / (renderedPointCount - 1);
    }

    struct PhasePlotBounds
    {
        float minAngle{std::numeric_limits<float>::max()};
        float maxAngle{std::numeric_limits<float>::lowest()};
        float minAngularVelocity{std::numeric_limits<float>::max()};
        float maxAngularVelocity{std::numeric_limits<float>::lowest()};
    };

    void expandPhaseBounds(const std::vector<physim::PhaseSpaceSample> &samples, PhasePlotBounds &bounds)
    {
        for (const physim::PhaseSpaceSample &sample : samples)
        {
            if (!isFinitePhaseSpaceSample(sample))
            {
                continue;
            }

            bounds.minAngle = std::min(bounds.minAngle, sample.angle);
            bounds.maxAngle = std::max(bounds.maxAngle, sample.angle);
            bounds.minAngularVelocity = std::min(bounds.minAngularVelocity, sample.angularVelocity);
            bounds.maxAngularVelocity = std::max(bounds.maxAngularVelocity, sample.angularVelocity);
        }
    }

    PhasePlotBounds normalizePhaseBounds(PhasePlotBounds bounds)
    {
        if (!std::isfinite(bounds.minAngle) || !std::isfinite(bounds.maxAngle))
        {
            return {-1.0f, 1.0f, -1.0f, 1.0f};
        }

        if ((bounds.maxAngle - bounds.minAngle) < 1.0e-4f)
        {
            bounds.minAngle -= 1.0f;
            bounds.maxAngle += 1.0f;
        }

        if ((bounds.maxAngularVelocity - bounds.minAngularVelocity) < 1.0e-4f)
        {
            bounds.minAngularVelocity -= 1.0f;
            bounds.maxAngularVelocity += 1.0f;
        }

        const float anglePadding = 0.08f * (bounds.maxAngle - bounds.minAngle);
        const float velocityPadding = 0.08f * (bounds.maxAngularVelocity - bounds.minAngularVelocity);

        bounds.minAngle -= anglePadding;
        bounds.maxAngle += anglePadding;
        bounds.minAngularVelocity -= velocityPadding;
        bounds.maxAngularVelocity += velocityPadding;

        return bounds;
    }

    ImVec2 mapPhaseSpaceSample(const physim::PhaseSpaceSample &sample,
                               const PhasePlotBounds &bounds,
                               const ImVec2 &topLeft,
                               const ImVec2 &size)
    {
        const float normalizedX = (sample.angle - bounds.minAngle) / (bounds.maxAngle - bounds.minAngle);
        const float normalizedY = (sample.angularVelocity - bounds.minAngularVelocity) / (bounds.maxAngularVelocity - bounds.minAngularVelocity);

        return {
            topLeft.x + (normalizedX * size.x),
            topLeft.y + size.y - (normalizedY * size.y),
        };
    }

    void drawPhaseTrace(ImDrawList *drawList,
                        const std::vector<physim::PhaseSpaceSample> &samples,
                        const PhasePlotBounds &bounds,
                        const ImVec2 &topLeft,
                        const ImVec2 &size,
                        ImU32 color)
    {
        const std::size_t renderedPointCount = computeRenderedPointCount(samples.size(), size.x);
        if (renderedPointCount < 2)
        {
            return;
        }

        bool hasPreviousPoint = false;
        ImVec2 previousPoint{};

        for (std::size_t renderedIndex = 0; renderedIndex < renderedPointCount; ++renderedIndex)
        {
            const physim::PhaseSpaceSample &sample = samples[mapRenderedSampleIndex(samples.size(), renderedPointCount, renderedIndex)];
            if (!isFinitePhaseSpaceSample(sample))
            {
                hasPreviousPoint = false;
                continue;
            }

            const ImVec2 currentPoint = mapPhaseSpaceSample(sample, bounds, topLeft, size);
            if (!std::isfinite(currentPoint.x) || !std::isfinite(currentPoint.y))
            {
                hasPreviousPoint = false;
                continue;
            }

            if (hasPreviousPoint)
            {
                drawList->AddLine(previousPoint, currentPoint, color, 1.5f);
            }

            previousPoint = currentPoint;
            hasPreviousPoint = true;
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
        drawAnalysisSection();
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

    void PendulumUiMenus::drawAnalysisSection() const
    {
        ImGui::Separator();
        ImGui::Spacing();
        ImGui::TextDisabled("Analysis Plots");
        drawEnergyHistoryPlot();
        drawPhaseSpacePlot();
    }

    void PendulumUiMenus::drawEnergyHistoryPlot() const
    {
        const auto &energyHistory = session.getSimulation().getEnergyHistory();

        ImGui::TextDisabled("Energy History");
        if (energyHistory.empty())
        {
            ImGui::TextDisabled("No energy samples recorded yet.");
            return;
        }

        const std::size_t renderedPointCount = computeRenderedPointCount(energyHistory.size(), ImGui::GetContentRegionAvail().x);
        std::vector<float> renderedEnergyValues;
        renderedEnergyValues.reserve(renderedPointCount);

        float minimumEnergy = std::numeric_limits<float>::max();
        float maximumEnergy = std::numeric_limits<float>::lowest();
        float firstSampleTime = 0.0f;
        float lastSampleTime = 0.0f;

        for (std::size_t renderedIndex = 0; renderedIndex < renderedPointCount; ++renderedIndex)
        {
            const ScalarHistorySample &sample = energyHistory[mapRenderedSampleIndex(energyHistory.size(), renderedPointCount, renderedIndex)];
            if (!isFiniteScalarHistorySample(sample))
            {
                continue;
            }

            if (renderedEnergyValues.empty())
            {
                firstSampleTime = sample.time;
            }

            lastSampleTime = sample.time;
            renderedEnergyValues.push_back(sample.value);
            minimumEnergy = std::min(minimumEnergy, sample.value);
            maximumEnergy = std::max(maximumEnergy, sample.value);
        }

        if (renderedEnergyValues.empty())
        {
            ImGui::TextDisabled("Energy history contains only non-finite samples.");
            return;
        }

        if ((maximumEnergy - minimumEnergy) < 1.0e-6f)
        {
            maximumEnergy += 1.0f;
            minimumEnergy -= 1.0f;
        }

        const float duration = lastSampleTime - firstSampleTime;
        ImGui::PlotLines("##energy-history-plot",
                         renderedEnergyValues.data(),
                         static_cast<int>(renderedEnergyValues.size()),
                         0,
                         nullptr,
                         minimumEnergy,
                         maximumEnergy,
                         ANALYSIS_PLOT_SIZE);
        ImGui::TextDisabled("Energy range: %.4f to %.4f J | span %.3f s",
                            minimumEnergy,
                            maximumEnergy,
                            duration);
    }

    void PendulumUiMenus::drawPhaseSpacePlot() const
    {
        const auto &simulation = session.getSimulation();
        const auto &primaryPhaseHistory = simulation.getPrimaryPhaseSpaceHistory();
        const auto &secondaryPhaseHistory = simulation.getSecondaryPhaseSpaceHistory();

        ImGui::TextDisabled("Phase Space");
        if (primaryPhaseHistory.size() < 2)
        {
            ImGui::TextDisabled("No phase-space samples recorded yet.");
            return;
        }

        const ImVec2 plotSize{std::max(ImGui::GetContentRegionAvail().x, 1.0f), 170.0f};
        const ImVec2 cursorPosition = ImGui::GetCursorScreenPos();
        ImGui::InvisibleButton("##phase-space-plot", plotSize);

        ImDrawList *drawList = ImGui::GetWindowDrawList();
        drawList->AddRectFilled(cursorPosition,
                                {cursorPosition.x + plotSize.x, cursorPosition.y + plotSize.y},
                                ANALYSIS_BACKGROUND_COLOR,
                                6.0f);
        drawList->AddRect(cursorPosition,
                          {cursorPosition.x + plotSize.x, cursorPosition.y + plotSize.y},
                          ANALYSIS_BORDER_COLOR,
                          6.0f,
                          0,
                          1.0f);

        for (int subdivision = 1; subdivision < ANALYSIS_GRID_SUBDIVISIONS; ++subdivision)
        {
            const float t = static_cast<float>(subdivision) / static_cast<float>(ANALYSIS_GRID_SUBDIVISIONS);
            const float gridX = cursorPosition.x + (plotSize.x * t);
            const float gridY = cursorPosition.y + (plotSize.y * t);

            drawList->AddLine({gridX, cursorPosition.y}, {gridX, cursorPosition.y + plotSize.y}, ANALYSIS_GRID_COLOR, 1.0f);
            drawList->AddLine({cursorPosition.x, gridY}, {cursorPosition.x + plotSize.x, gridY}, ANALYSIS_GRID_COLOR, 1.0f);
        }

        PhasePlotBounds bounds;
        expandPhaseBounds(primaryPhaseHistory, bounds);
        if (simulation.isDoubleMode())
        {
            expandPhaseBounds(secondaryPhaseHistory, bounds);
        }

        if (!std::isfinite(bounds.minAngle) || !std::isfinite(bounds.maxAngle) ||
            !std::isfinite(bounds.minAngularVelocity) || !std::isfinite(bounds.maxAngularVelocity))
        {
            ImGui::TextDisabled("Phase-space history contains only non-finite samples.");
            return;
        }

        bounds = normalizePhaseBounds(bounds);

        if (bounds.minAngle <= 0.0f && bounds.maxAngle >= 0.0f)
        {
            const physim::PhaseSpaceSample zeroAngleSample{0.0f, 0.0f, bounds.minAngularVelocity};
            const physim::PhaseSpaceSample zeroAngleSampleTop{0.0f, 0.0f, bounds.maxAngularVelocity};
            drawList->AddLine(mapPhaseSpaceSample(zeroAngleSample, bounds, cursorPosition, plotSize),
                              mapPhaseSpaceSample(zeroAngleSampleTop, bounds, cursorPosition, plotSize),
                              ANALYSIS_AXIS_COLOR,
                              1.0f);
        }

        if (bounds.minAngularVelocity <= 0.0f && bounds.maxAngularVelocity >= 0.0f)
        {
            const physim::PhaseSpaceSample zeroVelocitySample{0.0f, bounds.minAngle, 0.0f};
            const physim::PhaseSpaceSample zeroVelocitySampleRight{0.0f, bounds.maxAngle, 0.0f};
            drawList->AddLine(mapPhaseSpaceSample(zeroVelocitySample, bounds, cursorPosition, plotSize),
                              mapPhaseSpaceSample(zeroVelocitySampleRight, bounds, cursorPosition, plotSize),
                              ANALYSIS_AXIS_COLOR,
                              1.0f);
        }

        drawPhaseTrace(drawList, primaryPhaseHistory, bounds, cursorPosition, plotSize, PRIMARY_PHASE_COLOR);
        if (simulation.isDoubleMode())
        {
            drawPhaseTrace(drawList, secondaryPhaseHistory, bounds, cursorPosition, plotSize, SECONDARY_PHASE_COLOR);
            ImGui::TextDisabled("Two traces: (theta1, omega1) and (theta2, omega2)");
        }
        else
        {
            ImGui::TextDisabled("Trace: (theta, omega) for the simple pendulum");
        }

        ImGui::TextDisabled("theta range: %.2f to %.2f rad | omega range: %.2f to %.2f rad/s",
                            bounds.minAngle,
                            bounds.maxAngle,
                            bounds.minAngularVelocity,
                            bounds.maxAngularVelocity);
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
