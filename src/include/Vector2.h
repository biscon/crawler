//
// Created by bison on 06-01-23.
//

#ifndef PLATFORMER_VECTOR2_H
#define PLATFORMER_VECTOR2_H

#include <cmath>
#include "defs.h"

struct Vector2 {
    float x{}, y{};

    Vector2() = default;
    Vector2(float x, float y) : x(x), y(y) {}
    Vector2(const Vector2& other) {
        x = other.x;
        y = other.y;
    }

    Vector2 operator+(const Vector2& other) const {
        return Vector2(x + other.x, y + other.y);
    }

    Vector2 operator-(const Vector2& other) const {
        return Vector2(x - other.x, y - other.y);
    }

    Vector2 operator*(const Vector2& other) const {
        return Vector2(x * other.x, y * other.y);
    }

    Vector2 operator/(const Vector2& other) const {
        return Vector2(x / other.x, y / other.y);
    }

    Vector2 operator-() const {
        return Vector2(-x, -y);
    }

    Vector2 operator*(float s) const {
        return Vector2(x * s, y * s);
    }

    Vector2 operator/(float s) const {
        return Vector2(x / s, y / s);
    }

    void operator+=(const Vector2& other) {
        *this = *this + other;
    }

    void operator-=(const Vector2& other) {
        *this = *this - other;
    }

    void operator*=(float s) {
        *this = Vector2(x * s, y * s);
    }

    void operator/=(float s) {
        *this = Vector2(x / s, y / s);
    }

    void operator*=(const Vector2& other) {
        *this = *this * other;
    }

    void operator/=(const Vector2& other) {
        *this = *this / other;
    }

    float dot(const Vector2& other) const {
        return x * other.x + y * other.y;
    }

    float cross(const Vector2& other) const {
        return x * other.y - y * other.x;
    }

    float lengthSquared() const {
        return x * x + y * y;
    }

    float length() const {
        return sqrtf(lengthSquared());
    }

    Vector2 normalized() const {
        return *this * (1.0 / length());
    }

    void normalize() {
        *this = normalized();
    }

    float distance(const Vector2& other) {
        return sqrtf(powf(other.x - x, 2) + powf(other.y - y, 2) * 1.0f);
    }

    void zero() {
        x = 0;
        y = 0;
    }

    void set(float xval, float yval) {
        x = xval;
        y = yval;
    }
};

struct UIntRect {
    u32 x{},y{},w{},h{};

    UIntRect() = default;
    UIntRect(u32 x, u32 y, u32 w, u32 h) : x(x), y(y), w(w), h(h) {}
};

struct FloatRect {
    FloatRect() = default;
    FloatRect(float left, float top, float right, float bottom) : left(left), top(top),
                                                                  right(right),
                                                                  bottom(bottom) {}

    FloatRect(const FloatRect& r) {
        left = r.left;
        right = r.right;
        top = r.top;
        bottom = r.bottom;
    }


    inline float width() const {
        return right - left;
    }

    inline float height() const {
        return bottom - top;
    }

    void scale(float scale) {
        left *= scale;
        top *= scale;
        right *= scale;
        bottom *= scale;
    }

    void round() {
        left = roundf(left);
        top = roundf(top);
        right = roundf(right);
        bottom = roundf(bottom);
    }

    void expand(float pad) {
        left -= pad;
        right += pad;
        top -= pad;
        bottom += pad;
    }

    void add(float n) {
        left += n;
        right += n;
        top += n;
        bottom += n;
    }

    void subtract(float n) {
        left -= n;
        right -= n;
        top -= n;
        bottom -= n;
    }

    void translate(float x, float y) {
        left += x;
        top += y;
        right += x;
        bottom += y;
    }

    void zero() {
        left = right = top = bottom = 0;
    }

    bool containsPoint(float px, float py) {
        return px >= left && px <= right && py >= top && py <= bottom;
    }

    float centerX() const {
        return left + width()/2;
    }

    float centerY() const {
        return top + height()/2;
    }

    float                           left{};
    float                           top{};
    float                           right{};
    float                           bottom{};
};


struct UIntPos {
    UIntPos(u32 x, u32 y) : x(x), y(y) {}
    u32 x,y;
};


#endif //PLATFORMER_VECTOR2_H
