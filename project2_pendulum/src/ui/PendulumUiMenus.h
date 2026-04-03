#ifndef PENDULUM_UI_MENUS_H
#define PENDULUM_UI_MENUS_H

namespace physim
{
    class PendulumSession;

    class PendulumUiMenus
    {
    public:
        PendulumUiMenus() = delete;
        explicit PendulumUiMenus(PendulumSession &session);

        void render();

    private:
        void drawModeSection(bool editableInitialConditions);
        void drawEnvironmentSection(bool editableInitialConditions);
        void drawVisualizationSection();
        void drawSimplePendulumSection(bool editableInitialConditions);
        void drawDoublePendulumSection(bool editableInitialConditions);
        void drawStatusSection() const;
        void drawControlSection();

        PendulumSession &session;
    };
} // namespace physim

#endif // PENDULUM_UI_MENUS_H
