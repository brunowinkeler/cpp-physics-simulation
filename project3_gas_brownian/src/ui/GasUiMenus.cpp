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
    constexpr float MIN_TIME_SCALE = 0.1f;
    constexpr float MAX_TIME_SCALE = 20.0f;
    constexpr float MIN_BOX_WIDTH = 4.0f;
    constexpr float MAX_BOX_WIDTH = 30.0f;
    constexpr float MIN_BOX_HEIGHT = 2.0f;
    constexpr float MAX_BOX_HEIGHT = 20.0f;
    constexpr float MIN_PHYSICS_TIME_STEP = 1.0e-4f;
    constexpr float MAX_PHYSICS_TIME_STEP = 1.0f / 60.0f;
    constexpr float MIN_THERMAL_LEVEL = 20.0f;
    constexpr float MAX_THERMAL_LEVEL = 600.0f;
    constexpr float MIN_TRACER_MASS_SCALE = 1.0f;
    constexpr float MAX_TRACER_MASS_SCALE = 64.0f;
    constexpr float MIN_TRACER_RADIUS_SCALE = 1.0f;
    constexpr float MAX_TRACER_RADIUS_SCALE = 8.0f;

    const char *modeDescription(bool brownianMode)
    {
        return brownianMode
                   ? "A heavier tracer moves inside the particle bath."
                   : "A uniform particle bath inside the box.";
    }

    const char *gasPresetDescription(physim::GasPresetId gasPresetId)
    {
        switch (gasPresetId)
        {
        case physim::GasPresetId::Helium:
            return "Light gas preset with high thermal agitation for the same effective temperature.";
        case physim::GasPresetId::Air:
            return "Balanced baseline preset for general experiments and comparisons.";
        case physim::GasPresetId::Argon:
            return "Heavier inert gas preset with slower characteristic particle speeds.";
        case physim::GasPresetId::CarbonDioxide:
        default:
            return "Dense molecular preset useful for contrasting heavier bath particles.";
        }
    }

    void drawParticleCountPresetButtons(physim::GasSimulation &simulation)
    {
        constexpr int COUNT_PRESETS[] = {100, 250, 500, 1000, 2000, 5000};

        ImGui::TextDisabled("Quick counts");
        for (std::size_t index = 0; index < (sizeof(COUNT_PRESETS) / sizeof(COUNT_PRESETS[0])); ++index)
        {
            ImGui::PushID(static_cast<int>(index));

            const int countPreset = COUNT_PRESETS[index];
            const bool selected = simulation.getParticleCount() == countPreset;
            if (selected)
            {
                ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered));
            }

            if (ImGui::Button(std::to_string(countPreset).c_str(), {56.0f, 0.0f}))
            {
                simulation.setParticleCount(countPreset);
            }

            if (selected)
            {
                ImGui::PopStyleColor();
            }

            ImGui::PopID();
            if (index + 1 < (sizeof(COUNT_PRESETS) / sizeof(COUNT_PRESETS[0])))
            {
                ImGui::SameLine();
            }
        }
    }

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

        ImGui::SeparatorText("Simulation Mode");
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

        ImGui::TextDisabled("%s", modeDescription(simulation.isBrownianMode()));
    }

    void GasUiMenus::drawEnvironmentSection(bool editable)
    {
        GasSimulation &simulation = session.getSimulation();
        GasEnvironment &environment = session.getEnvironment();

        ImGui::SeparatorText("Bath And Box");

        float timeScale = environment.timeScale;
        if (ImGui::InputFloat("Time Scale (x)", &timeScale, 0.10f, 1.00f, "%.2f"))
        {
            environment.timeScale = std::clamp(timeScale, MIN_TIME_SCALE, MAX_TIME_SCALE);
        }
        ImGui::TextDisabled("Runtime multiplier for the simulation clock.");

        if (!editable)
        {
            ImGui::BeginDisabled();
        }

        int particleCount = simulation.getParticleCount();
        if (ImGui::InputInt("Particle Count", &particleCount, 100, 500))
        {
            simulation.setParticleCount(particleCount);
        }

        drawParticleCountPresetButtons(simulation);

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

        const GasPreset &preset = simulation.getGasPreset();
        ImGui::TextDisabled("%s", gasPresetDescription(preset.id));
        ImGui::TextDisabled("Molar mass %.4e kg/mol | particle mass %.4e kg | effective radius %.4f m",
                            preset.molarMassKgPerMol,
                            preset.particleMassKg,
                            preset.effectiveParticleRadius);
        ImGui::TextDisabled("The radius is effective for collision and readability, not a literal molecular diameter.");

        float boxWidth = environment.boxWidth;
        float boxHeight = environment.boxHeight;
        if (ImGui::InputFloat("Box Width (m)", &boxWidth, 0.25f, 1.00f, "%.2f"))
        {
            simulation.setBoxSize(std::clamp(boxWidth, MIN_BOX_WIDTH, MAX_BOX_WIDTH), environment.boxHeight);
        }
        if (ImGui::InputFloat("Box Height (m)", &boxHeight, 0.25f, 1.00f, "%.2f"))
        {
            simulation.setBoxSize(environment.boxWidth, std::clamp(boxHeight, MIN_BOX_HEIGHT, MAX_BOX_HEIGHT));
        }

        float physicsTimeStep = simulation.getPhysicsTimeStep();
        if (ImGui::InputFloat("Physics dt (s)", &physicsTimeStep, 0.0005f, 0.0050f, "%.5f"))
        {
            simulation.setPhysicsTimeStep(std::clamp(physicsTimeStep, MIN_PHYSICS_TIME_STEP, MAX_PHYSICS_TIME_STEP));
        }

        float thermalLevel = simulation.getInitialTemperatureKelvin();
        if (ImGui::InputFloat("Thermal Level (K)", &thermalLevel, 5.0f, 25.0f, "%.1f"))
        {
            simulation.setInitialTemperatureKelvin(std::clamp(thermalLevel, MIN_THERMAL_LEVEL, MAX_THERMAL_LEVEL));
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
    }

    void GasUiMenus::drawVisualizationSection()
    {
        GasVisualizationSettings &visualization = session.getVisualizationSettings();

        ImGui::SeparatorText("Display");
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

        ImGui::SeparatorText("Brownian Tracer");

        bool tracerTrailEnabled = simulation.isTracerTrailEnabled();
        if (ImGui::Checkbox("Record Tracer Trail", &tracerTrailEnabled))
        {
            simulation.setTracerTrailEnabled(tracerTrailEnabled);
        }
        ImGui::TextDisabled("Disable the trail to reduce update and draw cost at higher particle counts.");

        if (!editable)
        {
            ImGui::BeginDisabled();
        }

        float tracerMassMultiplier = simulation.getTracerMassMultiplier();
        if (ImGui::InputFloat("Tracer Mass Scale", &tracerMassMultiplier, 1.0f, 4.0f, "%.2f"))
        {
            simulation.setTracerMassMultiplier(std::clamp(tracerMassMultiplier, MIN_TRACER_MASS_SCALE, MAX_TRACER_MASS_SCALE));
        }

        float tracerRadiusMultiplier = simulation.getTracerRadiusMultiplier();
        if (ImGui::InputFloat("Tracer Radius Scale", &tracerRadiusMultiplier, 0.25f, 1.0f, "%.2f"))
        {
            simulation.setTracerRadiusMultiplier(std::clamp(tracerRadiusMultiplier, MIN_TRACER_RADIUS_SCALE, MAX_TRACER_RADIUS_SCALE));
        }

        if (!editable)
        {
            ImGui::EndDisabled();
        }
    }

    void GasUiMenus::drawStatusSection() const
    {
        const GasSimulation &simulation = session.getSimulation();

        ImGui::SeparatorText("Live Metrics");
        ImGui::Text("State: %s", simulationStateLabel(simulation.getState()));
        ImGui::Text("Sim Time: %.3f s", simulation.getTimeGlobal());
        ImGui::Text("Instantaneous Pressure: %.3e N/m", simulation.getCurrentPressureInstant());
        ImGui::Text("Effective Temperature: %.2f K", simulation.getCurrentTemperatureKelvin());
        ImGui::Text("Average KE: %.3e J", simulation.getAverageKineticEnergy());
        ImGui::Text("Average Speed: %.3f m/s", simulation.getAverageSpeed());
        ImGui::Text("Active Particles: %d", static_cast<int>(simulation.getParticles().size()));
    }

    void GasUiMenus::drawPressurePlot() const
    {
        if (!session.getVisualizationSettings().showPressurePlot)
        {
            return;
        }

        const auto &pressureHistory = session.getSimulation().getPressureHistory();
        ImGui::SeparatorText("Pressure Plot");

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

        const float currentPressure = session.getSimulation().getCurrentPressureInstant();
        const float windowSpan = lastSampleTime - firstSampleTime;

        ImGui::Text("Current value: %.3e N/m", currentPressure);
        ImGui::TextDisabled("Recent window: %.2f s | range %.3e .. %.3e N/m",
                            windowSpan,
                            minimumPressure,
                            maximumPressure);

        ImGui::PlotLines("##pressure-history-plot",
                         renderedPressureValues.data(),
                         static_cast<int>(renderedPressureValues.size()),
                         0,
                         nullptr,
                         minimumPressure,
                         maximumPressure,
                         PRESSURE_PLOT_SIZE);
        ImGui::TextDisabled("Instantaneous wall loading over the recent sliding window.");
    }

    void GasUiMenus::drawControlSection()
    {
        GasSimulation &simulation = session.getSimulation();

        ImGui::SeparatorText("Controls");

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
