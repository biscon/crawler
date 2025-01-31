//
// Created by bison on 23-03-24.
//

#include <SDL_log.h>
#include <algorithm>
#include "LevelRenderer.h"
#include "glm/ext.hpp"
#include <glm/gtx/rotate_vector.hpp>
#include "Viewport.h"

extern "C" {
#include "glad.h"
}

namespace Renderer {

    const float screenQuadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
            // positions   // texCoords
            -1.0f, 1.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f,
            1.0f, -1.0f, 1.0f, 0.0f,

            -1.0f, 1.0f, 0.0f, 1.0f,
            1.0f, -1.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 1.0f, 1.0f
    };

    struct {
        const std::string model = "model";
        const std::string view = "view";
        const std::string proj = "projection";
    } ShaderUniforms;

    void InitLevelRenderer(LevelRenderer &r) {
        r.settings.FOV = 60.0f;
        r.settings.shadowsEnabled = true;
        r.settings.headTilt = 3.0f;
        r.settings.normalMapping = true;
        r.settings.specularMapping = true;
        r.settings.SSAO = true;
        r.settings.shadowMapSize = 512;
        r.settings.renderWidth = 1920;
        r.settings.renderHeight = 1080;

        r.gpuFrameTimer = std::make_unique<GPUTimerQuery>();

        r.camera = std::make_unique<Camera>(glm::vec3(0.0f, 0.0f, 0.0f));
        SDL_Log("Compiling geometry shader");
        r.geometryShader = std::make_unique<ShaderProgram>("shaders/geometry_vertex.glsl",
                                                           "shaders/geometry_fragment.glsl");
        r.geometryShader->setupUniform(ShaderUniforms.model);
        r.geometryShader->setupUniform(ShaderUniforms.view);
        r.geometryShader->setupUniform(ShaderUniforms.proj);

        SDL_Log("Compiling sprite shader");
        r.spriteShader = std::make_unique<ShaderProgram>("shaders/sprite_vertex.glsl",
                                                         "shaders/sprite_fragment.glsl");

        SDL_Log("Compiling model shader");
        r.modelShader = std::make_unique<ShaderProgram>("shaders/model_vertex.glsl",
                                                         "shaders/model_fragment.glsl");

        SDL_Log("Compiling shadow shader");
        r.shadowShader = std::make_unique<ShaderProgram>("shaders/shadow_vertex.glsl",
                                                        "shaders/shadow_fragment.glsl");

        SDL_Log("Compiling depth shader");
        r.depthShader = std::make_unique<ShaderProgram>("shaders/depth_vertex.glsl",
                                                         "shaders/depth_fragment.glsl");

        r.screenShader = std::make_unique<ShaderProgram>("shaders/screen_vertex.glsl",
                                                         "shaders/screen_fragment.glsl");


        auto vp = GetViewport();
        auto width = (i32) vp.screenWidth;
        auto height = (i32) vp.screenHeight;

        // depth buffer fbo
        r.depthBufferFbo = std::make_unique<DepthBufferFBO>();
        r.depthBufferFbo->init(r.settings.renderWidth, r.settings.renderHeight);

        // fbo for later use

        r.fboTexture = CreateTexture();
        SetFilteringTexture(r.fboTexture, TextureFiltering::NEAREST);
        UploadTexture(r.fboTexture, r.settings.renderWidth, r.settings.renderHeight, nullptr, TextureFormatInternal::RGBA8, TextureFormatData::RGBA);
        r.fbo = CreateFrameBuffer(r.fboTexture, TextureFormatInternal::RGBA8, TextureFormatData::RGBA, r.settings.renderWidth, r.settings.renderHeight);

        // shadow fbo
        r.shadowFbo1 = std::make_unique<ShadowCubeMapFBO>();
        r.shadowFbo1->init(r.settings.shadowMapSize);
        r.shadowFbo2 = std::make_unique<ShadowCubeMapFBO>();
        r.shadowFbo2->init(r.settings.shadowMapSize);
        r.shadowFbo3 = std::make_unique<ShadowCubeMapFBO>();
        r.shadowFbo3->init(r.settings.shadowMapSize);
        r.shadowFbo4 = std::make_unique<ShadowCubeMapFBO>();
        r.shadowFbo4->init(r.settings.shadowMapSize);

        r.wallDiffuseMaps.create(64, 64, 3);
        r.wallDiffuseMaps.uploadLayer("assets/walls/brick_wall2.png", WALL);
        r.wallDiffuseMaps.uploadLayer("assets/walls/wood_floor.png", FLOOR);
        r.wallDiffuseMaps.uploadLayer("assets/walls/wood_floor.png", CEILING);
        r.wallDiffuseMaps.setFilteringTexture(TextureFiltering::NEAREST_MIPMAP);
        r.wallDiffuseMaps.generateMipmaps();

        r.wallNormalMaps.create(64, 64, 3);
        r.wallNormalMaps.uploadLayer("assets/walls/brick_wall2_n.png", WALL);
        r.wallNormalMaps.uploadLayer("assets/walls/wood_floor_n.png", FLOOR);
        r.wallNormalMaps.uploadLayer("assets/walls/wood_floor_n.png", CEILING);
        r.wallNormalMaps.setFilteringTexture(TextureFiltering::NEAREST_MIPMAP);
        r.wallNormalMaps.generateMipmaps();

        r.wallSpecularMaps.create(64, 64, 3);
        r.wallSpecularMaps.uploadLayer("assets/walls/brick_wall2_s.png", WALL);
        r.wallSpecularMaps.uploadLayer("assets/walls/wood_floor_s.png", FLOOR);
        r.wallSpecularMaps.uploadLayer("assets/walls/wood_floor_s.png", CEILING);
        r.wallSpecularMaps.setFilteringTexture(TextureFiltering::NEAREST_MIPMAP);
        r.wallSpecularMaps.generateMipmaps();

        // geometry vbo
        VertexAttributes geometryAttrs;
        geometryAttrs.add(0, 3, VertexAttributeType::Float); // position
        geometryAttrs.add(1, 3, VertexAttributeType::Float); // tex coords
        geometryAttrs.add(2, 3, VertexAttributeType::Float); // normals
        geometryAttrs.add(3, 3, VertexAttributeType::Float); // tangents
        r.geometryVbo = std::make_unique<VertexBuffer>(geometryAttrs);
        
        // sprite vbo
        VertexAttributes spriteAttrs;
        spriteAttrs.add(0, 3, VertexAttributeType::Float); // position
        spriteAttrs.add(1, 2, VertexAttributeType::Float); // tex coords
        r.spriteVbo = std::make_unique<VertexBuffer>(spriteAttrs);

        // setup offscreen rendering VertexBuffer
        VertexAttributes attrs;
        attrs.add(0, 2, VertexAttributeType::Float); // position
        attrs.add(1, 2, VertexAttributeType::Float); // tex coords
        r.screenVbo = std::make_unique<VertexBuffer>(attrs);
        r.screenVbo->allocate((void *) &screenQuadVertices, sizeof(screenQuadVertices),
                                     VertexAccessType::STATIC);

        // setup SSAO
        r.ssao = std::make_unique<SSAO>();
        r.ssao->init(width, height);

        r.doorModelIndex = LoadModel(r, "assets/models/door_frame2.obj", "assets/models/door_frame.png");
    }

    void ShutdownLevelRenderer(LevelRenderer &renderer) {
        DestroyTextureAtlas(renderer.spriteTextureAtlas);
        DestroyFrameBuffer(renderer.fbo);
    }

    static void renderModel(LevelRenderer &r, RenderBatch& batch, bool renderShadows, ShaderProgram& shader) {
        float halfSize = CUBE_SIZE / 2.0f;
        glm::mat4 model = glm::mat4(1.0f);
        // set world pos
        model = glm::translate(model, batch.position);
        // apply transform
        model = glm::translate(model, batch.transform);
        model = glm::rotate(model, glm::radians(batch.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(batch.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(batch.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(batch.modelScale));
        switch(batch.modelAlignSide) {
            case NORTH: {
                model = glm::translate(model, glm::vec3(0.0f, 0.0f, -halfSize));
                break;
            }
            case SOUTH: {
                model = glm::translate(model, glm::vec3(0.0f, 0.0f, halfSize));
                model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                break;
            }
            case WEST: {
                model = glm::translate(model, glm::vec3(-halfSize, 0.0f, 0.0f));
                model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                break;
            }
            case EAST: {
                model = glm::translate(model, glm::vec3(halfSize, 0.0f, 0.0f));
                model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                break;
            }
            case TOP: {
                break;
            }
            case BOTTOM: {
                model = glm::translate(model, glm::vec3(0.0f, -halfSize, 0.0f));
                break;
            }
        }

        //model = glm::translate(model, glm::vec3(0.0f, 0.0f, -halfSize));
        Model &m = r.models[batch.modelIndex];

        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE); // Enable depth buffer writing
        shader.setMat4("model", model);
        if(!renderShadows) {
            bool normalMap = m.hasNormalMap && r.settings.normalMapping;
            bool specularMap = m.hasSpecularMap && r.settings.specularMapping;
            shader.setInt("material.hasNormalMap", normalMap ? 1 : 0);
            shader.setInt("material.hasSpecularMap", specularMap ? 1 : 0);
            glActiveTexture(GL_TEXTURE0);
            BindTexture(m.diffuseMapId);

            if(m.hasNormalMap) {
                glActiveTexture(GL_TEXTURE1);
                BindTexture(m.normalMapId);
            }
            if(m.hasSpecularMap) {
                glActiveTexture(GL_TEXTURE2);
                BindTexture(m.specularMapId);
            }

            // bind depth buffer
            r.depthBufferFbo->bindForReading(GL_TEXTURE6);

            // bind shadow cube map
            r.shadowFbo1->bindForReading(GL_TEXTURE7);
            r.shadowFbo2->bindForReading(GL_TEXTURE8);
            r.shadowFbo3->bindForReading(GL_TEXTURE9);
            r.shadowFbo4->bindForReading(GL_TEXTURE10);
        }
        m.vbo->bind();
        if(batch.modelObjName == "*") {
            glDrawArrays(GL_TRIANGLES, 0, (i32) m.vertices.size());
        }
        else {
            const auto& obj = m.objects[batch.modelObjName];
            glDrawArrays(GL_TRIANGLES, (i32) obj.offset, (i32) obj.count);
        }
        m.vbo->unbind();
        if(!renderShadows) {
            UnbindTexture();
        }
    }

    static void setShaderLights(LevelRenderer &r, ShaderProgram& program) {
        i32 lightIndex = 0;
        for(auto &renderLight : r.renderLights) {
            auto& light = r.lights[renderLight.lightIndex];
            std::string lightIndexStr = std::to_string(lightIndex);
            std::string lightPosStr = "pointLights[" + lightIndexStr + "].position";
            program.setVec3(lightPosStr, light.position);
            std::string lightAmbientStr = "pointLights[" + lightIndexStr + "].ambient";
            program.setVec3(lightAmbientStr, light.ambient);
            std::string lightDiffuseStr = "pointLights[" + lightIndexStr + "].diffuse";
            program.setVec3(lightDiffuseStr, light.diffuse);
            std::string lightSpecularStr = "pointLights[" + lightIndexStr + "].specular";
            program.setVec3(lightSpecularStr, light.specular);


            std::string constantStr = "pointLights[" + lightIndexStr + "].constant";
            program.setFloat(constantStr, 1.0f);
            std::string linearStr = "pointLights[" + lightIndexStr + "].linear";
            program.setFloat(linearStr, light.linear);
            std::string quadraticStr = "pointLights[" + lightIndexStr + "].quadratic";
            program.setFloat(quadraticStr, light.quadratic);

            std::string lightEnabledStr = "pointLights[" + lightIndexStr + "].enabled";
            program.setInt(lightEnabledStr, 1);

            std::string shadowEnabledStr = "pointLights[" + lightIndexStr + "].shadowEnabled";
            program.setInt(shadowEnabledStr, renderLight.shadowEnabled ? 1 : 0);

            if(lightIndex >= 0 && lightIndex < 4) {
                std::string shadowMapStr = "pointLights[" + lightIndexStr + "].shadowCubeMap";
                program.setInt(shadowMapStr, 7 + lightIndex);
            }
            lightIndex++;
            if(lightIndex >= 7) {
                break;
            }
        }
    }


   static CameraDirection gCameraDirections[NUM_OF_LAYERS] =
           {
                   { GL_TEXTURE_CUBE_MAP_POSITIVE_X, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f) },
                   { GL_TEXTURE_CUBE_MAP_NEGATIVE_X, glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f) },
                   { GL_TEXTURE_CUBE_MAP_POSITIVE_Y, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f) },
                   { GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f) },
                   { GL_TEXTURE_CUBE_MAP_POSITIVE_Z, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f) },
                   { GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f) }
           };

    void RenderLevelShadows(LevelRenderer &r, RenderLight& renderLight, ShadowCubeMapFBO& fbo) {
        // create transformations
        glm::mat4 model         = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        glm::mat4 view          = glm::mat4(1.0f);
        glm::mat4 projection    = glm::mat4(1.0f);

        const auto& light = r.lights[renderLight.lightIndex];
        r.shadowShader->use();
        r.shadowShader->setVec3("LightPos", light.position);

        glClearColor(FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX);
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE); // Enable depth buffer writing

        for (u32 i = 0 ; i < 6 ; i++) {
            fbo.bindForWriting(gCameraDirections[i].CubemapFace);
            glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
            view = glm::lookAt(light.position, light.position + gCameraDirections[i].Target, gCameraDirections[i].Up);
            projection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 100.0f);
            r.shadowShader->setMat4("model", model);
            r.shadowShader->setMat4("view", view);
            r.shadowShader->setMat4("projection", projection);
            r.shadowShader->setFloat("FarPlane", 100.0f);

            r.geometryVbo->bind();
            glDrawArrays(GL_TRIANGLES, (i32) 0, (i32) r.geometryMesh.size());
            r.geometryVbo->unbind();
            // render batches
            /*
            for(auto &batch : r.batches) {
                switch(batch.type) {
                    case BatchType::GEOMETRY: {
                        //glCullFace(GL_FRONT);
                        r.geometryVbo->bind();
                        glDrawArrays(GL_TRIANGLES, (i32) batch.offset, (i32) batch.count);
                        r.geometryVbo->unbind();
                        //glCullFace(GL_BACK);
                        break;
                    }
                }
            }
             */
            for(auto &batch : r.batches) {
                switch(batch.type) {
                    case BatchType::MODEL: {
                        renderModel(r, batch, true, *r.shadowShader);
                        break;
                    }
                }
            }
        }
    }

    static float calculateBoundingSphereRadius(float constant, float linear, float quadratic, float intensityThreshold) {
        // Solve quadratic equation
        // ax^2 + bx + c = 0
        float a = quadratic;
        float b = linear;
        float c = constant - 1.0f / intensityThreshold;
        float discriminant = b * b - 4 * a * c;
        if (discriminant < 0) {
            return -1.0f; // No real roots, invalid configuration
        } else {
            // Find the positive root
            float root = (-b + std::sqrt(discriminant)) / (2 * a);
            return root;
        }
    }


    static bool isPointLightInFrustum(LevelRenderer &r, const Light& l) {
        float threshold = 0.1f;
        float radius = calculateBoundingSphereRadius(1.0f, l.linear, l.quadratic, threshold);
        return r.frustum.sphereInFrustum(l.position, radius) != ViewFrustum::OUTSIDE;
    }

    static void assignRenderLights(LevelRenderer &r) {
        // sort lights by whether they are in the frustum or not, then by distance to camera
        std::sort(r.lights.begin(), r.lights.end(), [&r](const Light& a, const Light& b) {
            auto isAInFrustum = isPointLightInFrustum(r, a);
            auto isBInFrustum = isPointLightInFrustum(r, b);
            if(isAInFrustum == isBInFrustum) {
                return glm::length(a.position - r.camera->Position) < glm::length(b.position - r.camera->Position);
            }
            return isAInFrustum > isBInFrustum;
        });
        // sort lights according to distance to camera, closest first
        /*
        std::sort(r.lights.begin(), r.lights.end(), [&r](const Light& a, const Light& b) {
            return glm::length(a.position - r.camera->Position) < glm::length(b.position - r.camera->Position);
        });
         */
        // select the 8 closest lights
        r.renderLights.clear();
        for(i32 i = 0 ; i < r.lights.size(); i++) {
            RenderLight rl{};
            rl.lightIndex = i;
            rl.level = r.lights[i].level;
            rl.shadowEnabled = r.lights[i].shadowEnabled;
            //rl.shadowEnabled = false;
            rl.distance = glm::length(r.lights[i].position - r.camera->Position);
            r.renderLights.push_back(rl);
            if(i >= 7) {
                break;
            }
        }
        // sort render lights by shadow enabled, then level, then distance
        std::sort(r.renderLights.begin(), r.renderLights.end(), [](const RenderLight& a, const RenderLight& b) {
            if(a.shadowEnabled == b.shadowEnabled) {
                if(a.level == b.level) {
                    return a.distance < b.distance;
                }
                return a.level > b.level;
            }
            return a.shadowEnabled > b.shadowEnabled;
        });
        // disable shadows on all but the first 4 lights
        for(i32 i = 0 ; i < r.renderLights.size(); i++) {
            if(i >= 4) {
                r.renderLights[i].shadowEnabled = false;
            }
        }
    }

    static void calculateGeometryTangents(std::vector<MeshVertex>& vertices) {
        for(i32 i = 0 ; i < vertices.size(); i += 3) {
            MeshVertex& v0 = vertices[i];
            MeshVertex& v1 = vertices[i+1];
            MeshVertex& v2 = vertices[i+2];

            glm::vec3 Edge1 = v1.position - v0.position;
            glm::vec3 Edge2 = v2.position - v0.position;

            float DeltaU1 = v1.textureCoords.x - v0.textureCoords.x;
            float DeltaV1 = v1.textureCoords.y - v0.textureCoords.y;
            float DeltaU2 = v2.textureCoords.x - v0.textureCoords.x;
            float DeltaV2 = v2.textureCoords.y - v0.textureCoords.y;

            float f = 1.0f / (DeltaU1 * DeltaV2 - DeltaU2 * DeltaV1);

            glm::vec3 Tangent;
            Tangent.x = f * (DeltaV2 * Edge1.x - DeltaV1 * Edge2.x);
            Tangent.y = f * (DeltaV2 * Edge1.y - DeltaV1 * Edge2.y);
            Tangent.z = f * (DeltaV2 * Edge1.z - DeltaV1 * Edge2.z);

            Tangent = glm::normalize(Tangent);
            v0.tangent = Tangent;
            v1.tangent = Tangent;
            v2.tangent = Tangent;
        }
    }

    static bool isGeometryInFrustum(LevelRenderer &r, glm::vec3 position) {
        float halfSize = CUBE_SIZE / 2.0f;
        auto pos = Vec3(position.x - halfSize, position.y - halfSize, position.z - halfSize);
        //auto pos = Vec3(position.x, position.y, position.z);
        // box is defined by the corner and the size, pos is the center of the box
        AABox box(pos, CUBE_SIZE, CUBE_SIZE, CUBE_SIZE);
        //AABox box(pos, -halfSize, -halfSize, -halfSize);
        return r.frustum.boxInFrustum(box) != ViewFrustum::OUTSIDE;
        //return r.frustum.pointInFrustum(position);
    }

    void RenderDepthPrePass(LevelRenderer &r) {
        glViewport(0, 0, r.settings.renderWidth, r.settings.renderHeight);
        // create matrices
        glm::mat4 model         = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        glm::mat4 view          = glm::mat4(1.0f);
        glm::mat4 projection    = glm::mat4(1.0f);

        r.depthShader->use();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);

        r.depthBufferFbo->bindForWriting();
        glClear(GL_DEPTH_BUFFER_BIT);


        // pitch the camera down a bit
        r.camera->Pitch -= r.settings.headTilt;
        auto oldCamPos = r.camera->Position;
        // move the camera back a bit and up
        r.camera->Position -= r.camera->Front * 1.45f;
        r.camera->updateCameraVectors();

        view = r.camera->GetViewMatrix();
        glm::vec3 camPos = r.camera->Position;
        glm::vec3 camDir = (camPos + r.camera->Front);
        glm::vec3 camUp = r.camera->Up;
        r.camera->Position = oldCamPos;
        r.camera->Pitch += r.settings.headTilt;
        r.camera->updateCameraVectors();

        auto viewPort = GetViewport();
        // update the view frustum
        r.frustum.setCamInternals(r.settings.FOV, (float) r.settings.renderWidth / (float) r.settings.renderHeight, 0.5f, 100.0f);
        r.frustum.setCamDef(camPos, camDir, camUp);
        projection = glm::perspective(glm::radians(r.settings.FOV), (float) r.settings.renderWidth / (float) r.settings.renderHeight, 0.5f, 100.0f);

        r.depthShader->setMat4("model", model);
        r.depthShader->setMat4("view", view);
        r.depthShader->setMat4("projection", projection);

        // render batches
        for(auto &batch : r.batches) {
            switch(batch.type) {
                case BatchType::GEOMETRY: {
                    if(!isGeometryInFrustum(r, batch.position)) {
                        continue;
                    }
                    r.geometryVbo->bind();
                    glDrawArrays(GL_TRIANGLES, (i32) batch.offset, (i32) batch.count);
                    r.geometryVbo->unbind();
                    break;
                }
            }
        }
        for(auto &batch : r.batches) {
            switch(batch.type) {
                case BatchType::MODEL: {
                    renderModel(r, batch, true, *r.depthShader);
                    break;
                }
            }
        }
        // Restore color writes
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    }


    void RenderLevel(LevelRenderer &r, float delta) {
        r.gpuFrameTimer->start();
        r.cellsRendered = 0;
        glEnable(GL_CULL_FACE);
        glFrontFace(GL_CCW);
        glCullFace(GL_BACK);
        assignRenderLights(r);

        RenderDepthPrePass(r);

        if(r.settings.shadowsEnabled) {
            if (r.renderLights.size() > 0)
                RenderLevelShadows(r, r.renderLights[0], *r.shadowFbo1);
            if (r.renderLights.size() > 1)
                RenderLevelShadows(r, r.renderLights[1], *r.shadowFbo2);
            if (r.renderLights.size() > 2)
                RenderLevelShadows(r, r.renderLights[2], *r.shadowFbo3);
            if (r.renderLights.size() > 3)
                RenderLevelShadows(r, r.renderLights[3], *r.shadowFbo4);
        }
        glViewport(0, 0, r.settings.renderWidth, r.settings.renderHeight);
        BindFrameBuffer(r.fbo);
        //glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glEnable(GL_DEPTH_TEST);
        //glDisable(GL_CULL_FACE);
        //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClear(GL_COLOR_BUFFER_BIT);
        // create transformations
        glm::mat4 model         = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        glm::mat4 view          = glm::mat4(1.0f);
        glm::mat4 projection    = glm::mat4(1.0f);

        // pitch the camera down a bit
        r.camera->Pitch -= r.settings.headTilt;
        auto oldCamPos = r.camera->Position;
        // move the camera back a bit and up
        r.camera->Position -= r.camera->Front * 1.45f;
        r.camera->updateCameraVectors();

        view = r.camera->GetViewMatrix();
        glm::vec3 camPos = r.camera->Position;
        glm::vec3 camDir = (camPos + r.camera->Front);
        glm::vec3 camUp = r.camera->Up;
        r.camera->Position = oldCamPos;
        r.camera->Pitch += r.settings.headTilt;
        r.camera->updateCameraVectors();


        auto viewPort = GetViewport();
        // update the view frustum
        r.frustum.setCamInternals(r.settings.FOV, (float) r.settings.renderWidth / (float) r.settings.renderHeight, 0.5f, 100.0f);
        r.frustum.setCamDef(camPos, camDir, camUp);
        projection = glm::perspective(glm::radians(r.settings.FOV), (float) r.settings.renderWidth / (float) r.settings.renderHeight, 0.5f, 100.0f);

        // Setup geometry shader
        r.geometryShader->use();
        r.geometryShader->setUniform(ShaderUniforms.model, model);
        r.geometryShader->setUniform(ShaderUniforms.view, view);
        r.geometryShader->setUniform(ShaderUniforms.proj, projection);
        r.geometryShader->setVec3("CameraPos", r.camera->Position);
        r.geometryShader->setFloat("FogDensity", 0.15f);
        r.geometryShader->setVec3("FogColor", glm::vec3(0.00f, 0.00f, 0.00f));
        r.geometryShader->setInt("FogEnabled", 0);
        r.geometryShader->setInt("ShadowEnabled", r.settings.shadowsEnabled ? 1 : 0);
        r.geometryShader->setFloat("FarPlane", 100.0f);
        r.geometryShader->setInt("depthBufferTexture", 6);
        

        // Setup sprite shader
        r.spriteShader->use();
        r.spriteShader->setMat4("model", model);
        r.spriteShader->setMat4("view", view);
        r.spriteShader->setMat4("projection", projection);
        r.spriteShader->setVec3("CameraPos", r.camera->Position);
        r.spriteShader->setVec3("CameraRight", r.camera->Right);
        r.spriteShader->setVec3("CameraUp", r.camera->Up);
        r.spriteShader->setFloat("FogDensity", 0.15f);
        r.spriteShader->setVec3("FogColor", glm::vec3(0.0f, 0.0f, 0.0));
        r.spriteShader->setInt("FogEnabled", 0);
        r.spriteShader->setInt("ShadowEnabled", 1);
        r.spriteShader->setFloat("FarPlane", 100.0f);

        // Setup model shader
        r.modelShader->use();
        r.modelShader->setMat4("model", model);
        r.modelShader->setMat4("view", view);
        r.modelShader->setMat4("projection", projection);
        r.modelShader->setVec3("CameraPos", r.camera->Position);
        r.modelShader->setFloat("FogDensity", 0.15f);
        r.modelShader->setVec3("FogColor", glm::vec3(0.00f, 0.00f, 0.00f));
        r.modelShader->setInt("FogEnabled", 0);
        r.modelShader->setInt("ShadowEnabled", r.settings.shadowsEnabled ? 1 : 0);
        r.modelShader->setFloat("FarPlane", 100.0f);
        r.modelShader->setInt("depthBufferTexture", 6);

        // render batches
        for(auto &batch : r.batches) {
            switch(batch.type) {
                case BatchType::GEOMETRY: {
                    if(!isGeometryInFrustum(r, batch.position)) {
                        continue;
                    }
                    r.cellsRendered++;
                    glEnable(GL_DEPTH_TEST);
                    glDepthMask(GL_TRUE); // Enable depth buffer writing
                    r.geometryShader->use();
                    r.geometryShader->setVec3("material.ambient", glm::vec3(0.001f, 0.001f, 0.001f));
                    r.geometryShader->setInt("material.diffuse", 0);
                    r.geometryShader->setInt("material.normalMap", 1);
                    r.geometryShader->setInt("material.specularMap", 2);
                    r.geometryShader->setFloat("material.shininess", 20.0f);
                    r.geometryShader->setInt("material.hasNormalMap", r.settings.normalMapping ? 1 : 0);
                    r.geometryShader->setInt("material.hasSpecularMap", r.settings.specularMapping ? 1 : 0);
                    setShaderLights(r, *r.geometryShader);
                    // bind diffuse map
                    r.wallDiffuseMaps.bind(GL_TEXTURE0);
                    // bind normal map
                    r.wallNormalMaps.bind(GL_TEXTURE1);
                    // bind specular map
                    r.wallSpecularMaps.bind(GL_TEXTURE2);

                    // bind depth buffer
                    r.depthBufferFbo->bindForReading(GL_TEXTURE6);
                    // bind shadow cube map
                    r.shadowFbo1->bindForReading(GL_TEXTURE7);
                    r.shadowFbo2->bindForReading(GL_TEXTURE8);
                    r.shadowFbo3->bindForReading(GL_TEXTURE9);
                    r.shadowFbo4->bindForReading(GL_TEXTURE10);

                    r.geometryVbo->bind();
                    glDrawArrays(GL_TRIANGLES, (i32) batch.offset, (i32) batch.count);
                    r.geometryVbo->unbind();
                    UnbindTexture();
                    break;
                }
                case BatchType::SPRITE: {
                    //glDepthMask(GL_FALSE); // Disable depth buffer writing
                    glEnable(GL_DEPTH_TEST);
                    // enable blending
                    glEnable(GL_BLEND);
                    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                    r.spriteShader->use();
                    r.spriteShader->setVec3("SpritePos", batch.position);
                    r.spriteShader->setVec3("SpriteNormal", batch.spriteNormal);
                    r.spriteShader->setInt("billboarding", batch.billboarding);
                    r.spriteShader->setVec3("material.ambient", glm::vec3(0.001f, 0.001f, 0.001f));
                    r.spriteShader->setInt("material.diffuse", 0);
                    r.spriteShader->setFloat("material.shininess", 8.0f);
                    r.spriteShader->setInt("material.hasNormalMap", 0);
                    r.spriteShader->setInt("material.hasSpecularMap", 0);
                    setShaderLights(r, *r.spriteShader);
                    //SDL_Log("Rendering sprite batch");
                    glActiveTexture(GL_TEXTURE0);
                    BindTexture(r.spriteTextureAtlas.textureId);
                    // bind shadow cube map
                    r.shadowFbo1->bindForReading(GL_TEXTURE7);
                    r.shadowFbo2->bindForReading(GL_TEXTURE8);
                    r.shadowFbo3->bindForReading(GL_TEXTURE9);
                    r.shadowFbo4->bindForReading(GL_TEXTURE10);

                    r.spriteVbo->bind();
                    glDrawArrays(GL_TRIANGLES, (i32) batch.offset, (i32) batch.count);
                    r.spriteVbo->unbind();
                    UnbindTexture();
                    glDisable(GL_BLEND);
                    break;
                }
                case BatchType::MODEL: {
                    glEnable(GL_DEPTH_TEST);
                    glDepthMask(GL_TRUE); // Enable depth buffer writing
                    r.modelShader->use();
                    r.modelShader->setVec3("material.ambient", glm::vec3(0.001f, 0.001f, 0.001f));
                    r.modelShader->setFloat("material.shininess", 64.0f);
                    r.modelShader->setInt("material.diffuse", 0);
                    r.modelShader->setInt("material.normalMap", 1);
                    r.modelShader->setInt("material.specularMap", 2);
                    setShaderLights(r, *r.modelShader);
                    renderModel(r, batch, false, *r.modelShader);
                    break;
                }
            }
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        // render to screen
        SetViewport();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        r.screenShader->use();
        r.screenShader->setInt("screenTexture", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, r.fboTexture);
        r.screenVbo->bind();
        glDrawArrays(GL_TRIANGLES, 0, 6);
        r.screenVbo->unbind();

        //SDL_Log("Rendered %d geometry batches", geometryCount);
        //SDL_Log("Rendered %d batches", (i32) r.batches.size());
        r.gpuFrameTimer->stop();
    }

    void UploadLevelMesh(LevelRenderer &r) {
        calculateGeometryTangents(r.geometryMesh);
        r.geometryVbo->allocate(r.geometryMesh.data(), r.geometryMesh.size() * 12 * sizeof(float), VertexAccessType::DYNAMIC);
        r.spriteVbo->allocate(r.spriteMesh.data(), r.spriteMesh.size() * 5 * sizeof(float), VertexAccessType::DYNAMIC);
    }

    void UpdateLevelRenderer(LevelRenderer &r, float delta) {
        r.camera->Update(delta);
    }

    u32 LoadModel(LevelRenderer &r, const std::string &filename, const std::string &textureFile) {
        Model model;
        LoadModel(model, filename, false);
        LoadDiffuseMap(model, textureFile);
        r.models.push_back(model);
        return (u32) r.models.size() - 1;
    }

    u32 LoadModel(LevelRenderer &r, const std::string &filename, const std::string &diffuseFile, const std::string &normalFile) {
        Model model;
        LoadModel(model, filename, true);
        LoadDiffuseMap(model, diffuseFile);
        LoadNormalMap(model, normalFile);
        r.models.push_back(model);
        return (u32) r.models.size() - 1;
    }

    u32 LoadModel(LevelRenderer &r, const std::string &filename, const std::string &diffuseFile, const std::string &normalFile, const std::string &specularFile) {
        Model model;
        LoadModel(model, filename, true);
        LoadDiffuseMap(model, diffuseFile);
        LoadNormalMap(model, normalFile);
        LoadSpecularMap(model, specularFile);
        r.models.push_back(model);
        return (u32) r.models.size() - 1;
    }
}
