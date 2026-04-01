#ifndef TEST_SUPPORT_H
#define TEST_SUPPORT_H

#include "project1_balistica/src/physics/Projectile.h"
#include "project1_balistica/src/simulation/Simulation.h"

#include <limits>

namespace physim
{
    namespace testsupport
    {
        inline constexpr float GRAVITY = 9.81f;
        inline constexpr float TIME_LIMIT = 60.0f;

        inline void runSimulationUntilStopped(Simulation &simulation, int maxSteps, float frameStep)
        {
            for (int step = 0; step < maxSteps && simulation.isRunning(); ++step)
            {
                simulation.update(frameStep);
            }
        }

        inline double simulateRange(Projectile projectile, Environment environment, float timeStep)
        {
            projectile.launch();

            float elapsedTime = 0.0f;
            while (!projectile.isLanded() && elapsedTime < TIME_LIMIT)
            {
                projectile.update(timeStep, environment);
                elapsedTime += timeStep;
            }

            if (!projectile.isLanded())
            {
                return std::numeric_limits<double>::quiet_NaN();
            }

            return static_cast<double>(projectile.getPosition().x);
        }
    }
}

#endif // TEST_SUPPORT_H
