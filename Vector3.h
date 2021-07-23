﻿#pragma once
#include <cmath>

class Vector3
{
public:
    float x, y, z;

    // コンストラクタ
    Vector3() {}

    // コピーコンストラクタ(Vector3)
    Vector3(const Vector3 &a) : x(a.x), y(a.y), z(a.z) {}

    // コピーコンストラクタ(3値)
    Vector3(float nx, float ny, float nz) : x(nx), y(ny), z(nz) {}

    // 代入
    Vector3 &operator=(const Vector3 &a)
    {
        x = a.x;
        y = a.y;
        z = a.z;
        return *this;
    }

    bool operator==(const Vector3 &a) const
    {
        return x == a.x && y == a.y && z == a.z;
    }

    bool operator!=(const Vector3 &a) const
    {
        return x != a.x || y != a.y || z != a.z;
    }

    void zero()
    {
        x = y = z = 0;
    }

    Vector3 operator-() const
    {
        return Vector3(-x, -y, -z);
    }

    Vector3 operator+(const Vector3 &a) const
    {
        return Vector3(x + a.x, y + a.y, z + a.z);
    }

    Vector3 operator-(const Vector3 &a) const
    {
        return Vector3(x - a.x, y - a.y, z - a.z);
    }

    Vector3 operator*(float a) const
    {
        return Vector3(x * a, y * a, z * a);
    }

    Vector3 operator/(float a) const
    {
        float oneOverA = 1.0f / a;
        return Vector3(x * oneOverA, y * oneOverA, z * oneOverA);
    }

    Vector3 &operator+=(const Vector3 &a)
    {
        x += a.x;
        y += a.y;
        z += a.z;
        return *this;
    }

    Vector3 &operator-=(const Vector3 &a)
    {
        x -= a.x;
        y -= a.y;
        z -= a.z;
        return *this;
    }

    Vector3 &operator*=(float a)
    {
        x *= a;
        y *= a;
        z *= a;
        return *this;
    }

    Vector3 &operator/=(float a)
    {
        float oneOverA = 1.0f / a;
        x *= oneOverA;
        y *= oneOverA;
        z *= oneOverA;
        return *this;
    }

    void normalize()
    {
        float magSq = x * x + y * y + z * z;
        if (magSq > 0.0f)
        {
            float oneOverMag = 1.0f / sqrt(magSq);
            x *= oneOverMag;
            y *= oneOverMag;
            z *= oneOverMag;
        }
    }

    float operator*(const Vector3 &a) const
    {
        return x * a.x + y * a.y + z * a.z;
    }
};

// 非メンバ関数

// ベクトルの大きさ
inline float vectorMag(const Vector3 &a)
{
    return sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
}

// 外積
inline Vector3 cross(const Vector3 &a, const Vector3 &b)
{
    return Vector3(a.y * b.z - b.y * a.z, a.z * b.x - b.z * a.x, a.x * b.y - b.x * a.y);
}

// スカラー
inline Vector3 operator*(float k, const Vector3 &v)
{
    return Vector3(k * v.x, k * v.y, k * v.z);
}

// 距離
inline float distance(const Vector3 &a, const Vector3 &b)
{
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    float dz = a.z - b.z;
    return sqrt(dx * dx + dy * dy + dz * dz);
}

extern const Vector3 kZeroVector;
