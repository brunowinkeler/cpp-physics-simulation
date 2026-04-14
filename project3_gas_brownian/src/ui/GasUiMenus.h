#ifndef GAS_UI_MENUS_H
#define GAS_UI_MENUS_H

namespace physim
{
    class GasSession;

    class GasUiMenus
    {
    public:
        GasUiMenus() = delete;
        explicit GasUiMenus(GasSession &session);

        void render();

    private:
        void drawModeSection(bool editable);
        void drawEnvironmentSection(bool editable);
        void drawVisualizationSection();
        void drawTracerSection(bool editable);
        void drawStatusSection() const;
        void drawPressurePlot() const;
        void drawControlSection();

        GasSession &session;
    };
} // namespace physim

#endif // GAS_UI_MENUS_H
