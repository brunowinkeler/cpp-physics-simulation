#ifndef GAS_SESSION_H
#define GAS_SESSION_H

#include "physics/GasEnvironment.h"
#include "GasSimulation.h"

namespace physim
{
    struct GasVisualizationSettings
    {
        bool showGrid{true};
        bool showPressurePlot{true};
    };

    class GasSession
    {
    public:
        GasSession() : simulation{environment} {}

        GasEnvironment &getEnvironment() { return environment; }
        const GasEnvironment &getEnvironment() const { return environment; }

        GasSimulation &getSimulation() { return simulation; }
        const GasSimulation &getSimulation() const { return simulation; }

        GasVisualizationSettings &getVisualizationSettings() { return visualizationSettings; }
        const GasVisualizationSettings &getVisualizationSettings() const { return visualizationSettings; }

    private:
        GasEnvironment environment;
        GasVisualizationSettings visualizationSettings;
        GasSimulation simulation;
    };
} // namespace physim

#endif // GAS_SESSION_H
