#include "GasCamera.h"

#include "imgui.h"

#include <algorithm>
#include <cmath>

namespace physim
{
    void GasCamera::update(float frameTime)
    {
        constexpr float MIN_ZOOM = 8.0f;
        constexpr float MAX_ZOOM = 240.0f;
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
            Vector2 keyboardPan{0.0f, 0.0f};
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
    }

    void GasCamera::setTrackedBox(float boxWidth, float boxHeight)
    {
        if (std::abs(trackedBoxWidth - boxWidth) <= 1.0e-6f && std::abs(trackedBoxHeight - boxHeight) <= 1.0e-6f)
        {
            return;
        }

        trackedBoxWidth = boxWidth;
        trackedBoxHeight = boxHeight;
        reset();
    }

    void GasCamera::reset()
    {
        const float screenWidth = static_cast<float>(std::max(GetScreenWidth(), 1));
        const float screenHeight = static_cast<float>(std::max(GetScreenHeight(), 1));
        const float widthScale = screenWidth / std::max(trackedBoxWidth + 2.0f, 1.0f);
        const float heightScale = screenHeight / std::max(trackedBoxHeight + 2.0f, 1.0f);

        camera2D.offset = {screenWidth * 0.5f, screenHeight * 0.5f};
        camera2D.target = {trackedBoxWidth * 0.5f, -trackedBoxHeight * 0.5f};
        camera2D.rotation = 0.0f;
        camera2D.zoom = std::clamp(0.9f * std::min(widthScale, heightScale), 8.0f, 240.0f);
    }
} // namespace physim
