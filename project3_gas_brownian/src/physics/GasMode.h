#ifndef GAS_MODE_H
#define GAS_MODE_H

namespace physim
{
    enum class GasSimulationMode
    {
        IdealGas = 0,
        Brownian,
    };

    enum class GasSimulationState
    {
        Idle = 0,
        Running,
        Paused,
    };
} // namespace physim

#endif // GAS_MODE_H
