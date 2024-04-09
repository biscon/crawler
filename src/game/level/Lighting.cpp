//
// Created by bison on 30-03-24.
//

#include <cstdio>
#include <algorithm>
#include "Lighting.h"

namespace Game {

    static void printLightMap(Lighting& l) {
        // Print the resulting map
        for (int y = 0; y < l.height; y++) {
            for (int x = 0; x < l.width; x++) {
                printf("%2d ", l.lightMap[y * l.width + x]);
            }
            printf("\n");
        }
        // print light level values
        for(i32 i = 0; i < l.lightLevels.size(); i++) {
            printf("%d: %f\n", i, l.lightLevels[i]);
        }
    }

    static void precomputeLightLevels(Lighting& l, int divisions) {
        float start = 1.0f;
        l.lightLevels.clear();
        if (divisions < 1) {
            return;
        }
        l.lightLevels.push_back(start); // Push the start value into the vector
        for (int i = 1; i < divisions; ++i) {
            start /= 3.5f;
            l.lightLevels.push_back(start);
        }
        // Reverse the vector
        std::reverse(l.lightLevels.begin(), l.lightLevels.end());
    }
    
    // Function to propagate light in the lightMap
    static void propagateLight(Lighting& l, i32 startX, i32 startY, const u8 map[], const u8 blockedMap[]) {
        // Directions for propagation (n, s, w, e, nw, ne, sw, se)
        i32 dx[] = {-1, 1,  0, 0, -1,  1, -1, 1};
        i32 dy[] = { 0, 0, -1, 1, -1, -1,  1, 1};

        i32 front = 0, rear = 0;

        // Add initial seed cell
        l.lightQueue[rear].x = startX;
        l.lightQueue[rear].y = startY;
        rear++;

        while (front < rear) {
            // Dequeue cell
            i32 x = l.lightQueue[front].x;
            i32 y = l.lightQueue[front].y;
            front++;

            // Calculate index for current cell
            i32 index = y * l.width + x;
            u8 currentBrightness = l.lightMap[index];

            // Propagate light to adjacent cells
            for (i32 i = 0; i < 4; i++) {
                i32 nx = x + dx[i];
                i32 ny = y + dy[i];
                i32 newIndex = ny * l.width + nx;

                // Check if the adjacent cell is within bounds
                if (nx >= 0 && nx < l.width && ny >= 0 && ny < l.height) {
                    // Check if the cell is blocked
                    if (blockedMap[newIndex] == 1) {
                        continue;
                    }
                    // Update brightness and add cell to queue
                    if (l.lightMap[newIndex] < currentBrightness - 1) {
                        l.lightMap[newIndex] = currentBrightness - 1;
                        l.lightQueue[rear].x = nx;
                        l.lightQueue[rear].y = ny;
                        rear++;
                    }
                }
            }
        }
    }

    float GetLightLevelAt(Lighting& l, i32 x, i32 y, i32 &count) {
        if (x < 0 || x >= l.width || y < 0 || y >= l.height) {
            return l.lightLevels[0];
        }
        auto lightIndex = l.lightMap[l.width * y + x];
        float level = l.lightLevels[lightIndex];
        if(level <= 0.01f) {
            //level = 0.01f;
        }
        count++;
        return level;
    }

    static float lightScale = 1.50f;

    static inline void setLightColor(float level, glm::vec3& color) {
        level *= lightScale;
        level = glm::clamp(level, 0.0f, 1.0f);
        auto tint = glm::vec3(1.0f, 0.95f, 0.90f);
        //auto tint = glm::vec3(1.0f, 0.922f, 0.735f);
        //auto tint = glm::vec3(1.0f, 1.0f, 1.00f);
        color = tint * glm::vec3(level, level, level);
    }

    void GetLightColorAt(Lighting& l, i32 x, i32 y, glm::vec3& color) {
        i32 count = 0;
        float level = GetLightLevelAt(l, x, y, count);
        setLightColor(level, color);
    }

    // Top-Bottom
    void SetLightColorTopLeft_TB(Lighting& r, i32 x, i32 y, glm::vec3& color) {
        i32 count = 0;
        float level = GetLightLevelAt(r, x, y, count) + GetLightLevelAt(r, x-1, y, count) + GetLightLevelAt(r, x-1, y-1, count) + GetLightLevelAt(r, x, y-1, count);
        level /= (float) count;
        setLightColor(level, color);
    }

    void SetLightColorTopRight_TB(Lighting& l, i32 x, i32 y, glm::vec3& color) {
        i32 count = 0;
        float level = GetLightLevelAt(l, x, y, count) + GetLightLevelAt(l, x+1, y, count) + GetLightLevelAt(l, x+1, y-1, count) + GetLightLevelAt(l, x, y-1, count);
        level /= (float) count;
        setLightColor(level, color);
    }

    void SetLightColorBottomLeft_TB(Lighting& l, i32 x, i32 y, glm::vec3& color) {
        i32 count = 0;
        float level = GetLightLevelAt(l, x, y, count) + GetLightLevelAt(l, x-1, y, count) + GetLightLevelAt(l, x-1, y+1, count) + GetLightLevelAt(l, x, y+1, count);
        level /= (float) count;
        setLightColor(level, color);
    }

    void SetLightColorBottomRight_TB(Lighting& l, i32 x, i32 y, glm::vec3& color) {
        i32 count = 0;
        float level = GetLightLevelAt(l, x, y, count) + GetLightLevelAt(l, x+1, y, count) + GetLightLevelAt(l, x+1, y+1, count) + GetLightLevelAt(l, x, y+1, count);
        level /= (float) count;
        setLightColor(level, color);
    }

    // Left-Right
    void SetLightColorLeft_LR(Lighting& l, i32 x, i32 y, glm::vec3& color) {
        i32 count = 0;
        float level = GetLightLevelAt(l, x, y, count) + GetLightLevelAt(l, x, y+1, count);
        level /= (float) count;
        setLightColor(level, color);
    }

    void SetLightColorRight_LR(Lighting& l, i32 x, i32 y, glm::vec3& color) {
        i32 count = 0;
        float level = GetLightLevelAt(l, x, y, count) + GetLightLevelAt(l, x, y-1, count);
        level /= (float) count;
        setLightColor(level, color);
    }

    // Front-Back
    void SetLightColorLeft_FB(Lighting& l, i32 x, i32 y, glm::vec3& color) {
        i32 count = 0;
        float level = GetLightLevelAt(l, x, y, count) + GetLightLevelAt(l, x-1, y, count);
        level /= (float) count;
        setLightColor(level, color);
    }

    void SetLightColorRight_FB(Lighting& l, i32 x, i32 y, glm::vec3& color) {
        i32 count = 0;
        float level = GetLightLevelAt(l, x, y, count) + GetLightLevelAt(l, x+1, y, count);
        level /= (float) count;
        setLightColor(level, color);
    }

    void BuildLightMap(Lighting& l, const u8 map[], const u8 blockedMap[]) {
        l.lightQueue.clear();
        l.lightQueue.reserve(l.width * l.height);
        l.lightMap.clear();
        l.lightMap.resize(l.width * l.height);
        // Iterate over the map
        for (int y = 0; y < l.height; y++) {
            for (int x = 0; x < l.width; x++) {
                // Get the current cell
                char currentCell = map[y * l.width + x];
                if (currentCell == 'L') {
                    l.lightMap[y * l.width + x] = 7;
                    propagateLight(l, x, y, map, blockedMap);
                }
            }
        }
        //printLightMap(l);
    }
    
    void InitLighting(Lighting &lighting, int32_t w, int32_t h) {
        precomputeLightLevels(lighting, 8);
        lighting.width = w;
        lighting.height = h;
    }
}