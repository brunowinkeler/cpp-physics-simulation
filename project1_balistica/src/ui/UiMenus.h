#ifndef UIMENUS_H
#define UIMENUS_H

#include "physics/Environment.h"
#include "physics/Projectile.h"

namespace physim
{
    class UiMenus
    {
    public:
        UiMenus() = delete;
        UiMenus(Environment &env, Projectile &proj);
        void parametersSelectionScreen();

    private:
        Environment &environment;
        Projectile &projectile;
    };
} // namespace physim

#endif // UIMENUS_H
