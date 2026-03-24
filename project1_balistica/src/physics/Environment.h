#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

namespace physim
{
    struct Environment
    {
        float gravity = 9.81f;
        float airDensity = 0.0f;
        float timeScale = 3.0f;
    };
}

#endif // ENVIRONMENT_H
