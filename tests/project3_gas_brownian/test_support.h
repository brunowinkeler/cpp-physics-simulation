#ifndef PROJECT3_GAS_BROWNIAN_TEST_SUPPORT_H
#define PROJECT3_GAS_BROWNIAN_TEST_SUPPORT_H

#include "project3_gas_brownian/src/simulation/GasSimulation.h"

namespace physim
{
    namespace testsupport
    {
        inline void runGasSimulationSteps(GasSimulation &simulation, int maxSteps, float frameStep)
        {
            for (int step = 0; step < maxSteps && simulation.isRunning(); ++step)
            {
                simulation.update(frameStep);
            }
        }
    }
}

#endif // PROJECT3_GAS_BROWNIAN_TEST_SUPPORT_H
