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
#include "ShadowCubeMapFBO.h"
#include "frustum/ViewFrustum.h"
#include "ArrayTexture.h"
#include "DepthBufferFBO.h"
#include "GPUTimerQuery.h"
#include "SSAO.h"

#define CUBE_SIZE 3.0f

namespace Renderer {
    enum CubeSide { NORTH, SOUTH, WEST, EAST, TOP, BOTTOM, CENTER};
    enum WallTexture {
        WALL,
        FLOOR,
        CEILING,
        NONE,
    };

    struct MeshVertex {
        glm::vec3 position;
        glm::vec3 textureCoords;
        glm::vec3 normal;
        glm::vec3 tangent;
    };

    struct SpriteVertex {
        glm::vec3 position;
        glm::vec2 textureCoords;
    };

    struct CubeFaces {
        WallTexture front;
        WallTexture back;
        WallTexture left;
        WallTexture right;
        WallTexture top;
        WallTexture bottom;
    };

    struct Light {
        glm::vec3 position;
        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;
        float linear;
        float quadratic;
        bool shadowEnabled;
        u32 level;
    };

    struct RenderLight {
        u32 lightIndex;
        float distance;
        bool shadowEnabled;
        u32 level;
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
        glm::vec3 spriteNormal;
        glm::vec2 spriteSize;
        u32 modelIndex;
        float modelScale;
        CubeSide modelAlignSide;
        std::string modelObjName;
        glm::vec3 lightColor;
        glm::vec3 transform;
        glm::vec3 rotation;
    };

    struct RenderSettings {
        float FOV;
        bool shadowsEnabled;
        u32 shadowMapSize;
        float headTilt;
        bool normalMapping;
        bool specularMapping;
        bool SSAO;
        i32 renderWidth;
        i32 renderHeight;
    };

    struct LevelRenderer {
        RenderSettings settings;
        ViewFrustum frustum;
        std::unique_ptr<ShaderProgram> geometryShader;
        std::unique_ptr<VertexBuffer> geometryVbo;
        std::vector<MeshVertex> geometryMesh;

        std::unique_ptr<ShaderProgram> spriteShader;
        std::unique_ptr<VertexBuffer> spriteVbo;
        TextureAtlas spriteTextureAtlas;
        std::vector<SpriteVertex> spriteMesh;

        std::unique_ptr<ShaderProgram> modelShader;

        std::unique_ptr<ShaderProgram> screenShader;
        std::unique_ptr<VertexBuffer> screenVbo;

        std::unique_ptr<ShaderProgram> shadowShader;
        std::unique_ptr<ShadowCubeMapFBO> shadowFbo1;
        std::unique_ptr<ShadowCubeMapFBO> shadowFbo2;
        std::unique_ptr<ShadowCubeMapFBO> shadowFbo3;
        std::unique_ptr<ShadowCubeMapFBO> shadowFbo4;

        std::unique_ptr<ShaderProgram> depthShader;
        std::unique_ptr<DepthBufferFBO> depthBufferFbo;

        u32 fbo;
        u32 fboTexture;

        ArrayTexture wallDiffuseMaps;
        ArrayTexture wallNormalMaps;
        ArrayTexture wallSpecularMaps;

        std::vector<RenderBatch> batches;
        std::unique_ptr<Camera> camera;
        std::vector<Model> models;
        std::vector<Light> lights;
        std::vector<RenderLight> renderLights;
        u32 doorModelIndex;
        u32 cellsRendered;
        std::unique_ptr<GPUTimerQuery> gpuFrameTimer;
        std::unique_ptr<SSAO> ssao;

    };

    void InitLevelRenderer(LevelRenderer& r);
    void UpdateLevelRenderer(LevelRenderer& r, float delta);
    void ShutdownLevelRenderer(LevelRenderer& renderer);
    void RenderLevel(LevelRenderer& r, float delta);
    void UploadLevelMesh(LevelRenderer &r);
    u32 LoadModel(LevelRenderer &r, const std::string &filename, const std::string &textureFile);
    u32 LoadModel(LevelRenderer &r, const std::string &filename, const std::string &diffuseFile, const std::string &normalFile);
    u32 LoadModel(LevelRenderer &r, const std::string &filename, const std::string &diffuseFile, const std::string &normalFile, const std::string &specularFile);
}

#endif //CRAWLER_LEVELRENDERER_H
