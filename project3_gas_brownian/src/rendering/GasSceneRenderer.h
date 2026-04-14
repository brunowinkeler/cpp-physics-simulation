#ifndef GAS_SCENE_RENDERER_H
#define GAS_SCENE_RENDERER_H

#include "GasCamera.h"

namespace physim
{
    class GasSession;

    class GasSceneRenderer
    {
    public:
        GasSceneRenderer() = delete;
        explicit GasSceneRenderer(const GasSession &session);

        void updateCamera(float frameTime);
        void resetCamera();
        void render() const;

    private:
        void drawGrid() const;
        void drawBox() const;
        void drawTracerTrail() const;
        void drawParticles() const;

        static Vector2 toRenderPosition(float x, float y);

        const GasSession &session;
        mutable GasCamera camera;
    };
} // namespace physim

#endif // GAS_SCENE_RENDERER_H
