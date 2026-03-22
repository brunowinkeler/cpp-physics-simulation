#include "Camera.h"

#include "imgui.h"
#include "raymath.h"

namespace physim
{
    void Camera::update()
    {
        // Translate based on mouse right click
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

            // Zoom increment
            float scale = 0.2f * wheel;
            camera2D.zoom = Clamp(expf(logf(camera2D.zoom) + scale), 0.125f, 64.0f);
        }
    }

    void Camera::reset()
    {
        camera2D.offset = {540.0f, 600.0f};
        camera2D.target = {200.0f, 22.0f};
        camera2D.rotation = 0.0f;
        camera2D.zoom = 1.5f;
    }
} // namespace physim
