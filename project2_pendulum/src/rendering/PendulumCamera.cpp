#include "PendulumCamera.h"

#include "imgui.h"

#include <algorithm>
#include <cmath>

namespace physim
{
    void PendulumCamera::update(float frameTime)
    {
        constexpr float MIN_ZOOM = 10.0f;
        constexpr float MAX_ZOOM = 600.0f;
        constexpr float KEYBOARD_PAN_SPEED = 600.0f;
        constexpr float KEYBOARD_ZOOM_SPEED = 1.75f;

        const bool keyboardAvailable = !ImGui::GetIO().WantCaptureKeyboard && !ImGui::GetIO().WantTextInput;

        if (!ImGui::GetIO().WantCaptureMouse && IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        {
            Vector2 delta = GetMouseDelta();
            delta.x *= -1.0f / camera2D.zoom;
            delta.y *= -1.0f / camera2D.zoom;
            camera2D.target.x += delta.x;
            camera2D.target.y += delta.y;
        }

        if (keyboardAvailable)
        {
            Vector2 keyboardPan = {0.0f, 0.0f};

            if (IsKeyDown(KEY_A))
            {
                keyboardPan.x -= 1.0f;
            }
            if (IsKeyDown(KEY_D))
            {
                keyboardPan.x += 1.0f;
            }
            if (IsKeyDown(KEY_W))
            {
                keyboardPan.y -= 1.0f;
            }
            if (IsKeyDown(KEY_S))
            {
                keyboardPan.y += 1.0f;
            }

            camera2D.target.x += (keyboardPan.x * KEYBOARD_PAN_SPEED * frameTime) / camera2D.zoom;
            camera2D.target.y += (keyboardPan.y * KEYBOARD_PAN_SPEED * frameTime) / camera2D.zoom;

            float keyboardZoomDirection = 0.0f;
            if (IsKeyDown(KEY_E))
            {
                keyboardZoomDirection += 1.0f;
            }
            if (IsKeyDown(KEY_Q))
            {
                keyboardZoomDirection -= 1.0f;
            }

            if (keyboardZoomDirection != 0.0f)
            {
                const float scale = KEYBOARD_ZOOM_SPEED * keyboardZoomDirection * frameTime;
                camera2D.zoom = std::clamp(std::exp(std::log(camera2D.zoom) + scale), MIN_ZOOM, MAX_ZOOM);
            }
        }

        if (IsKeyPressed(KEY_F))
        {
            reset();
        }

        const float wheel = ImGui::GetIO().WantCaptureMouse ? 0.0f : GetMouseWheelMove();
        if (wheel != 0.0f)
        {
            const Vector2 mouseWorldPosition = GetScreenToWorld2D(GetMousePosition(), camera2D);
            camera2D.offset = GetMousePosition();
            camera2D.target = mouseWorldPosition;

            const float scale = 0.2f * wheel;
            camera2D.zoom = std::clamp(std::exp(std::log(camera2D.zoom) + scale), MIN_ZOOM, MAX_ZOOM);
        }

        camera2D.zoom = std::clamp(camera2D.zoom, MIN_ZOOM, MAX_ZOOM);
    }

    void PendulumCamera::reset()
    {
        camera2D.offset = {static_cast<float>(GetScreenWidth()) * 0.5f, 140.0f};
        camera2D.target = {0.0f, 0.0f};
        camera2D.rotation = 0.0f;
        camera2D.zoom = 160.0f;
    }
} // namespace physim
