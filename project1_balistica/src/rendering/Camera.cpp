#include "Camera.h"

#include "imgui.h"
#include "raymath.h"

namespace physim
{
    void Camera::update()
    {
        // World bounds (must match floor in BackgroundRenderer)
        const float worldMinX = -100.0f, worldMaxX = 1000.0f;
        const float worldMinY = -300.0f, worldMaxY = 500.0f;
        const float worldW = worldMaxX - worldMinX;
        const float worldH = worldMaxY - worldMinY;

        float screenW = (float)GetScreenWidth();
        float screenH = (float)GetScreenHeight();

        // Minimum zoom: viewport must never be larger than the world bounds
        float minZoom = fmaxf(screenW / worldW, screenH / worldH);

        // Translate based on mouse left button drag
        if (!ImGui::GetIO().WantCaptureMouse && IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        {
            Vector2 delta = GetMouseDelta();
            delta = Vector2Scale(delta, -1.0f / camera2D.zoom);
            camera2D.target = Vector2Add(camera2D.target, delta);
        }

        if (IsKeyPressed(KEY_F))
        {
            reset();
        }

        float wheel = ImGui::GetIO().WantCaptureMouse ? 0.0f : GetMouseWheelMove();
        if (wheel != 0)
        {
            Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), camera2D);

            camera2D.offset = GetMousePosition();
            camera2D.target = mouseWorldPos;

            float scale = 0.2f * wheel;
            camera2D.zoom = Clamp(expf(logf(camera2D.zoom) + scale), minZoom, 32.0f);
        }

        // // Also enforce minZoom in case screen was resized (making previous zoom too small)
        // if (camera2D.zoom < minZoom)
        //     camera2D.zoom = minZoom;

        // // Compute the world coordinates visible at each screen edge
        // float leftEdge = camera2D.target.x - camera2D.offset.x / camera2D.zoom;
        // float rightEdge = camera2D.target.x + (screenW - camera2D.offset.x) / camera2D.zoom;
        // float topEdge = camera2D.target.y - camera2D.offset.y / camera2D.zoom;
        // float bottomEdge = camera2D.target.y + (screenH - camera2D.offset.y) / camera2D.zoom;

        // // Horizontal clamp
        // if (rightEdge - leftEdge >= worldW)
        //     camera2D.target.x = (worldMinX + worldMaxX) / 2.0f;
        // else if (leftEdge < worldMinX)
        //     camera2D.target.x += worldMinX - leftEdge;
        // else if (rightEdge > worldMaxX)
        //     camera2D.target.x -= rightEdge - worldMaxX;

        // // Vertical clamp
        // if (bottomEdge - topEdge >= worldH)
        //     camera2D.target.y = (worldMinY + worldMaxY) / 2.0f;
        // else if (topEdge < worldMinY)
        //     camera2D.target.y += worldMinY - topEdge;
        // else if (bottomEdge > worldMaxY)
        //     camera2D.target.y -= bottomEdge - worldMaxY;
    }

    void Camera::reset()
    {
        camera2D.offset = {540.0f, 600.0f};
        camera2D.target = {200.0f, 22.0f};
        camera2D.rotation = 0.0f;
        camera2D.zoom = 1.5f;
    }

} // namespace physim
