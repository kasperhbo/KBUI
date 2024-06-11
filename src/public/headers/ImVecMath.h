//
// Created by Kasper de bruin on 10/06/2024.
//

#ifndef KBUI_IMVECMATH_H
#define KBUI_IMVECMATH_H

#define IM_VEC4_CLASS_EXTRA                                             \
    ImVec4 operator+(const ImVec4& rhs) const { return ImVec4(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w); } \
    ImVec4 operator-(const ImVec4& rhs) const { return ImVec4(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w); } \
    ImVec4 operator/(float scalar) const { return ImVec4(x / scalar, y / scalar, z / scalar, w / scalar); } \
    ImVec4& operator+=(const ImVec4& rhs) { x += rhs.x; y += rhs.y; z += rhs.z; w += rhs.w; return *this; } \
    ImVec4& operator-=(const ImVec4& rhs) { x -= rhs.x; y -= rhs.y; z -= rhs.z; w -= rhs.w; return *this; } \
    ImVec4& operator*=(float scalar) { x *= scalar; y *= scalar; z *= scalar; w *= scalar; return *this; } \
    ImVec4& operator/=(float scalar) { x /= scalar; y /= scalar; z /= scalar; w /= scalar; return *this; } \
    bool operator==(const ImVec4& rhs) const { return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w; } \
    bool operator!=(const ImVec4& rhs) const { return !(*this == rhs); }  \
    float dot(const ImVec4& rhs) const { return x * rhs.x + y * rhs.y + z * rhs.z + w * rhs.w; }             \
    ImVec4 operator*(float scalar) const { return ImVec4(x * scalar, y * scalar, z * scalar, w * scalar); }  \
    ImVec4 operator*(const ImVec4& rhs) const { return ImVec4(x * rhs.x, y * rhs.y, z * rhs.z, w * rhs.w); } \
    ImVec4 operator/(const ImVec4& rhs) const { return ImVec4(x / rhs.x, y / rhs.y, z / rhs.z, w / rhs.w); } \
    ImVec4 operator-() const { return ImVec4(-x, -y, -z, -w); }         \
    ImVec4 lerp(const ImVec4& rhs, float t) const { return *this + (rhs - *this) * t; }                      \


 #endif //KBUI_IMVECMATH_H



