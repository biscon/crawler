//
// Created by bison on 29-03-24.
//

#include <algorithm>
#include <cstdio>
#include <utility>
#include <SDL_log.h>
#include "Level.h"
#include <glm/gtx/rotate_vector.hpp>

namespace Game {
    static void meshCell(Level& r, float x, float y, float z, CubeFaces& faces, std::vector<MeshVertex>& vertices) {
        // Calculate half size for centering
        float halfSize = CUBE_SIZE / 2.0f;

        glm::vec3 n;
        auto uvRect = FloatRect(0.0f, 0.0f, 1.0f, 1.0f);
        // Front face
        if (faces.front != WallTexture::NONE) {
            n = glm::vec3(0.0f, 0.0f, 1.0f);
            auto l = (float) faces.front;

            vertices.emplace_back(MeshVertex{{-halfSize + x, -halfSize + y, -halfSize + z}, {uvRect.left, uvRect.bottom, l}, n});
            vertices.emplace_back(MeshVertex{{halfSize + x, -halfSize + y, -halfSize + z}, {uvRect.right, uvRect.bottom, l}, n});
            vertices.emplace_back(MeshVertex{{halfSize + x, halfSize + y, -halfSize + z}, {uvRect.right, uvRect.top, l}, n});
            vertices.emplace_back(MeshVertex{{halfSize + x, halfSize + y, -halfSize + z}, {uvRect.right, uvRect.top, l}, n});
            vertices.emplace_back(MeshVertex{{-halfSize + x, halfSize + y, -halfSize + z}, {uvRect.left, uvRect.top, l}, n});
            vertices.emplace_back(MeshVertex{{-halfSize + x, -halfSize + y, -halfSize + z}, {uvRect.left, uvRect.bottom, l}, n});
        }

        // Back face
        if (faces.back != WallTexture::NONE) {
            n = glm::vec3(0.0f, 0.0f, -1.0f);
            auto l = (float) faces.back;
            vertices.emplace_back(MeshVertex{{-halfSize + x, -halfSize + y, halfSize + z}, {uvRect.left, uvRect.bottom, l}, n});
            vertices.emplace_back(MeshVertex{{-halfSize + x, halfSize + y, halfSize + z}, {uvRect.left, uvRect.top, l}, {n.x, n.y, n.z}});
            vertices.emplace_back(MeshVertex{{halfSize + x, halfSize + y, halfSize + z}, {uvRect.right, uvRect.top, l}, {n.x, n.y, n.z}});
            vertices.emplace_back(MeshVertex{{halfSize + x, halfSize + y, halfSize + z}, {uvRect.right, uvRect.top, l}, {n.x, n.y, n.z}});
            vertices.emplace_back(MeshVertex{{halfSize + x, -halfSize + y, halfSize + z}, {uvRect.right, uvRect.bottom, l}, {n.x, n.y, n.z}});
            vertices.emplace_back(MeshVertex{{-halfSize + x, -halfSize + y, halfSize + z}, {uvRect.left, uvRect.bottom, l}, {n.x, n.y, n.z}});
        }

        // Left face
        if (faces.left != WallTexture::NONE) {
            n = glm::vec3(1.0f, 0.0f, 0.0f);
            auto l = (float) faces.left;
            vertices.emplace_back(MeshVertex{{-halfSize + x, halfSize + y, halfSize + z}, {uvRect.left, uvRect.top, l}, {n.x, n.y, n.z}});
            vertices.emplace_back(MeshVertex{{-halfSize + x, -halfSize + y, halfSize + z}, {uvRect.left, uvRect.bottom, l}, {n.x, n.y, n.z}});
            vertices.emplace_back(MeshVertex{{-halfSize + x, -halfSize + y, -halfSize + z}, {uvRect.right, uvRect.bottom, l}, {n.x, n.y, n.z}});
            vertices.emplace_back(MeshVertex{{-halfSize + x, -halfSize + y, -halfSize + z}, {uvRect.right, uvRect.bottom, l}, {n.x, n.y, n.z}});
            vertices.emplace_back(MeshVertex{{-halfSize + x, halfSize + y, -halfSize + z}, {uvRect.right, uvRect.top, l}, {n.x, n.y, n.z}});
            vertices.emplace_back(MeshVertex{{-halfSize + x, halfSize + y, halfSize + z}, {uvRect.left, uvRect.top, l}, {n.x, n.y, n.z}});
        }

        // Right face
        if (faces.right != WallTexture::NONE) {
            n = glm::vec3(-1.0f, 0.0f, 0.0f);
            auto l = (float) faces.right;
            vertices.emplace_back(MeshVertex{{halfSize + x, halfSize + y, halfSize + z}, {uvRect.left, uvRect.top, l}, {n.x, n.y, n.z}});
            vertices.emplace_back(MeshVertex{{halfSize + x, halfSize + y, -halfSize + z}, {uvRect.right, uvRect.top, l}, {n.x, n.y, n.z}});
            vertices.emplace_back(MeshVertex{{halfSize + x, -halfSize + y, -halfSize + z}, {uvRect.right, uvRect.bottom, l}, {n.x, n.y, n.z}});
            vertices.emplace_back(MeshVertex{{halfSize + x, -halfSize + y, -halfSize + z}, {uvRect.right, uvRect.bottom, l}, {n.x, n.y, n.z}});
            vertices.emplace_back(MeshVertex{{halfSize + x, -halfSize + y, halfSize + z}, {uvRect.left, uvRect.bottom, l}, {n.x, n.y, n.z}});
            vertices.emplace_back(MeshVertex{{halfSize + x, halfSize + y, halfSize + z}, {uvRect.left, uvRect.top, l}, {n.x, n.y, n.z}});
        }

        // Bottom face
        if (faces.bottom != WallTexture::NONE) {
            n = glm::vec3(0.0f, 1.0f, 0.0f);
            auto l = (float) faces.bottom;
            vertices.emplace_back(MeshVertex{{-halfSize + x, -halfSize + y, -halfSize + z}, {uvRect.left, uvRect.top, l}, {n.x, n.y, n.z}});
            vertices.emplace_back(MeshVertex{{-halfSize + x, -halfSize + y, halfSize + z}, {uvRect.left, uvRect.bottom, l}, {n.x, n.y, n.z}});
            vertices.emplace_back(MeshVertex{{halfSize + x, -halfSize + y, halfSize + z}, {uvRect.right, uvRect.bottom, l}, {n.x, n.y, n.z}});
            vertices.emplace_back(MeshVertex{{halfSize + x, -halfSize + y, halfSize + z}, {uvRect.right, uvRect.bottom, l}, {n.x, n.y, n.z}});
            vertices.emplace_back(MeshVertex{{halfSize + x, -halfSize + y, -halfSize + z}, {uvRect.right, uvRect.top, l}, {n.x, n.y, n.z}});
            vertices.emplace_back(MeshVertex{{-halfSize + x, -halfSize + y, -halfSize + z}, {uvRect.left, uvRect.top, l}, {n.x, n.y, n.z}});
        }

        // Top face
        if (faces.top != WallTexture::NONE) {
            n = glm::vec3(0.0f, -1.0f, 0.0f);
            auto l = (float) faces.top;
            vertices.emplace_back(MeshVertex{{-halfSize + x, halfSize + y, -halfSize + z}, {uvRect.left, uvRect.top, l}, {n.x, n.y, n.z}});
            vertices.emplace_back(MeshVertex{{halfSize + x, halfSize + y, -halfSize + z}, {uvRect.right, uvRect.top, l}, {n.x, n.y, n.z}});
            vertices.emplace_back(MeshVertex{{halfSize + x, halfSize + y, halfSize + z}, {uvRect.right, uvRect.bottom, l}, {n.x, n.y, n.z}});
            vertices.emplace_back(MeshVertex{{halfSize + x, halfSize + y, halfSize + z}, {uvRect.right, uvRect.bottom, l}, {n.x, n.y, n.z}});
            vertices.emplace_back(MeshVertex{{-halfSize + x, halfSize + y, halfSize + z}, {uvRect.left, uvRect.bottom, l}, {n.x, n.y, n.z}});
            vertices.emplace_back(MeshVertex{{-halfSize + x, halfSize + y, -halfSize + z}, {uvRect.left, uvRect.top, l}, {n.x, n.y, n.z}});
        }
    }
    
    static WallTexture charToWallTexture(LevelRenderer& r, u8 c) {
        switch(c) {
            case '#': return WallTexture::WALL;
        }
        return WallTexture::NONE;
    }

    static bool isOpenCell(Level &l, i32 x, i32 y) {
        if(x < 0 || x >= l.width || y < 0 || y >= l.height) {
            return false;
        }
        auto index = y * l.width + x;
        //return l.map[index] == ' ' || l.map[index] == 'P' || l.map[index] == 'L' || l.map[index] == 'M';
        return l.map[index] != '#';
    }

    static void buildCellMesh(Level &l, Renderer::LevelRenderer& r, DepthSortedObject& dso) {
        auto x = dso.mapX;
        auto y = dso.mapY;

        // Determine neighboring cells
        char leftCell = (x > 0) ? l.map[y * l.width + (x - 1)] : '#';
        char rightCell = (x < l.width - 1) ? l.map[y * l.width + (x + 1)] : '#';
        char frontCell = (y > 0) ? l.map[(y - 1) * l.width + x] : '#';
        char backCell = (y < l.height - 1) ? l.map[(y + 1) * l.width + x] : '#';
        char aboveCell = (y > 0) ? l.map[(y - 1) * l.width + x] : '#';
        char belowCell = (y < l.height - 1) ? l.map[(y + 1) * l.width + x] : '#';

        // Determine wall conditions
        Renderer::CubeFaces faces = {};
        /*
        faces.top = (aboveCell == '#') ? 1 : 0;
        faces.bottom = (belowCell == '#') ? 1 : 0;
         */
        faces.top = WallTexture::CEILING;
        faces.bottom = WallTexture::FLOOR;
        faces.left = charToWallTexture(r, leftCell);
        faces.right = charToWallTexture(r, rightCell);
        faces.front = charToWallTexture(r, frontCell);
        faces.back = charToWallTexture(r, backCell);

        // Call meshCell function
        meshCell(l, dso.worldPosition.x, dso.worldPosition.y, dso.worldPosition.z, faces, r.geometryMesh);
    }

    static void normalizeResolution(int width, int height, float scale, float* normalizedWidth, float* normalizedHeight) {
        // Calculate the normalized values
        *normalizedWidth = (float)width / (float)(width > height ? width : height);
        *normalizedHeight = (float)height / (float)(width > height ? width : height);
        // Apply the scaling factor
        *normalizedWidth *= scale;
        *normalizedHeight *= scale;
    }

    static CellSide getFacingSide(const glm::vec3& cameraFront, const glm::vec2& mobDir) {
        auto camForward = glm::vec2(-cameraFront.x, -cameraFront.z);

        float cosAngle = glm::dot(camForward, mobDir);
        if (cosAngle > 0.707) { // cos(45°) = 0.707
            return CellSide::NORTH;
        } else if (cosAngle < -0.707) {
            return CellSide::SOUTH;
        }

        // use glm::cross to determine if mob is to the left or right of camera
        float cross = (camForward.x * mobDir.y - camForward.y * mobDir.x);
        if (cross < 0.0f ) {
            return CellSide::WEST;
        } else if (cross > 0.0f ) {
            return CellSide::EAST;
        } else {
            return CellSide::EAST;
        }
    }

    static void calcWorldPosition(Level& l, SpriteEntity& sprite, DepthSortedObject& dso) {
        auto x = sprite.x;
        auto y = sprite.y;
        dso.mapX = x;
        dso.mapY = y;
        float worldX = ((float) x) * CUBE_SIZE;
        float worldZ = ((float) y) * CUBE_SIZE;
        dso.worldPosition = glm::vec3(worldX, 0.0f, worldZ);

        // place sprite at floor
        auto halfCubeSize = CUBE_SIZE / 2.0f;
        dso.worldPosition.y = -halfCubeSize + (sprite.size.y / 2.0f);
        switch(sprite.corner) {
            case CellCorner::NW:
                dso.worldPosition += glm::vec3(-halfCubeSize + (sprite.size.x / 2.0f), 0.0f, -halfCubeSize + (sprite.size.x / 2.0f));
                break;
            case CellCorner::NE:
                dso.worldPosition += glm::vec3(halfCubeSize - (sprite.size.x / 2.0f), 0.0f, -halfCubeSize + (sprite.size.x / 2.0f));
                break;
            case CellCorner::SW:
                dso.worldPosition += glm::vec3(-halfCubeSize + (sprite.size.x / 2.0f), 0.0f, halfCubeSize - (sprite.size.x / 2.0f));
                break;
            case CellCorner::SE:
                dso.worldPosition += glm::vec3(halfCubeSize - (sprite.size.x / 2.0f), 0.0f, halfCubeSize - (sprite.size.x / 2.0f));
                break;
            case CellCorner::CENTER:
                break;
        }
    }

    static void buildSpriteMesh(Level &l, Renderer::LevelRenderer& r, DepthSortedObject& dso, glm::vec2& size, u32 texture, CellAxis axis) {
        auto x = dso.worldPosition.x;
        auto y = dso.worldPosition.y;
        auto z = dso.worldPosition.z;
        float halfWidth = size.x / 2.0f; // Half of the width of the rectangle
        float halfHeight = size.y / 2.0f; // Half of the height of the rectangle

        // Front face
        auto uvRect = r.spriteTextureAtlas.uvRects[texture];

        if(axis == CellAxis::CELL_AXIS_XY) {
            r.spriteMesh.emplace_back(SpriteVertex{{-halfWidth + x, -halfHeight + y, z}, {uvRect.left, uvRect.bottom}});
            r.spriteMesh.emplace_back(SpriteVertex{{-halfWidth + x, halfHeight + y, z}, {uvRect.left, uvRect.top}});
            r.spriteMesh.emplace_back(SpriteVertex{{halfWidth + x, halfHeight + y, z}, {uvRect.right, uvRect.top}});
            r.spriteMesh.emplace_back(SpriteVertex{{halfWidth + x, halfHeight + y, z}, {uvRect.right, uvRect.top}});
            r.spriteMesh.emplace_back(SpriteVertex{{halfWidth + x, -halfHeight + y, z}, {uvRect.right, uvRect.bottom}});
            r.spriteMesh.emplace_back(SpriteVertex{{-halfWidth + x, -halfHeight + y, z}, {uvRect.left, uvRect.bottom}});
        }
        if(axis == CellAxis::CELL_AXIS_ZY) { // rotated 90 degrees about the y axis
            r.spriteMesh.emplace_back(SpriteVertex{{x, -halfHeight + y, halfWidth + z}, {uvRect.left, uvRect.bottom}});
            r.spriteMesh.emplace_back(SpriteVertex{{x, halfHeight + y, halfWidth + z}, {uvRect.left, uvRect.top}});
            r.spriteMesh.emplace_back(SpriteVertex{{x, halfHeight + y, -halfWidth + z}, {uvRect.right, uvRect.top}});
            r.spriteMesh.emplace_back(SpriteVertex{{x, halfHeight + y, -halfWidth + z}, {uvRect.right, uvRect.top}});
            r.spriteMesh.emplace_back(SpriteVertex{{x, -halfHeight + y, -halfWidth + z}, {uvRect.right, uvRect.bottom}});
            r.spriteMesh.emplace_back(SpriteVertex{{x, -halfHeight + y, halfWidth + z}, {uvRect.left, uvRect.bottom}});
        }
    }

    bool depthSortedComparator(const DepthSortedObject& a, const DepthSortedObject& b) {
        // First, compare by distance
        if (a.distanceToCamera != b.distanceToCamera) {
            return a.distanceToCamera > b.distanceToCamera;
        } else {
            // If distances are equal, compare by DSOType
            // The order will be: GEOMETRY < DOOR < SPRITE
            if (a.type != b.type) {
                return a.type < b.type;
            }
        }
        return false; // If both distance and type are equal, no need to swap
    }


    static void buildMapMesh(Level &l, Renderer::LevelRenderer& r) {
        l.depthSortedObjects.clear();
        r.lights.clear();
        for (int y = 0; y < l.height; y++) {
            for (int x = 0; x < l.width; x++) {
                // Get the current cell
                char currentCell = (i8) l.map[y * l.width + x];
                // add geometry
                if(isOpenCell(l, x, y)) {
                    float worldX = ((float) x) * CUBE_SIZE;
                    float worldZ = ((float) y) * CUBE_SIZE;
                    auto depthSortedObject = DepthSortedObject{};
                    depthSortedObject.type = DSOType::GEOMETRY;
                    depthSortedObject.worldPosition = glm::vec3(worldX, 0.0f, worldZ);
                    depthSortedObject.mapX = x;
                    depthSortedObject.mapY = y;
                    depthSortedObject.distanceToCamera = glm::distance(r.camera->Position, depthSortedObject.worldPosition);
                    l.depthSortedObjects.push_back(depthSortedObject);
                }
                // add lights
                if(std::isdigit(currentCell)) {
                    i32 level = currentCell - '0';
                    Renderer::Light light{};
                    light.level = level;
                    light.position = glm::vec3(((float) x) * CUBE_SIZE, CUBE_SIZE / 4.0f, ((float) y) * CUBE_SIZE);
                    GetLightByLevel(light, level);
                    light.shadowEnabled = level >= 2;
                    r.lights.push_back(light);
                }
            }
        }
        //SDL_Log("Camera at %f, %f, %f\n", r.camera->Position.x, r.camera->Position.y, r.camera->Position.z);
        //r.lights[0].position = r.camera->Position;
        //r.lights[0].position.y = 1.25f;

        // add monsters
        for(auto& m : l.monsters) {
            auto depthSortedObject = DepthSortedObject{};
            depthSortedObject.type = DSOType::SPRITE;
            calcWorldPosition(l, m, depthSortedObject);
            auto sortPos = glm::vec3(((float) m.x) * CUBE_SIZE, 0.0f, ((float) m.y) * CUBE_SIZE);
            depthSortedObject.distanceToCamera = glm::distance(r.camera->Position, sortPos);
            depthSortedObject.sprite = &m;
            l.depthSortedObjects.push_back(depthSortedObject);
        }

        // add objects
        for(auto& o : l.objects) {
            auto depthSortedObject = DepthSortedObject{};
            depthSortedObject.type = DSOType::SPRITE;
            calcWorldPosition(l, o, depthSortedObject);
            auto sortPos = glm::vec3(((float) o.x) * CUBE_SIZE, 0.0f, ((float) o.y) * CUBE_SIZE);
            depthSortedObject.distanceToCamera = glm::distance(r.camera->Position, sortPos);
            depthSortedObject.sprite = &o;
            l.depthSortedObjects.push_back(depthSortedObject);
        }

        // add doors
        for(auto& d : l.doors) {
            auto depthSortedObject = DepthSortedObject{};
            depthSortedObject.type = DSOType::DOOR;
            auto x = d.x;
            auto y = d.y;
            float worldX = ((float) x) * CUBE_SIZE;
            float worldZ = ((float) y) * CUBE_SIZE;
            depthSortedObject.worldPosition = glm::vec3(worldX, 0.0f, worldZ);
            depthSortedObject.mapX = x;
            depthSortedObject.mapY = y;
            depthSortedObject.distanceToCamera = glm::distance(r.camera->Position, depthSortedObject.worldPosition);
            depthSortedObject.door = &d;
            l.depthSortedObjects.push_back(depthSortedObject);
        }

        // add 3d models
        for(auto& m : l.modelInstances) {
            auto depthSortedObject = DepthSortedObject{};
            depthSortedObject.type = DSOType::MODEL;
            auto sortPos = glm::vec3(((float) m.x) * CUBE_SIZE, 0.0f, ((float) m.y) * CUBE_SIZE);
            depthSortedObject.worldPosition = sortPos;
            depthSortedObject.distanceToCamera = glm::distance(r.camera->Position, sortPos);
            depthSortedObject.model = &m;
            depthSortedObject.mapX = m.x;
            depthSortedObject.mapY = m.y;
            l.depthSortedObjects.push_back(depthSortedObject);
        }

        // Sort depth sorted objects
        std::sort(l.depthSortedObjects.begin(), l.depthSortedObjects.end(), depthSortedComparator);
        // Generate render batches from depth sorted objects, painters algorithm (back to front)
        r.batches.clear();
        DSOType lastType = DSOType::GEOMETRY;
        for(auto& dso : l.depthSortedObjects) {
            switch(dso.type) {
                case DSOType::GEOMETRY: {
                    RenderBatch batch{};
                    batch.type = BatchType::GEOMETRY;
                    batch.offset = r.geometryMesh.size();
                    batch.position = dso.worldPosition;
                    buildCellMesh(l, r, dso);
                    batch.count = r.geometryMesh.size() - batch.offset;
                    r.batches.push_back(batch);
                    break;
                }
                case DSOType::SPRITE: {
                    RenderBatch batch{};
                    batch.type = BatchType::SPRITE;
                    batch.offset = r.spriteMesh.size();
                    batch.billboarding = 1;

                    batch.position = dso.worldPosition;
                    batch.spriteSize = dso.sprite->size;
                    batch.spriteNormal = glm::vec3(dso.sprite->direction.x, 0.0f, dso.sprite->direction.y);
                    auto side = CellSide::NORTH;
                    if(!dso.sprite->uniDirectional) {
                        side = getFacingSide(r.camera->Front, dso.sprite->direction);
                    }
                    //SDL_Log("SpriteEntity at %d, %d, facing %d\n", dso.mapX, dso.mapY, side);
                    buildSpriteMesh(l, r, dso, batch.spriteSize, dso.sprite->textures[side], CellAxis::CELL_AXIS_XY);
                    batch.count = r.spriteMesh.size() - batch.offset;
                    r.batches.push_back(batch);
                    break;
                }
                case DSOType::DOOR: {
                    glm::vec3 rotation;
                    if(dso.door->axis == CellAxis::CELL_AXIS_XY) {
                        rotation = glm::vec3(0.0f, 00.0f, 0.0f);
                    }
                    if(dso.door->axis == CellAxis::CELL_AXIS_ZY) {
                        rotation = glm::vec3(0.0f, -90.0f, 0.0f);
                    }
                    // frame
                    RenderBatch frameBatch{};
                    frameBatch.type = BatchType::MODEL;
                    frameBatch.modelIndex = dso.door->modelIndex;
                    frameBatch.position = dso.worldPosition;
                    frameBatch.modelScale = 1.0f;
                    frameBatch.modelAlignSide = CubeSide::BOTTOM;
                    frameBatch.modelObjName = "frame";
                    frameBatch.transform = glm::vec3(0.0f, 0.0f, 0.0f);
                    frameBatch.rotation = rotation;
                    r.batches.push_back(frameBatch);
                    // door
                    RenderBatch doorBatch{};
                    doorBatch.type = BatchType::MODEL;
                    doorBatch.modelIndex = dso.door->modelIndex;
                    doorBatch.position = dso.worldPosition;
                    doorBatch.modelScale = 1.0f;
                    doorBatch.modelAlignSide = CubeSide::BOTTOM;
                    doorBatch.modelObjName = "door";
                    doorBatch.transform = glm::vec3(0.0f, dso.door->offsetY, 0.0f);
                    doorBatch.rotation = rotation;
                    r.batches.push_back(doorBatch);
                    break;
                }
                case DSOType::MODEL: {
                    RenderBatch batch{};
                    batch.type = BatchType::MODEL;
                    batch.modelIndex = dso.model->modelIndex;
                    batch.position = dso.worldPosition;
                    batch.modelScale = dso.model->scale;
                    batch.modelAlignSide = dso.model->alignSide;
                    batch.modelObjName = "*";
                    r.batches.push_back(batch);
                    break;
                }
            }
            lastType = dso.type;
        }
    }

    static void setPlayerPosition(Level &l, Renderer::LevelRenderer& r) {
        int width = l.width;
        int height = l.height;
        // Iterate over the map
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                float worldX = ((float) x) * CUBE_SIZE;
                float worldZ = ((float) y) * CUBE_SIZE;
                // Get the current cell
                char currentCell = l.map[y * width + x];

                if(currentCell == 'P') {
                    printf("Setting camera position\n");
                    l.player.x = x;
                    l.player.y = y;
                    l.player.direction = glm::vec2(0.0f, 1.0f);
                    r.camera->Position = glm::vec3(worldX, 0.0f, worldZ);
                    //r.camera->Yaw = 90.0f;
                    return;
                }
            }
        }
    }

    static void loadMonsterBluePrints(Level& l, LevelRenderer& r, TextureAtlasBuilder& builder) {
        i32 w,h;
        for(auto& [symbol, bluePrint] : l.monsterBluePrints) {
            bluePrint.base.textures[CellSide::NORTH] = builder.addFromPngSize(bluePrint.base.textureFile + "_front.png", true, w, h);
            bluePrint.base.textures[CellSide::SOUTH] = builder.addFromPngSize(bluePrint.base.textureFile + "_back.png", true, w, h);
            bluePrint.base.textures[CellSide::WEST] = builder.addFromPngSize(bluePrint.base.textureFile + "_left.png", true, w, h);
            bluePrint.base.textures[CellSide::EAST] = builder.addFromPngSize(bluePrint.base.textureFile + "_right.png", true, w, h);
        }
    }

    static void getRandomDirection(glm::vec2& direction) {
        int r = rand() % 4;
        switch(r) {
            case 0: direction = North; break;
            case 1: direction = South; break;
            case 2: direction = West; break;
            case 3: direction = East; break;
            default: direction = North; break;
        }
    }

    static CellCorner getRandomCorner() {
        i32 r = rand() % 5;
        switch(r) {
            case 0: return NW;
            case 1: return NE;
            case 2: return SW;
            case 3: return SE;
            case 4: return CENTER;
            default: return NW;
        }
    }

    static void spawnMonsters(Level& l) {
        for(auto& [symbol, bluePrint] : l.monsterBluePrints) {
            for (int y = 0; y < l.height; y++) {
                for (int x = 0; x < l.width; x++) {
                    // Get the current cell
                    char currentCell = l.map[y * l.width + x];
                    if(currentCell == bluePrint.base.mapSymbol) {
                        SpriteEntity m{};
                        m.x = x;
                        m.y = y;
                        m.corner = getRandomCorner();
                        glm::vec2 mobDir;
                        getRandomDirection(mobDir);
                        m.direction = mobDir;
                        m.textures[CellSide::NORTH] = bluePrint.base.textures[CellSide::NORTH];
                        m.textures[CellSide::SOUTH] = bluePrint.base.textures[CellSide::SOUTH];
                        m.textures[CellSide::WEST] = bluePrint.base.textures[CellSide::WEST];
                        m.textures[CellSide::EAST] = bluePrint.base.textures[CellSide::EAST];
                        normalizeResolution(bluePrint.base.frameWidth, bluePrint.base.frameHeight, bluePrint.base.scale, &m.size.x, &m.size.y);
                        l.monsters.push_back(m);
                        SDL_Log("Spawned monster at %d, %d\n", x, y);
                    }
                }
            }
        }
    }

    static void spawnObjects(Level& l) {
        for(auto& [symbol, bluePrint] : l.objectBluePrints) {
            for (int y = 0; y < l.height; y++) {
                for (int x = 0; x < l.width; x++) {
                    // Get the current cell
                    char currentCell = l.map[y * l.width + x];
                    if(currentCell == bluePrint.base.mapSymbol) {
                        SpriteEntity obj{};
                        obj.x = x;
                        obj.y = y;
                        obj.corner = getRandomCorner();
                        glm::vec2 objDir;
                        getRandomDirection(objDir);
                        obj.direction = objDir;
                        if(bluePrint.base.dirSymbol == '*') {
                            obj.uniDirectional = true;
                            obj.textures[CellSide::NORTH] = bluePrint.base.textures[CellSide::NORTH];
                        } else {
                            obj.uniDirectional = false;
                            obj.textures[CellSide::NORTH] = bluePrint.base.textures[CellSide::NORTH];
                            obj.textures[CellSide::SOUTH] = bluePrint.base.textures[CellSide::SOUTH];
                            obj.textures[CellSide::WEST] = bluePrint.base.textures[CellSide::WEST];
                            obj.textures[CellSide::EAST] = bluePrint.base.textures[CellSide::EAST];
                        }
                        normalizeResolution(bluePrint.base.frameWidth, bluePrint.base.frameHeight, bluePrint.base.scale, &obj.size.x, &obj.size.y);
                        l.monsters.push_back(obj);
                        SDL_Log("Spawned object at %d, %d\n", x, y);
                    }
                }
            }
        }
    }

    static void loadObjectBluePrints(Level& l, LevelRenderer& r, TextureAtlasBuilder& builder) {
        i32 w,h;
        for(auto& [symbol, bluePrint] : l.objectBluePrints) {
            if(bluePrint.base.dirSymbol == '*') {
                bluePrint.base.textures[CellSide::NORTH] = builder.addFromPngSize(bluePrint.base.textureFile + ".png", true, w, h);
            } else {
                bluePrint.base.textures[CellSide::NORTH] = builder.addFromPngSize(bluePrint.base.textureFile + "_front.png", true, w, h);
                bluePrint.base.textures[CellSide::SOUTH] = builder.addFromPngSize(bluePrint.base.textureFile + "_back.png", true, w, h);
                bluePrint.base.textures[CellSide::WEST] = builder.addFromPngSize(bluePrint.base.textureFile + "_left.png", true, w, h);
                bluePrint.base.textures[CellSide::EAST] = builder.addFromPngSize(bluePrint.base.textureFile + "_right.png", true, w, h);
            }
        }
    }

    static void spawnDoors(Level& l, u32 doorModelIndex) {
        for (int y = 0; y < l.height; y++) {
            for (int x = 0; x < l.width; x++) {
                // Get the current cell
                char currentCell = l.map[y * l.width + x];
                if(currentCell == 'D' || currentCell == 'd') {
                    Door d{};
                    d.x = x;
                    d.y = y;
                    if(currentCell == 'D') {
                        d.axis = CellAxis::CELL_AXIS_XY;
                    } else {
                        d.axis = CellAxis::CELL_AXIS_ZY;
                    }
                    d.open = false;
                    d.opening = false;
                    d.modelIndex = doorModelIndex;
                    d.elapsed = 0.0f;
                    d.duration = 5.0f;
                    d.offsetY = 0.0f;
                    d.targetOffsetY = -CUBE_SIZE + 0.075f;
                    l.doors.push_back(d);
                    SDL_Log("Spawned door at %d, %d\n", x, y);
                }
            }
        }
    }

    void LoadLevel(Level& level, LevelRenderer& renderer, const u8 map[], i32 w, i32 h) {
        // seed rand
        srand(time(nullptr));
        level.monsters.clear();
        level.objects.clear();
        level.modelInstances.clear();
        level.doors.clear();
        level.freeCam = false;
        level.width = w;
        level.height = h;
        level.moveDuration = 0.35f;
        level.turnDuration = 0.25f;
        auto mapSize = level.width * level.height;
        
        level.map.clear();
        level.map.resize(mapSize);
        // copy map
        for (i32 i = 0; i < mapSize; i++) {
            level.map[i] = map[i];
        }
        setPlayerPosition(level, renderer);
        auto builder = Renderer::TextureAtlasBuilder(1024, 1024, Renderer::PixelFormat::RGBA);
        loadMonsterBluePrints(level, renderer, builder);
        loadObjectBluePrints(level, renderer, builder);

        builder.build(renderer.spriteTextureAtlas);
        spawnDoors(level, renderer.doorModelIndex);
        spawnMonsters(level);
        spawnObjects(level);
    }

    void ShutdownLevel(Level &level) {

    }

    /*
    static void adjustCamera(Level &level, LevelRenderer& renderer) {
        float cubeSize = CUBE_SIZE;
        float worldX = ((float) level.player.x) * cubeSize;
        float worldZ = ((float) level.player.y) * cubeSize;
        renderer.camera->Yaw = level.player.angle;
        renderer.camera->Pitch = 0.0f;
        renderer.camera->Position = glm::vec3(worldX, 0.0f, worldZ);
        renderer.camera->updateCameraVectors();
    }
    */
    static void updateDoors(Level &level, LevelRenderer& renderer, float delta) {
        for(auto& d : level.doors) {
            if(d.opening || d.closing) {
                bool done = false;
                d.elapsed += delta;
                if(d.elapsed >= d.duration) {
                    d.elapsed = d.duration;
                    done = true;
                }
                float t = d.elapsed / d.duration;
                if(d.opening) {
                    d.offsetY = t * d.targetOffsetY;
                }
                if(d.closing) {
                    t = 1.0f - t;
                    d.offsetY = t * d.targetOffsetY;
                }
                if(done) {
                    d.open = !d.open;
                    d.opening = false;
                    d.closing = false;
                }
            }
        }
    }


    void UpdateLevel(Level &level, LevelRenderer& renderer, float delta) {
        if(!level.freeCam) {
            renderer.camera->Position.y = 0.25f;
        }
        updateDoors(level, renderer, delta);
        if(!level.freeCam) {
            //adjustCamera(level, renderer);
        }
        renderer.geometryMesh.clear();
        renderer.spriteMesh.clear();
        buildMapMesh(level, renderer);
        UploadLevelMesh(renderer);
    }

    void MoveForward(Level &level, Camera& c) {
        c.AnimateMove(Camera_Movement::FORWARD, level.moveDuration, CUBE_SIZE);
        level.player.x -= (i32) level.player.direction.x;
        level.player.y -= (i32) level.player.direction.y;
        SDL_Log("Player at %d, %d\n", level.player.x, level.player.y);
        SDL_Log("Camera at %f, %f, %f\n", c.Position.x, c.Position.y, c.Position.z);
    }

    void MoveBackward(Level &level, Camera& c) {
        c.AnimateMove(Camera_Movement::BACKWARD, level.moveDuration, CUBE_SIZE);
        level.player.x += (i32) level.player.direction.x;
        level.player.y += (i32) level.player.direction.y;
        SDL_Log("Player at %d, %d\n", level.player.x, level.player.y);
        SDL_Log("Camera at %f, %f, %f\n", c.Position.x, c.Position.y, c.Position.z);
    }

    void MoveLeft(Level &level, Camera& c) {
        c.AnimateMove(Camera_Movement::LEFT, level.moveDuration, CUBE_SIZE);
        auto rotated = glm::rotate(level.player.direction, glm::radians(90.0f));
        level.player.x += (i32) rotated.x;
        level.player.y += (i32) rotated.y;
        SDL_Log("Player at %d, %d\n", level.player.x, level.player.y);
        SDL_Log("Camera at %f, %f, %f\n", c.Position.x, c.Position.y, c.Position.z);
    }

    void MoveRight(Level &level, Camera& c) {
        c.AnimateMove(Camera_Movement::RIGHT, level.moveDuration, CUBE_SIZE);
        auto rotated = glm::rotate(level.player.direction, glm::radians(-90.0f));
        level.player.x += (i32) rotated.x;
        level.player.y += (i32) rotated.y;
        SDL_Log("Player at %d, %d\n", level.player.x, level.player.y);
        SDL_Log("Camera at %f, %f, %f\n", c.Position.x, c.Position.y, c.Position.z);
    }

    void TurnLeft(Level &level, Camera& c) {
        c.AnimateMove(Camera_Movement::TURN_LEFT, level.turnDuration, 0.0f);
        level.player.direction = glm::rotate(level.player.direction, glm::radians(-90.0f));
        SDL_Log("Player direction: %f, %f\n", level.player.direction.x, level.player.direction.y);
    }

    void TurnRight(Level &level, Camera& c) {
        c.AnimateMove(Camera_Movement::TURN_RIGHT, level.turnDuration, 0.0f);
        level.player.direction = glm::rotate(level.player.direction, glm::radians(90.0f));
        SDL_Log("Player direction: %f, %f\n", level.player.direction.x, level.player.direction.y);
    }

    void CreateMonsterBluePrint(Level& level, i8 mapSymbol, i8 dirSymbol, std::string textureFile, i32 fW, i32 fH, float scale) {
        MonsterBluePrint m{};
        m.base.dirSymbol = dirSymbol;
        m.base.mapSymbol = mapSymbol;
        m.base.textureFile = std::move(textureFile);
        m.base.textures[0] = 0;
        m.base.textures[1] = 0;
        m.base.textures[2] = 0;
        m.base.textures[3] = 0;
        m.base.frameWidth = fW;
        m.base.frameHeight = fH;
        m.base.scale = scale;
        level.monsterBluePrints[mapSymbol] = m;
    }

    void CreateObjectBluePrint(Level& level, i8 mapSymbol, i8 dirSymbol, std::string textureFile, i32 fW, i32 fH, float scale) {
        ObjectBluePrint m{};
        m.base.dirSymbol = dirSymbol;
        m.base.mapSymbol = mapSymbol;
        m.base.textureFile = std::move(textureFile);
        m.base.textures[0] = 0;
        m.base.textures[1] = 0;
        m.base.textures[2] = 0;
        m.base.textures[3] = 0;
        m.base.frameWidth = fW;
        m.base.frameHeight = fH;
        m.base.scale = scale;
        level.objectBluePrints[mapSymbol] = m;
    }

    void CreateModelInstance(Level &level, i32 x, i32 y, CubeSide alignSide, float scale, u32 modelIndex) {
        ModelInstance m{};
        m.x = x;
        m.y = y;
        m.alignSide = alignSide;
        m.scale = scale;
        m.modelIndex = modelIndex;
        level.modelInstances.push_back(m);
    }

    void OpenDoor(Level &level) {
        // calculate x,y in front of player
        i32 x = level.player.x - (i32) level.player.direction.x;
        i32 y = level.player.y - (i32) level.player.direction.y;
        SDL_Log("Player at %d, %d\n", level.player.x, level.player.y);
        SDL_Log("Try opening door at %d, %d\n", x, y);
        for(auto& d : level.doors) {
            if(d.x == x && d.y == y && !d.opening && !d.closing) {
                if(!d.open) {
                    d.opening = true;
                    d.closing = false;
                    SDL_Log("Opening door\n");
                    d.elapsed = 0.0f;
                    d.duration = 1.0f;
                    d.offsetY = 0.0f;
                    d.targetOffsetY = -CUBE_SIZE + 0.075f;
                } else {
                    d.closing = true;
                    d.opening = false;
                    SDL_Log("Closing door\n");
                    d.elapsed = 0.0f;
                    d.duration = 1.0f;
                    d.offsetY = 0.0f;
                    d.targetOffsetY = -CUBE_SIZE + 0.075f;
                }
            }
        }
    }

    /*
    void MoveForward(Level &level) {
        i32 x = level.player.x;
        i32 y = level.player.y;
        float angle = level.player.angle;
        float dx = cosf(angle * M_PI / 180.0f);
        float dy = sinf(angle * M_PI / 180.0f);
        SDL_Log("dx: %f, dy: %f\n", dx, dy);
        x += (i32) dx;
        y += (i32) dy;
        if(level.map[y * level.width + x] != '#') {
            level.player.x = x;
            level.player.y = y;
        }
    }

    void MoveBackward(Level &level) {
        i32 x = level.player.x;
        i32 y = level.player.y;
        float angle = level.player.angle;
        float dx = cosf(angle * M_PI / 180.0f);
        float dy = sinf(angle * M_PI / 180.0f);
        SDL_Log("dx: %f, dy: %f\n", dx, dy);
        x -= (i32) dx;
        y -= (i32) dy;
        if(level.map[y * level.width + x] != '#') {
            level.player.x = x;
            level.player.y = y;
        }
    }

    void MoveLeft(Level &level) {
        i32 x = level.player.x;
        i32 y = level.player.y;
        float angle = level.player.angle;
        float targetAngle = angle - 90.0f;
        if(targetAngle < 0.0f) {
            targetAngle += 360.0f;
        }
        float dx = cosf(targetAngle * M_PI / 180.0f);
        float dy = sinf(targetAngle * M_PI / 180.0f);
        SDL_Log("dx: %f, dy: %f\n", dx, dy);
        x += (i32) dx;
        y += (i32) dy;
        if(level.map[y * level.width + x] != '#') {
            level.player.x = x;
            level.player.y = y;
        }
    }

    void MoveRight(Level &level) {
        i32 x = level.player.x;
        i32 y = level.player.y;
        float angle = level.player.angle;
        float targetAngle = angle + 90.0f;
        if(targetAngle >= 360.0f) {
            targetAngle -= 360.0f;
        }
        float dx = cosf(targetAngle * (M_PI / 180.0f));
        float dy = sinf(targetAngle * (M_PI / 180.0f));
        SDL_Log("dx: %f, dy: %f\n", dx, dy);
        x += (i32) dx;
        y += (i32) dy;
        if(level.map[y * level.width + x] != '#') {
            level.player.x = x;
            level.player.y = y;
        }
    }

    void TurnLeft(Level &level) {
        level.player.angle -= 90.0f;
        // Normalize angle
        if(level.player.angle < 0.0f) {
            level.player.angle += 360.0f;
        }
    }

    void TurnRight(Level &level) {
        level.player.angle += 90.0f;
        // Normalize angle
        if(level.player.angle >= 360.0f) {
            level.player.angle -= 360.0f;
        }
    }
    */
}