#ifndef SIMULATION_OVERLAY_RENDERER_H
#define SIMULATION_OVERLAY_RENDERER_H

namespace physim
{
    class Simulation;

    class SimulationOverlayRenderer
    {
    public:
        explicit SimulationOverlayRenderer(const Simulation &simulation)
            : simulation{simulation}
        {
        }

        void render() const;

    private:
        void renderSimulationTime() const;
        void renderInteractionHint() const;

        const Simulation &simulation;
    };
} // namespace physim

#endif // SIMULATION_OVERLAY_RENDERER_H
