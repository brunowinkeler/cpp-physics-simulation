#ifndef UIMENUS_H
#define UIMENUS_H

namespace physim
{
    class SimulationSession;
}

namespace physim
{
    class UiMenus
    {
    public:
        UiMenus() = delete;
        explicit UiMenus(SimulationSession &session);
        void parametersSelectionScreen();
        int getSelectedHistoryEntryId() const { return selectedHistoryEntryId; }
        int getHoveredHistoryEntryId() const { return hoveredHistoryEntryId; }

    private:
        void drawLaunchHistoryWindow();

        SimulationSession &session;
        int selectedHistoryEntryId{-1};
        int hoveredHistoryEntryId{-1};

        bool showDemoWindow{false}; // For testing and demonstration purposes
    };
} // namespace physim

#endif // UIMENUS_H
