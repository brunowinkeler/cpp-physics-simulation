#ifndef PROJECT2_PENDULUM_TEST_SUPPORT_H
#define PROJECT2_PENDULUM_TEST_SUPPORT_H

#include "project2_pendulum/src/physics/SimplePendulum.h"
#include "project2_pendulum/src/simulation/PendulumSimulation.h"

#include <cmath>
#include <limits>
#include <optional>

namespace physim
{
    namespace testsupport
    {
        inline double interpolateZeroCrossing(double startTime, double startValue, double endTime, double endValue)
        {
            const double valueDelta = endValue - startValue;
            if (std::abs(valueDelta) <= 1.0e-12)
            {
                return endTime;
            }

            return startTime - (startValue * (endTime - startTime) / valueDelta);
        }

        inline double estimateSimplePendulumPeriod(SimplePendulum pendulum,
                                                   PendulumEnvironment environment,
                                                   float timeStep,
                                                   int maxSteps)
        {
            pendulum.reset();

            double time = 0.0;
            double previousTheta = pendulum.getState().theta;
            std::optional<double> firstCrossingTime;

            for (int step = 0; step < maxSteps; ++step)
            {
                pendulum.update(timeStep, environment);
                time += timeStep;

                const double theta = pendulum.getState().theta;

                if (!firstCrossingTime.has_value())
                {
                    if (previousTheta > 0.0 && theta <= 0.0)
                    {
                        firstCrossingTime = interpolateZeroCrossing(time - timeStep, previousTheta, time, theta);
                    }
                }
                else if (previousTheta < 0.0 && theta >= 0.0)
                {
                    const double secondCrossingTime = interpolateZeroCrossing(time - timeStep, previousTheta, time, theta);
                    return 2.0 * (secondCrossingTime - *firstCrossingTime);
                }

                previousTheta = theta;
            }

            return std::numeric_limits<double>::quiet_NaN();
        }

        inline void runPendulumSimulationSteps(PendulumSimulation &simulation, int maxSteps, float frameStep)
        {
            for (int step = 0; step < maxSteps && simulation.isRunning(); ++step)
            {
                simulation.update(frameStep);
            }
        }
    }
}

#endif // PROJECT2_PENDULUM_TEST_SUPPORT_H
