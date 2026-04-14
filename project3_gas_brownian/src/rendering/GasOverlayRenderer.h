#ifndef GAS_OVERLAY_RENDERER_H
#define GAS_OVERLAY_RENDERER_H

namespace physim
{
    class GasSession;

    class GasOverlayRenderer
    {
    public:
        GasOverlayRenderer() = delete;
        explicit GasOverlayRenderer(const GasSession &session);

        void render() const;

    private:
        const GasSession &session;
    };
} // namespace physim

#endif // GAS_OVERLAY_RENDERER_H
