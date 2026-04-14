#include "GasUiMenus.h"

#include "simulation/GasSession.h"

#include "imgui.h"
#include "rlImGui.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <string>
#include <vector>

namespace
{
    constexpr std::size_t MAX_RENDERED_PRESSURE_POINTS = 512;
    constexpr ImVec2 PRESSURE_PLOT_SIZE{0.0f, 160.0f};

    const char *simulationStateLabel(physim::GasSimulationState state)
    {
        switch (state)
        {
        case physim::GasSimulationState::Running:
            return "Running";
        case physim::GasSimulationState::Paused:
            return "Paused";
        case physim::GasSimulationState::Idle:
        default:
            return "Idle";
        }
    }

    std::size_t computeRenderedPointCount(std::size_t sampleCount, float availableWidth)
    {
        if (sampleCount <= 1)
        {
            return sampleCount;
        }

        const std::size_t widthBudget = static_cast<std::size_t>(std::max(2.0f, std::ceil(availableWidth)));
        return std::min(sampleCount, std::min(widthBudget, MAX_RENDERED_PRESSURE_POINTS));
    }

    std::size_t mapRenderedSampleIndex(std::size_t sampleCount, std::size_t renderedPointCount, std::size_t renderedIndex)
    {
        if (sampleCount <= 1 || renderedPointCount <= 1)
        {
            return 0;
        }

        return (renderedIndex * (sampleCount - 1)) / (renderedPointCount - 1);
    }
}

namespace physim
{
    GasUiMenus::GasUiMenus(GasSession &session)
        : session{session}
    {
    }

    void GasUiMenus::render()
    {
        rlImGuiBegin();

        GasSimulation &simulation = session.getSimulation();
        const bool editable = !simulation.isRunning();

        ImGui::SetNextWindowPos({20.0f, 20.0f}, ImGuiCond_Once);
        ImGui::SetNextWindowSize({390.0f, 660.0f}, ImGuiCond_Once);

        if (ImGui::Begin("Gas And Brownian Lab"))
        {
            drawModeSection(editable);
            drawEnvironmentSection(editable);
            drawVisualizationSection();
            drawTracerSection(editable);
            drawStatusSection();
            drawPressurePlot();
            drawControlSection();
        }

        ImGui::End();
        rlImGuiEnd();
    }

    void GasUiMenus::drawModeSection(bool editable)
    {
        GasSimulation &simulation = session.getSimulation();

        ImGui::TextDisabled("Mode");
        if (!editable)
        {
            ImGui::BeginDisabled();
        }

        int modeIndex = simulation.isBrownianMode() ? 1 : 0;
        if (ImGui::Combo("Simulation", &modeIndex, "Ideal Gas\0Brownian\0"))
        {
            simulation.setMode(modeIndex == 0 ? GasSimulationMode::IdealGas : GasSimulationMode::Brownian);
        }

        if (!editable)
        {
            ImGui::EndDisabled();
        }
    }

    void GasUiMenus::drawEnvironmentSection(bool editable)
    {
        GasSimulation &simulation = session.getSimulation();
        GasEnvironment &environment = session.getEnvironment();

        ImGui::Separator();
        ImGui::Spacing();
        ImGui::TextDisabled("Environment");

        ImGui::SliderFloat("Time Scale", &environment.timeScale, 0.1f, 20.0f, "%.2fx");

        if (!editable)
        {
            ImGui::BeginDisabled();
        }

        int particleCount = simulation.getParticleCount();
        if (ImGui::InputInt("Particle Count", &particleCount, 100, 500))
        {
            simulation.setParticleCount(particleCount);
        }

        const int countPresets[] = {100, 250, 500, 1000, 2000, 5000};
        if (ImGui::BeginCombo("Count Presets", "Apply..."))
        {
            for (int countPreset : countPresets)
            {
                const bool selected = simulation.getParticleCount() == countPreset;
                if (ImGui::Selectable(std::to_string(countPreset).c_str(), selected))
                {
                    simulation.setParticleCount(countPreset);
                }
            }
            ImGui::EndCombo();
        }

        int presetIndex = getGasPresetIndex(simulation.getGasPresetId());
        if (ImGui::BeginCombo("Gas Preset", simulation.getGasPreset().name))
        {
            for (int index = 0; index < getGasPresetCount(); ++index)
            {
                const GasPreset &preset = getGasPreset(getGasPresetId(index));
                const bool selected = presetIndex == index;
                if (ImGui::Selectable(preset.name, selected))
                {
                    simulation.setGasPresetId(preset.id);
                }
            }
            ImGui::EndCombo();
        }

        float boxWidth = environment.boxWidth;
        float boxHeight = environment.boxHeight;
        if (ImGui::SliderFloat("Box Width", &boxWidth, 4.0f, 30.0f, "%.1f m"))
        {
            simulation.setBoxSize(boxWidth, environment.boxHeight);
        }
        if (ImGui::SliderFloat("Box Height", &boxHeight, 2.0f, 20.0f, "%.1f m"))
        {
            simulation.setBoxSize(environment.boxWidth, boxHeight);
        }

        float physicsTimeStep = simulation.getPhysicsTimeStep();
        if (ImGui::SliderFloat("Physics dt", &physicsTimeStep, 1.0e-4f, 1.0f / 60.0f, "%.5f s", ImGuiSliderFlags_Logarithmic))
        {
            simulation.setPhysicsTimeStep(physicsTimeStep);
        }

        float thermalLevel = simulation.getInitialTemperatureKelvin();
        if (ImGui::SliderFloat("Thermal Level", &thermalLevel, 20.0f, 600.0f, "%.1f K"))
        {
            simulation.setInitialTemperatureKelvin(thermalLevel);
        }

        int randomSeed = static_cast<int>(simulation.getRandomSeed());
        if (ImGui::InputInt("Seed", &randomSeed))
        {
            simulation.setRandomSeed(static_cast<unsigned int>(std::max(randomSeed, 0)));
        }

        if (!editable)
        {
            ImGui::EndDisabled();
        }

        ImGui::TextDisabled("Gas analog: molar mass %.4e kg/mol | particle mass %.4e kg",
                            simulation.getGasPreset().molarMassKgPerMol,
                            simulation.getGasPreset().particleMassKg);
    }

    void GasUiMenus::drawVisualizationSection()
    {
        GasVisualizationSettings &visualization = session.getVisualizationSettings();

        ImGui::Separator();
        ImGui::Spacing();
        ImGui::TextDisabled("Visualization");
        ImGui::Checkbox("Show Grid", &visualization.showGrid);
        ImGui::Checkbox("Show Pressure Plot", &visualization.showPressurePlot);
    }

    void GasUiMenus::drawTracerSection(bool editable)
    {
        GasSimulation &simulation = session.getSimulation();
        if (!simulation.isBrownianMode())
        {
            return;
        }

        ImGui::Separator();
        ImGui::Spacing();
        ImGui::TextDisabled("Tracer Particle");

        bool tracerTrailEnabled = simulation.isTracerTrailEnabled();
        if (ImGui::Checkbox("Record Tracer Trail", &tracerTrailEnabled))
        {
            simulation.setTracerTrailEnabled(tracerTrailEnabled);
        }

        if (!editable)
        {
            ImGui::BeginDisabled();
        }

        float tracerMassMultiplier = simulation.getTracerMassMultiplier();
        if (ImGui::SliderFloat("Tracer Mass Scale", &tracerMassMultiplier, 1.0f, 64.0f, "%.1fx"))
        {
            simulation.setTracerMassMultiplier(tracerMassMultiplier);
        }

        float tracerRadiusMultiplier = simulation.getTracerRadiusMultiplier();
        if (ImGui::SliderFloat("Tracer Radius Scale", &tracerRadiusMultiplier, 1.0f, 8.0f, "%.1fx"))
        {
            simulation.setTracerRadiusMultiplier(tracerRadiusMultiplier);
        }

        if (!editable)
        {
            ImGui::EndDisabled();
        }
    }

    void GasUiMenus::drawStatusSection() const
    {
        const GasSimulation &simulation = session.getSimulation();

        ImGui::Separator();
        ImGui::Spacing();
        ImGui::TextDisabled("Status");
        ImGui::Text("State: %s", simulationStateLabel(simulation.getState()));
        ImGui::Text("Sim Time: %.3f s", simulation.getTimeGlobal());
        ImGui::Text("Pressure Instant: %.3e N/m", simulation.getCurrentPressureInstant());
        ImGui::Text("Thermal Level: %.2f K", simulation.getCurrentTemperatureKelvin());
        ImGui::Text("Average KE: %.3e J", simulation.getAverageKineticEnergy());
        ImGui::Text("Average Speed: %.3f m/s", simulation.getAverageSpeed());
        ImGui::Text("Rendered Particles: %d", static_cast<int>(simulation.getParticles().size()));
    }

    void GasUiMenus::drawPressurePlot() const
    {
        if (!session.getVisualizationSettings().showPressurePlot)
        {
            return;
        }

        const auto &pressureHistory = session.getSimulation().getPressureHistory();
        ImGui::Separator();
        ImGui::Spacing();
        ImGui::TextDisabled("Instantaneous Pressure History");

        if (pressureHistory.empty())
        {
            ImGui::TextDisabled("No pressure samples recorded yet.");
            return;
        }

        const std::size_t renderedPointCount = computeRenderedPointCount(pressureHistory.size(), ImGui::GetContentRegionAvail().x);
        std::vector<float> renderedPressureValues;
        renderedPressureValues.reserve(renderedPointCount);

        float minimumPressure = std::numeric_limits<float>::max();
        float maximumPressure = std::numeric_limits<float>::lowest();
        float firstSampleTime = 0.0f;
        float lastSampleTime = 0.0f;

        for (std::size_t renderedIndex = 0; renderedIndex < renderedPointCount; ++renderedIndex)
        {
            const PressureHistorySample &sample = pressureHistory[mapRenderedSampleIndex(pressureHistory.size(), renderedPointCount, renderedIndex)];
            if (!std::isfinite(sample.pressure))
            {
                continue;
            }

            if (renderedPressureValues.empty())
            {
                firstSampleTime = sample.time;
            }

            lastSampleTime = sample.time;
            renderedPressureValues.push_back(sample.pressure);
            minimumPressure = std::min(minimumPressure, sample.pressure);
            maximumPressure = std::max(maximumPressure, sample.pressure);
        }

        if (renderedPressureValues.empty())
        {
            ImGui::TextDisabled("Pressure history contains only non-finite samples.");
            return;
        }

        if ((maximumPressure - minimumPressure) < 1.0e-9f)
        {
            maximumPressure += 1.0f;
            minimumPressure -= 1.0f;
        }

        ImGui::PlotLines("##pressure-history-plot",
                         renderedPressureValues.data(),
                         static_cast<int>(renderedPressureValues.size()),
                         0,
                         nullptr,
                         minimumPressure,
                         maximumPressure,
                         PRESSURE_PLOT_SIZE);
        ImGui::TextDisabled("Window span: %.3f s | current %.3e N/m",
                            lastSampleTime - firstSampleTime,
                            session.getSimulation().getCurrentPressureInstant());
    }

    void GasUiMenus::drawControlSection()
    {
        GasSimulation &simulation = session.getSimulation();

        ImGui::Separator();
        ImGui::Spacing();
        ImGui::TextDisabled("Controls");

        if (simulation.isRunning())
        {
            if (ImGui::Button("Pause"))
            {
                simulation.stop();
            }
        }
        else if (ImGui::Button(simulation.isPaused() ? "Resume" : "Start"))
        {
            simulation.start();
        }

        ImGui::SameLine();
        if (ImGui::Button("Reset"))
        {
            simulation.reset();
        }
    }
} // namespace physim
