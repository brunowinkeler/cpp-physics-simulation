#include "Camera.h"

#include "imgui.h"

#include <algorithm>
#include <cmath>

namespace physim
{
    void Camera::update()
    {
        constexpr float MIN_ZOOM = 0.02f;
        constexpr float MAX_ZOOM = 64.0f;

        // Translate based on mouse left button drag
        if (!ImGui::GetIO().WantCaptureMouse && IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        {
            Vector2 delta = GetMouseDelta();
            delta.x *= -1.0f / camera2D.zoom;
            delta.y *= -1.0f / camera2D.zoom;
            camera2D.target.x += delta.x;
            camera2D.target.y += delta.y;
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
            camera2D.zoom = std::clamp(std::exp(std::log(camera2D.zoom) + scale), MIN_ZOOM, MAX_ZOOM);
        }

        camera2D.zoom = std::clamp(camera2D.zoom, MIN_ZOOM, MAX_ZOOM);
    }

    void Camera::reset()
    {
        camera2D.offset = {540.0f, 600.0f};
        camera2D.target = {200.0f, 22.0f};
        camera2D.rotation = 0.0f;
        camera2D.zoom = 1.5f;
    }

} // namespace physim
