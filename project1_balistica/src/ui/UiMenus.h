#ifndef UIMENUS_H
#define UIMENUS_H

#include "physics/Environment.h"
#include "physics/Projectile.h"
#include "simulation/Simulation.h"

namespace physim
{
    class UiMenus
    {
    public:
        UiMenus() = delete;
        UiMenus(Environment &env, Projectile &proj, Simulation &sim);
        void parametersSelectionScreen();
        int getSelectedHistoryEntryId() const { return selectedHistoryEntryId; }
        int getHoveredHistoryEntryId() const { return hoveredHistoryEntryId; }

    private:
        void drawLaunchHistoryWindow();

        Environment &environment;
        Projectile &projectile;
        Simulation &simulation;
        int selectedHistoryEntryId{-1};
        int hoveredHistoryEntryId{-1};

        bool showDemoWindow{false}; // For testing and demonstration purposes
    };
} // namespace physim

#endif // UIMENUS_H
