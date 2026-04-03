#ifndef INTEGRATION_METHOD_H
#define INTEGRATION_METHOD_H

namespace physim
{
    enum class IntegrationMethod
    {
        SymplecticEuler = 0,
        RungeKutta4
    };

    inline constexpr IntegrationMethod DEFAULT_INTEGRATION_METHOD = IntegrationMethod::RungeKutta4;

    const char *integrationMethodLabel(IntegrationMethod method);
    bool isSupportedIntegrationMethod(IntegrationMethod method);
} // namespace physim

#endif // INTEGRATION_METHOD_H