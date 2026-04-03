#include "core/physics/IntegrationMethod.h"

#include "gtest/gtest.h"

TEST(IntegrationMethodTest, LabelsAreStable)
{
    EXPECT_STREQ(integrationMethodLabel(physim::IntegrationMethod::SymplecticEuler), "Symplectic Euler");
    EXPECT_STREQ(integrationMethodLabel(physim::IntegrationMethod::RungeKutta4), "Runge-Kutta 4");
}

TEST(IntegrationMethodTest, SupportedMethodsAreRecognized)
{
    EXPECT_TRUE(isSupportedIntegrationMethod(physim::IntegrationMethod::SymplecticEuler));
    EXPECT_TRUE(isSupportedIntegrationMethod(physim::IntegrationMethod::RungeKutta4));
    EXPECT_FALSE(isSupportedIntegrationMethod(static_cast<physim::IntegrationMethod>(99)));
}

TEST(IntegrationMethodTest, DefaultMethodRemainsRungeKutta4)
{
    EXPECT_EQ(physim::DEFAULT_INTEGRATION_METHOD, physim::IntegrationMethod::RungeKutta4);
}
