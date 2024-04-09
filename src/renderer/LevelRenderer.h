//
// Created by bison on 23-03-24.
//

#ifndef CRAWLER_LEVELRENDERER_H
#define CRAWLER_LEVELRENDERER_H

#include <memory>
#include "ShaderProgram.h"
#include "FrameBuffer.h"
#include "VertexBuffer.h"
#include "TextureAtlas.h"
#include "Camera.h"
#include "Model.h"

#define CUBE_SIZE 3.0f

namespace Renderer {
    enum CubeSide { NORTH, SOUTH, WEST, EAST, TOP, BOTTOM, CENTER};

    struct MeshVertex {
        float position[3];
        float textureCoords[2];
        float color[3];
    };

    struct CubeFaces {
        u32 front;
        u32 back;
        u32 left;
        u32 right;
        u32 top;
        u32 bottom;
    };

    struct Light {
        glm::vec3 position;
        glm::vec3 diffuse;
    };

    enum class BatchType {
        GEOMETRY,
        MODEL,
        SPRITE
    };
    struct RenderBatch {
        BatchType type;
        u32 offset;
        u32 count;
        i32 billboarding;  // 0 = none, 1 = spherical, 2 = cylindrical
        glm::vec3 position;
        glm::vec2 spriteSize;
        u32 modelIndex;
        float modelScale;
        CubeSide modelAlignSide;
        std::string modelObjName;
        glm::vec3 lightColor;
        glm::vec3 transform;
        glm::vec3 rotation;
    };

    struct LevelRenderer {
        std::unique_ptr<ShaderProgram> geometryShader;
        std::unique_ptr<VertexBuffer> geometryVbo;
        TextureAtlas geometryTextureAtlas;
        std::vector<MeshVertex> geometryMesh;

        std::unique_ptr<ShaderProgram> spriteShader;
        std::unique_ptr<VertexBuffer> spriteVbo;
        TextureAtlas spriteTextureAtlas;
        std::vector<MeshVertex> spriteMesh;

        std::unique_ptr<ShaderProgram> modelShader;

        u32 fbo;
        u32 fboTexture;
        u32 wallTexture;
        u32 wallEndTexture;
        u32 ceilingTexture;
        u32 floorTexture;
        u32 drainTexture;
        u32 doorTexture;

        std::vector<RenderBatch> batches;
        std::unique_ptr<Camera> camera;
        std::vector<Model> models;
        std::vector<Light> lights;
        u32 doorModelIndex;
    };

    void InitLevelRenderer(LevelRenderer& r);
    void UpdateLevelRenderer(LevelRenderer& r, float delta);
    void ShutdownLevelRenderer(LevelRenderer& renderer);
    void RenderLevel(LevelRenderer& r, float delta);
    void UploadLevelMesh(LevelRenderer &r);
    u32 LoadModel(LevelRenderer &r, const std::string &filename, const std::string &textureFile);
}

#endif //CRAWLER_LEVELRENDERER_H
