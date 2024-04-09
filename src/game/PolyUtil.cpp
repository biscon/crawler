//
// Created by bison on 12-12-22.
//

#include <algorithm>
#include <SDL_log.h>
#include "PolyUtil.h"
#include <random>

namespace Game {

    bool PointInPolygon(Vector2 point, const std::vector<Vector2> &points) {
        int i, j, nvert = (i32) points.size();
        bool c = false;

        for (i = 0, j = nvert - 1; i < nvert; j = i++) {
            if (((points[i].y >= point.y) != (points[j].y >= point.y)) &&
                (point.x <=
                 (points[j].x - points[i].x) * (point.y - points[i].y) / (points[j].y - points[i].y) + points[i].x)
                    )
                c = !c;
        }

        return c;
    }

    bool PointInCircle(const Vector2 &point, const Vector2 &pos, float radius) {
        float d = fabsf(Distance(pos, point));
        return d < radius;
    }

// Returns 1 if the lines intersect, otherwise 0. In addition, if the lines
// intersect the intersection point may be stored in the floats i_x and i_y.
// from Andrew LaMothes old black book
    INTERNAL bool lineLineIntersection(float p0_x, float p0_y, float p1_x, float p1_y,
                                       float p2_x, float p2_y, float p3_x, float p3_y, float *i_x, float *i_y) {
        float s1_x, s1_y, s2_x, s2_y;
        s1_x = p1_x - p0_x;
        s1_y = p1_y - p0_y;
        s2_x = p3_x - p2_x;
        s2_y = p3_y - p2_y;

        float s, t;
        s = (-s1_y * (p0_x - p2_x) + s1_x * (p0_y - p2_y)) / (-s2_x * s1_y + s1_x * s2_y);
        t = (s2_x * (p0_y - p2_y) - s2_y * (p0_x - p2_x)) / (-s2_x * s1_y + s1_x * s2_y);

        if (s >= 0 && s <= 1 && t >= 0 && t <= 1) {
            // Collision detected
            if (i_x != nullptr)
                *i_x = p0_x + (t * s1_x);
            if (i_y != nullptr)
                *i_y = p0_y + (t * s1_y);
            return true;
        }

        return false; // No collision
    }


    bool
    PolyLineIntersection(const std::vector<Vector2> &polygon, Vector2 rayStart, Vector2 rayEnd,
                         float &x, float &y, Vector2 &normal) {

        float dist = INFINITY;
        Vector2 tmpPos;
        Vector2 tmpNormal;
        //SDL_Log("PolyLineIntersection");
        Vector2 lineStart, lineEnd;
        for (i32 i = 0; i < polygon.size(); ++i) {
            lineStart = polygon[i];
            if (i == polygon.size() - 1) {
                lineEnd = polygon[0];
            } else {
                lineEnd = polygon[i + 1];
            }
            //SDL_Log("lineStart: %.2f,%.2f lineEnd: %.2f,%.2f", lineStart.x, lineStart.y, lineEnd.x, lineEnd.y);
            bool hit = lineLineIntersection(rayStart.x, rayStart.y, rayEnd.x, rayEnd.y, lineStart.x, lineStart.y,
                                            lineEnd.x, lineEnd.y, &x, &y);
            if (hit) {
                float d = Distance(rayStart, Vector2(x, y));
                if (d < dist) {
                    dist = d;
                    // if we define dx=x2-x1 and dy=y2-y1, then the normals are (-dy, dx) and (dy, -dx)
                    tmpPos.x = x;
                    tmpPos.y = y;
                    float dx = lineEnd.x - lineStart.x;
                    float dy = lineEnd.y - lineStart.y;
                    tmpNormal.x = dy;
                    tmpNormal.y = -dx;
                    tmpNormal.normalize();
                }
                //SDL_Log("detected hit at: %.2f,%.2f, normal = %.2f,%.2f", x, y, normal.x, normal.y);
            }
        }
        if (dist < INFINITY) {
            x = tmpPos.x;
            y = tmpPos.y;
            normal = tmpNormal;
            return true;
        }
        return false;
    }

    bool LineLineIntersection(Vector2 lineStart, Vector2 lineEnd, Vector2 rayStart, Vector2 rayEnd,
                              float &x, float &y, Vector2 &normal) {

        bool hit = lineLineIntersection(rayStart.x, rayStart.y, rayEnd.x, rayEnd.y, lineStart.x, lineStart.y, lineEnd.x,
                                        lineEnd.y, &x, &y);
        if (hit) {
            float dx = lineEnd.x - lineStart.x;
            float dy = lineEnd.y - lineStart.y;
            normal.x = dy;
            normal.y = -dx;
            normal.normalize();
            return true;
        }
        return false;
    }

// Calculating Distance pythagoras
    float Distance(const Vector2 &a, const Vector2 &b) {
        return sqrtf(powf(b.x - a.x, 2) + powf(b.y - a.y, 2) * 1.0f);
    }

    float AngleBetweenVectors(const Vector2 &a, const Vector2 &b) {
        float theta = atan2f(a.x * b.y - a.y * b.x, a.x * b.x + a.y * b.y);
        //return theta;
        return fabsf(theta);
    }

    float RadiansToDegrees(float radians) {
        return radians * (180.0f / PI);
    }

/*
 * Decompiled from C sharp, included with unity. Some fancy timestep independent lerp function
 */
    float
    SmoothDamp(float current, float target, float &currentVelocity, float smoothTime, float maxSpeed, float deltaTime) {
        smoothTime = std::fmax(0.0001f, smoothTime);
        float num = 2.0f / smoothTime;
        float num2 = num * deltaTime;
        float num3 = 1.0f / (1.0f + num2 + 0.48f * num2 * num2 + 0.235f * num2 * num2 * num2);
        float num4 = current - target;
        float num5 = target;
        float num6 = maxSpeed * smoothTime;
        num4 = std::clamp(num4, -num6, num6);
        target = current - num4;
        float num7 = (currentVelocity + num * num4) * deltaTime;
        currentVelocity = (currentVelocity - num * num7) * num3;
        float num8 = target + (num4 + num7) * num3;
        if (num5 - current > 0.0f == num8 > num5) {
            num8 = num5;
            currentVelocity = (num8 - num5) / deltaTime;
        }
        return num8;
    }

    INTERNAL void pushVertex(const Vector2 &v, const Renderer::Color &c, std::vector<float> &vertices) {
        vertices.emplace_back(v.x);
        vertices.emplace_back(v.y);
        vertices.emplace_back(c.r);
        vertices.emplace_back(c.g);
        vertices.emplace_back(c.b);
        vertices.emplace_back(c.a);
        vertices.emplace_back(0.0f);
        vertices.emplace_back(0.0f);
    }

    INTERNAL void updateVertex(const Vector2 &v, const Renderer::Color &c, std::vector<float> &vertices, u32 &index) {
        vertices[index] = v.x;
        vertices[index + 1] = v.y;
        vertices[index + 2] = c.r;
        vertices[index + 3] = c.g;
        vertices[index + 4] = c.b;
        vertices[index + 5] = c.a;
        vertices[index + 6] = 0.0f;
        vertices[index + 7] = 0.0f;
        index += 8;
    }

    void BuildTorusTriangleStripMesh(const Vector2 &translate, float inner, float outer, u32 pts,
                                     const Renderer::Color &innerColor, const Renderer::Color &outerColor,
                                     std::vector<float> &vertices) {
        auto fpts = (float) pts;
        for (u32 i = 1; i <= pts; i += 1) {
            float angle = (i / fpts) * PI * 2.0f;
            Vector2 v0(inner * cosf(angle), inner * sinf(angle));
            Vector2 v1(outer * cosf(angle), outer * sinf(angle));

            v0 += translate;
            v1 += translate;

            pushVertex(v0, innerColor, vertices);
            pushVertex(v1, outerColor, vertices);
        }

        Vector2 v0(vertices[0], vertices[1]);
        Vector2 v1(vertices[8], vertices[9]);
        pushVertex(v0, innerColor, vertices);
        pushVertex(v1, outerColor, vertices);
    }

    void UpdateTorusTriangleStripMesh(const Vector2 &translate, float inner, float outer, u32 pts,
                                      const Renderer::Color &innerColor, const Renderer::Color &outerColor,
                                      std::vector<float> &vertices) {
        auto fpts = (float) pts;
        u32 index = 0;
        for (u32 i = 1; i <= pts; i += 1) {
            float angle = (i / fpts) * PI * 2.0f;
            Vector2 v0(inner * cosf(angle), inner * sinf(angle));
            Vector2 v1(outer * cosf(angle), outer * sinf(angle));

            v0 += translate;
            v1 += translate;

            updateVertex(v0, innerColor, vertices, index);
            updateVertex(v1, outerColor, vertices, index);
        }

        Vector2 v0(vertices[0], vertices[1]);
        Vector2 v1(vertices[8], vertices[9]);
        updateVertex(v0, innerColor, vertices, index);
        updateVertex(v1, outerColor, vertices, index);
    }

    void ExtendLine(Vector2 &p1, Vector2 &p2, float amount) {
        float dx = p2.x - p1.x;
        float dy = p2.y - p1.y;
        float length = sqrt(dx * dx + dy * dy);
        float extension = amount / length;
        p1.x -= extension * dx;
        p1.y -= extension * dy;
        p2.x += extension * dx;
        p2.y += extension * dy;
    }

    void ExtendLineDirection(Vector2 &p1, Vector2 &p2, float amount) {
        float dx = p2.x - p1.x;
        float dy = p2.y - p1.y;
        float length = sqrt(dx * dx + dy * dy);
        float extension = amount / length;
        p2.x += extension * dx;
        p2.y += extension * dy;
    }


    std::random_device rd;
    std::mt19937 gen(rd());

    float RandomFloatRange(float min, float max) {
        std::uniform_real_distribution<float> distribution(min, max);
        return distribution(gen);
    }

    void GeneratePlaneTileMesh() {

    }
}