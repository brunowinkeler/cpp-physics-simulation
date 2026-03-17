#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

namespace physim
{
    namespace constants
    {
        inline constexpr float PI = 3.14159265f;
        inline constexpr float DEG_TO_RAD = PI / 180.0f;
        inline constexpr float RAD_TO_DEG = 180.0f / PI;
    }
    struct Environment
    {
        float gravity = 9.81f;   // Acceleration due to gravity (m/s^2)
        float airDensity = 0.0f; // Density of air (kg/m^3)
        float timeScale = 1.0f;  // Simulation time scale (1.0 = real-time)
    };
}

#endif // ENVIRONMENT_H
