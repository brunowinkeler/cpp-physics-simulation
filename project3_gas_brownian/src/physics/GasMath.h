#ifndef GAS_MATH_H
#define GAS_MATH_H

#include <cmath>

namespace physim
{
    struct GasVector2
    {
        float x;
        float y;
    };

    inline GasVector2 operator+(const GasVector2 &left, const GasVector2 &right)
    {
        return {left.x + right.x, left.y + right.y};
    }

    inline GasVector2 operator-(const GasVector2 &left, const GasVector2 &right)
    {
        return {left.x - right.x, left.y - right.y};
    }

    inline GasVector2 operator*(const GasVector2 &vector, float scalar)
    {
        return {vector.x * scalar, vector.y * scalar};
    }

    inline GasVector2 operator/(const GasVector2 &vector, float scalar)
    {
        return {vector.x / scalar, vector.y / scalar};
    }

    inline GasVector2 &operator+=(GasVector2 &left, const GasVector2 &right)
    {
        left.x += right.x;
        left.y += right.y;
        return left;
    }

    inline GasVector2 &operator-=(GasVector2 &left, const GasVector2 &right)
    {
        left.x -= right.x;
        left.y -= right.y;
        return left;
    }

    inline float gasDot(const GasVector2 &left, const GasVector2 &right)
    {
        return (left.x * right.x) + (left.y * right.y);
    }

    inline float gasLengthSquared(const GasVector2 &vector)
    {
        return gasDot(vector, vector);
    }

    inline float gasLength(const GasVector2 &vector)
    {
        return std::sqrt(gasLengthSquared(vector));
    }

    inline GasVector2 gasNormalize(const GasVector2 &vector)
    {
        const float length = gasLength(vector);
        if (length <= 1.0e-6f)
        {
            return {1.0f, 0.0f};
        }

        return vector / length;
    }
} // namespace physim

#endif // GAS_MATH_H
