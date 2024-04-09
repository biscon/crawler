//
// Created by bison on 12-12-22.
//

#ifndef PLATFORMER_POLYUTIL_H
#define PLATFORMER_POLYUTIL_H

#include <vector>
#include <Vector2.h>
#include "../renderer/RenderBuffer.h"

#define PI 3.1415926535897f

namespace Game {
    bool PointInPolygon(Vector2 point, const std::vector<Vector2> &points);

    bool PointInCircle(const Vector2 &point, const Vector2 &pos, float radius);

    bool PolyLineIntersection(const std::vector<Vector2> &polygon, Vector2 rayStart, Vector2 rayEnd, float &x, float &y,
                              Vector2 &normal);

    bool LineLineIntersection(Vector2 lineStart, Vector2 lineEnd, Vector2 rayStart, Vector2 rayEnd,
                              float &x, float &y, Vector2 &normal);

    float Distance(const Vector2 &a, const Vector2 &b);

    float AngleBetweenVectors(const Vector2 &a, const Vector2 &b);

    float RadiansToDegrees(float radians);

    float
    SmoothDamp(float current, float target, float &currentVelocity, float smoothTime, float maxSpeed, float deltaTime);

    void BuildTorusTriangleStripMesh(const Vector2 &translate, float inner, float outer, u32 pts,
                                     const Renderer::Color &innerColor, const Renderer::Color &outerColor,
                                     std::vector<float> &vertices);

    void UpdateTorusTriangleStripMesh(const Vector2 &translate, float inner, float outer, u32 pts,
                                      const Renderer::Color &innerColor, const Renderer::Color &outerColor,
                                      std::vector<float> &vertices);

    void ExtendLine(Vector2 &p1, Vector2 &p2, float amount);

    void ExtendLineDirection(Vector2 &p1, Vector2 &p2, float amount);

    float RandomFloatRange(float min, float max);
}
#endif //PLATFORMER_POLYUTIL_H
