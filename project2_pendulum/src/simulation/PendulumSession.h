#ifndef PENDULUM_SESSION_H
#define PENDULUM_SESSION_H

#include "physics/DoublePendulum.h"
#include "physics/PendulumEnvironment.h"
#include "physics/SimplePendulum.h"
#include "PendulumSimulation.h"

namespace physim
{
    struct PendulumVisualizationSettings
    {
        bool showGrid{true};
        bool showAxes{true};
        bool showPrimaryTrail{true};
        bool showSecondaryTrail{true};
    };

    class PendulumSession
    {
    public:
        PendulumSession() : simulation{environment, simplePendulum, doublePendulum} {}

        PendulumEnvironment &getEnvironment() { return environment; }
        const PendulumEnvironment &getEnvironment() const { return environment; }

        SimplePendulum &getSimplePendulum() { return simplePendulum; }
        const SimplePendulum &getSimplePendulum() const { return simplePendulum; }

        DoublePendulum &getDoublePendulum() { return doublePendulum; }
        const DoublePendulum &getDoublePendulum() const { return doublePendulum; }

        PendulumSimulation &getSimulation() { return simulation; }
        const PendulumSimulation &getSimulation() const { return simulation; }

        PendulumVisualizationSettings &getVisualizationSettings() { return visualizationSettings; }
        const PendulumVisualizationSettings &getVisualizationSettings() const { return visualizationSettings; }

    private:
        PendulumEnvironment environment;
        SimplePendulum simplePendulum;
        DoublePendulum doublePendulum;
        PendulumVisualizationSettings visualizationSettings;
        PendulumSimulation simulation;
    };
} // namespace physim

#endif // PENDULUM_SESSION_H
