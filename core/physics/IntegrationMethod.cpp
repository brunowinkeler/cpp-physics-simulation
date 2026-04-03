#include "IntegrationMethod.h"

namespace physim
{
    const char *integrationMethodLabel(IntegrationMethod method)
    {
        switch (method)
        {
        case IntegrationMethod::SymplecticEuler:
            return "Symplectic Euler";
        case IntegrationMethod::RungeKutta4:
        default:
            return "Runge-Kutta 4";
        }
    }

    bool isSupportedIntegrationMethod(IntegrationMethod method)
    {
        switch (method)
        {
        case IntegrationMethod::SymplecticEuler:
        case IntegrationMethod::RungeKutta4:
            return true;
        default:
            return false;
        }
    }
} // namespace physim