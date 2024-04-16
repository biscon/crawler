//
// Created by bison on 29-03-24.
//

#ifndef CRAWLER_LEVEL_H
#define CRAWLER_LEVEL_H

#include "defs.h"
#include "glm/ext.hpp"
#include "../../renderer/LevelRenderer.h"
#include "Lighting.h"
#include <vector>

#define MOVE_SPEED 5.0f
using Renderer::MeshVertex;
using Renderer::SpriteVertex;
using Renderer::TextureAtlas;
using Renderer::TextureAtlasBuilder;
using Renderer::CubeFaces;
using Renderer::LevelRenderer;
using Renderer::RenderBatch;
using Renderer::BatchType;
using Renderer::CubeSide;
using Renderer::WallTexture;

namespace Game {
    enum CellSide { NORTH, SOUTH, WEST, EAST };
    enum CellCorner { NW, NE, SW, SE, CENTER };
    enum CellAxis { CELL_AXIS_XY, CELL_AXIS_ZY };
    const glm::vec2 North = glm::vec2(0, -1);
    const glm::vec2 South = glm::vec2(0, 1);
    const glm::vec2 West = glm::vec2(-1, 0);
    const glm::vec2 East = glm::vec2(1, 0);

    struct Player {
        i32 x;
        i32 y;
        glm::vec2 direction;
    };

    struct BluePrintBase {
        i32 x;
        i32 y;
        i32 frameWidth;
        i32 frameHeight;
        float scale;
        i8 dirSymbol; // (N, S, W, E)
        i8 mapSymbol;
        std::string textureFile;
        u32 textures[4];
    };

    struct MonsterBluePrint {
        BluePrintBase base;
    };

    struct ObjectBluePrint {
        BluePrintBase base;
    };

    struct Door {
        i32 x;
        i32 y;
        CellAxis axis;
        bool open;
        u32 modelIndex;
        bool opening;
        bool closing;
        float duration;
        float elapsed;
        float offsetY;
        float targetOffsetY;
    };

    struct ModelInstance {
        i32 x;
        i32 y;
        CubeSide alignSide;
        float scale;
        u32 modelIndex;
    };

    struct SpriteEntity {
        i32 x;
        i32 y;
        CellCorner corner;
        glm::vec2 size;
        glm::vec2 direction;
        u32 textures[4];
        bool uniDirectional;
    };

    enum class DSOType {
        GEOMETRY,
        MODEL,
        DOOR,
        SPRITE,
    };

    struct DepthSortedObject {
        DSOType type;
        float distanceToCamera;
        glm::vec3 worldPosition;
        i32 mapX;
        i32 mapY;
        SpriteEntity* sprite;
        Door* door;
        ModelInstance* model;
    };

    struct Level {
        i32 width;
        i32 height;
        Player player;
        std::vector<u8> map;
        bool freeCam;
        float moveDuration;
        float turnDuration;
        std::vector<DepthSortedObject> depthSortedObjects;
        std::vector<SpriteEntity> monsters;
        std::vector<SpriteEntity> objects;
        std::vector<Door> doors;
        std::unordered_map<i8, MonsterBluePrint> monsterBluePrints;
        std::unordered_map<i8, ObjectBluePrint> objectBluePrints;
        std::vector<ModelInstance> modelInstances;
    };

    void LoadLevel(Level& level, LevelRenderer& renderer, const u8 map[], i32 w, i32 h);
    void ShutdownLevel(Level& level);
    void UpdateLevel(Level& level, LevelRenderer& renderer, float delta);
    void MoveForward(Level& level, Camera& c);
    void MoveBackward(Level &level, Camera& c);
    void MoveLeft(Level &level, Camera& c);
    void MoveRight(Level &level, Camera& c);
    void TurnLeft(Level &level, Camera& c);
    void TurnRight(Level &level, Camera& c);
    void CreateMonsterBluePrint(Level& level, i8 mapSymbol, i8 dirSymbol, std::string textureFile, i32 fW, i32 fH, float scale);
    void CreateObjectBluePrint(Level& level, i8 mapSymbol, i8 dirSymbol, std::string textureFile, i32 fW, i32 fH, float scale);
    void CreateModelInstance(Level& level, i32 x, i32 y, CubeSide alignSide, float scale, u32 modelIndex);
    void OpenDoor(Level& level);
}
#endif //CRAWLER_LEVEL_H
