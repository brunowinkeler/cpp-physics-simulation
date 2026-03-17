#include "Simulation.h"

int main()
{
    physim::Projectile projectile{50.0, 45.0};  // Initial position (0,0), speed 50 m/s at 45 degrees
    physim::Environment environment{9.81, 0.0}; // Gravity 9.81 m/s^2, no air resistance

    // Create a simulation instance
    physim::Simulation simulation(projectile, environment);

    // Run the simulation with a time step of 0.01 seconds
    simulation.run(0.01);

    return 0;
}
