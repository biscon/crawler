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

    struct {
        const std::string model = "model";
        const std::string view = "view";
        const std::string proj = "projection";
    } ShaderUniforms;

    void InitLevelRenderer(LevelRenderer &r) {
        r.camera = std::make_unique<Camera>(glm::vec3(0.0f, 0.0f, 0.0f));
        r.geometryShader = std::make_unique<ShaderProgram>("shaders/geometry_vertex.glsl",
                                                           "shaders/geometry_fragment.glsl");
        r.geometryShader->setupUniform(ShaderUniforms.model);
        r.geometryShader->setupUniform(ShaderUniforms.view);
        r.geometryShader->setupUniform(ShaderUniforms.proj);
        
        r.spriteShader = std::make_unique<ShaderProgram>("shaders/sprite_vertex.glsl",
                                                         "shaders/sprite_fragment.glsl");

        r.modelShader = std::make_unique<ShaderProgram>("shaders/model_vertex.glsl",
                                                         "shaders/model_fragment.glsl");

        // fbo for later use
        auto vp = GetViewport();
        auto width = (i32) vp.screenWidth;
        auto height = (i32) vp.screenHeight;
        r.fboTexture = CreateTexture();
        SetFilteringTexture(r.fboTexture, TextureFiltering::LINEAR);
        UploadTexture(r.fboTexture, width, height, nullptr, TextureFormatInternal::RGBA8, TextureFormatData::RGBA);
        r.fbo = CreateFrameBuffer(r.fboTexture, TextureFormatInternal::RGBA8, TextureFormatData::RGBA, width, height);

        // geometry texture atlas
        auto builder = Renderer::TextureAtlasBuilder(2048, 2048, Renderer::PixelFormat::RGBA);
        r.wallTexture = builder.addFromPng("assets/eye_wall.png", true);
        r.wallEndTexture = builder.addFromPng("assets/eye_wall_end.png", true);
        r.ceilingTexture = builder.addFromPng("assets/stone_floor.png", true);
        r.floorTexture = builder.addFromPng("assets/stone_floor2.png", true);
        r.drainTexture = builder.addFromPng("assets/eye_wall_drain.png", true);
        builder.build(r.geometryTextureAtlas);

        // geometry vbo
        VertexAttributes geometryAttrs;
        geometryAttrs.add(0, 3, VertexAttributeType::Float); // position
        geometryAttrs.add(1, 2, VertexAttributeType::Float); // tex coords
        geometryAttrs.add(2, 3, VertexAttributeType::Float); // colors
        r.geometryVbo = std::make_unique<VertexBuffer>(geometryAttrs);
        
        // sprite vbo
        VertexAttributes spriteAttrs;
        spriteAttrs.add(0, 3, VertexAttributeType::Float); // position
        spriteAttrs.add(1, 2, VertexAttributeType::Float); // tex coords
        spriteAttrs.add(2, 3, VertexAttributeType::Float); // colors
        r.spriteVbo = std::make_unique<VertexBuffer>(spriteAttrs);
        
        //r.geometryVbo->allocate(nullptr, 0, VertexAccessType::STATIC);
        /*
        r.wallTexture = CreateTexture();
        SetFilteringTexture(r.wallTexture, TextureFiltering::NEAREST);
        LoadTextureFromPng(r.wallTexture, "assets/stone_wall.png", false);
         */
        //GenerateTextureMipmaps(r.geometryTextureAtlas.textureId);
        r.doorModelIndex = LoadModel(r, "assets/models/door_frame.obj", "assets/models/door_frame.png");
    }

    void ShutdownLevelRenderer(LevelRenderer &renderer) {
        DestroyTextureAtlas(renderer.geometryTextureAtlas);
        DestroyTextureAtlas(renderer.spriteTextureAtlas);
        DestroyFrameBuffer(renderer.fbo);
    }

    static void renderModel(LevelRenderer &r, RenderBatch& batch) {
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
        r.modelShader->setMat4("model", model);
        glActiveTexture(GL_TEXTURE0);
        BindTexture(m.textureId);
        m.vbo->bind();
        if(batch.modelObjName == "*") {
            glDrawArrays(GL_TRIANGLES, 0, (i32) m.vertices.size());
        }
        else {
            const auto& obj = m.objects[batch.modelObjName];
            glDrawArrays(GL_TRIANGLES, (i32) obj.offset, (i32) obj.count);
        }
        m.vbo->unbind();
        UnbindTexture();
    }

    void RenderLevel(LevelRenderer &r, float delta) {
        static float accDelta = 0.0f;
        accDelta += delta/4.0f;

        //BindFrameBuffer(r.fbo);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glEnable(GL_DEPTH_TEST);
        //glDisable(GL_CULL_FACE);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // create transformations
        glm::mat4 model         = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        glm::mat4 view          = glm::mat4(1.0f);
        glm::mat4 projection    = glm::mat4(1.0f);

        // little trick with the model matrix
        //model = glm::translate(glm::mat4(1.0f), r.camera->Front * -2.5f);
        //model = glm::rotate(model, accDelta, glm::vec3(0.0f, 1.0f, 0.0f));

        // pitch the camera down a bit
        float pitch = 4.0f;
        r.camera->Pitch -= pitch;
        r.camera->updateCameraVectors();
        view = r.camera->GetViewMatrix();
        r.camera->Pitch += pitch;
        r.camera->updateCameraVectors();
        // move the camera back a bit and up
        view = glm::translate(view, r.camera->Front * 1.25f);
        view = glm::translate(view, -r.camera->Up * 0.25f);

        auto viewPort = GetViewport();
        projection = glm::perspective(glm::radians(65.0f), (float) viewPort.screenWidth / (float) viewPort.screenHeight, 0.5f, 100.0f);
        //projection = glm::perspective(glm::radians(75.0f), (float) 1920 / (float) 1080, 0.1f, 100.0f);

        // Setup geometry shader
        r.geometryShader->use();
        r.geometryShader->setUniform(ShaderUniforms.model, model);
        r.geometryShader->setUniform(ShaderUniforms.view, view);
        r.geometryShader->setUniform(ShaderUniforms.proj, projection);
        r.geometryShader->setVec3("CameraPos", r.camera->Position);
        r.geometryShader->setFloat("FogDensity", 0.10f);
        r.geometryShader->setVec3("FogColor", glm::vec3(0.005f, 0.005f, 0.005f));
        r.geometryShader->setInt("FogEnabled", 0);
        r.geometryShader->setInt("texture1", 0);

        // Setup sprite shader
        r.spriteShader->use();
        r.spriteShader->setMat4("model", model);
        r.spriteShader->setMat4("view", view);
        r.spriteShader->setMat4("projection", projection);
        r.spriteShader->setVec3("CameraPos", r.camera->Position);
        r.spriteShader->setVec3("CameraRight", r.camera->Right);
        r.spriteShader->setVec3("CameraUp", r.camera->Up);
        r.spriteShader->setFloat("FogDensity", 0.10f);
        r.spriteShader->setVec3("FogColor", glm::vec3(0.005f, 0.005f, 0.005f));
        r.spriteShader->setInt("FogEnabled", 0);
        r.spriteShader->setInt("texture1", 0);

        // Setup model shader
        r.modelShader->use();
        r.modelShader->setMat4("model", model);
        r.modelShader->setMat4("view", view);
        r.modelShader->setMat4("projection", projection);
        r.modelShader->setVec3("CameraPos", r.camera->Position);
        r.modelShader->setFloat("FogDensity", 0.10f);
        r.modelShader->setVec3("FogColor", glm::vec3(0.005f, 0.005f, 0.005f));
        r.modelShader->setInt("FogEnabled", 0);
        r.modelShader->setInt("texture1", 0);

        // render batches
        for(auto &batch : r.batches) {
            switch(batch.type) {
                case BatchType::GEOMETRY: {
                    glEnable(GL_DEPTH_TEST);
                    glDepthMask(GL_TRUE); // Enable depth buffer writing
                    r.geometryShader->use();
                    
                    // bind diffuse map
                    glActiveTexture(GL_TEXTURE0);
                    BindTexture(r.geometryTextureAtlas.textureId);
                    r.geometryVbo->bind();
                    glDrawArrays(GL_TRIANGLES, (i32) batch.offset, (i32) batch.count);
                    r.geometryVbo->unbind();
                    UnbindTexture();
                    break;
                }
                case BatchType::SPRITE: {
                    //glDepthMask(GL_FALSE); // Disable depth buffer writing
                    glDisable(GL_DEPTH_TEST);
                    // enable blending
                    glEnable(GL_BLEND);
                    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                    r.spriteShader->use();
                    r.spriteShader->setVec3("SpritePos", batch.position);
                    r.spriteShader->setInt("billboarding", batch.billboarding);
                    //SDL_Log("Rendering sprite batch");
                    glActiveTexture(GL_TEXTURE0);
                    BindTexture(r.spriteTextureAtlas.textureId);
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
                    r.modelShader->setVec3("LightColor", batch.lightColor);
                    i32 lightIndex = 0;
                    for(auto &light : r.lights) {
                        std::string lightIndexStr = std::to_string(lightIndex);
                        std::string lightPosStr = "Lights[" + lightIndexStr + "].position";
                        r.modelShader->setVec3(lightPosStr, light.position);
                        std::string lightAmbientStr = "Lights[" + lightIndexStr + "].ambient";
                        r.modelShader->setVec3(lightAmbientStr, glm::vec3(0.025f, 0.025f, 0.025f));
                        std::string lightDiffuseStr = "Lights[" + lightIndexStr + "].diffuse";
                        r.modelShader->setVec3(lightDiffuseStr, light.diffuse);

                        // 1.0	0.22	0.20
                        std::string constantStr = "Lights[" + lightIndexStr + "].constant";
                        r.modelShader->setFloat(constantStr, 1.0f);
                        std::string linearStr = "Lights[" + lightIndexStr + "].linear";
                        r.modelShader->setFloat(linearStr, 0.22f);
                        std::string quadraticStr = "Lights[" + lightIndexStr + "].quadratic";
                        r.modelShader->setFloat(quadraticStr, 0.20f);

                        std::string lightEnabledStr = "Lights[" + lightIndexStr + "].enabled";
                        r.modelShader->setInt(lightEnabledStr, 1);
                        lightIndex++;
                        if(lightIndex >= 7) {
                            break;
                        }
                    }
                    //r.modelShader->setVec3("Lights[0].position", glm::vec3(4.0f * CUBE_SIZE - 1.5f, 1.5f, 6.0f * CUBE_SIZE));
                    renderModel(r, batch);
                    break;
                }
            }
        }
        //SDL_Log("Rendered %d batches", (i32) r.batches.size());

        /*
        glActiveTexture(GL_TEXTURE0);
        BindTexture(r.geometryTextureAtlas.textureId);
        r.geometryVbo->bind();
        glDrawArrays(GL_TRIANGLES, 0, (i32) r.geometryMesh.size());
        r.geometryVbo->unbind();
        UnbindTexture();
        */
    }

    void UploadLevelMesh(LevelRenderer &r) {
        r.geometryVbo->allocate(r.geometryMesh.data(), r.geometryMesh.size() * 8 * sizeof(float), VertexAccessType::STATIC);
        r.spriteVbo->allocate(r.spriteMesh.data(), r.spriteMesh.size() * 8 * sizeof(float), VertexAccessType::STATIC);
    }

    void UpdateLevelRenderer(LevelRenderer &r, float delta) {
        r.camera->Update(delta);
    }

    u32 LoadModel(LevelRenderer &r, const std::string &filename, const std::string &textureFile) {
        Model model;
        LoadModel(model, filename, textureFile);
        r.models.push_back(model);
        return (u32) r.models.size() - 1;
    }
}

// normals
/*
 * static void meshCell(float x, float y, float z, CubeFaces& faces, std::vector<MeshVertex>& vertices, TextureAtlas& atlas) {
        // Your code to handle the side generation goes here
        //printf("Left: %d, Right: %d, Front: %d, Back: %d, Above: %d, Below: %d\n", faces.left, faces.right, faces.front, faces.back, faces.top, faces.bottom);
        // Calculate half size for centering
        float halfSize = cubeSize / 2.0f;

        // Front face
        if (faces.front) {
            auto uvRect = atlas.uvRects[faces.front];
            vertices.emplace_back(MeshVertex{{-halfSize + x, -halfSize + y, -halfSize + z}, {uvRect.left, uvRect.bottom}, {0.0f, 0.0f, 1.0f}});
            vertices.emplace_back(MeshVertex{{halfSize + x, -halfSize + y, -halfSize + z}, {uvRect.right, uvRect.bottom}, {0.0f, 0.0f, 1.0f}});
            vertices.emplace_back(MeshVertex{{halfSize + x, halfSize + y, -halfSize + z}, {uvRect.right, uvRect.top}, {0.0f, 0.0f, 1.0f}});
            vertices.emplace_back(MeshVertex{{halfSize + x, halfSize + y, -halfSize + z}, {uvRect.right, uvRect.top}, {0.0f, 0.0f, 1.0f}});
            vertices.emplace_back(MeshVertex{{-halfSize + x, halfSize + y, -halfSize + z}, {uvRect.left, uvRect.top}, {0.0f, 0.0f, 1.0f}});
            vertices.emplace_back(MeshVertex{{-halfSize + x, -halfSize + y, -halfSize + z}, {uvRect.left, uvRect.bottom}, {0.0f, 0.0f, 1.0f}});
        }

        // Back face
        if (faces.back) {
            auto uvRect = atlas.uvRects[faces.back];
            vertices.emplace_back(MeshVertex{{-halfSize + x, -halfSize + y, halfSize + z}, {uvRect.left, uvRect.bottom}, {0.0f, 0.0f, -1.0f}});
            vertices.emplace_back(MeshVertex{{halfSize + x, -halfSize + y, halfSize + z}, {uvRect.right, uvRect.bottom}, {0.0f, 0.0f, -1.0f}});
            vertices.emplace_back(MeshVertex{{halfSize + x, halfSize + y, halfSize + z}, {uvRect.right, uvRect.top}, {0.0f, 0.0f, -1.0f}});
            vertices.emplace_back(MeshVertex{{halfSize + x, halfSize + y, halfSize + z}, {uvRect.right, uvRect.top}, {0.0f, 0.0f, -1.0f}});
            vertices.emplace_back(MeshVertex{{-halfSize + x, halfSize + y, halfSize + z}, {uvRect.left, uvRect.top}, {0.0f, 0.0f, -1.0f}});
            vertices.emplace_back(MeshVertex{{-halfSize + x, -halfSize + y, halfSize + z}, {uvRect.left, uvRect.bottom}, {0.0f, 0.0f, -1.0f}});
        }

        // Left face
        if (faces.left) {
            auto uvRect = atlas.uvRects[faces.left];
            vertices.emplace_back(MeshVertex{{-halfSize + x, halfSize + y, halfSize + z}, {uvRect.left, uvRect.top}, {1.0f, 0.0f, 0.0f}});
            vertices.emplace_back(MeshVertex{{-halfSize + x, halfSize + y, -halfSize + z}, {uvRect.right, uvRect.top}, {1.0f, 0.0f, 0.0f}});
            vertices.emplace_back(MeshVertex{{-halfSize + x, -halfSize + y, -halfSize + z}, {uvRect.right, uvRect.bottom}, {1.0f, 0.0f, 0.0f}});
            vertices.emplace_back(MeshVertex{{-halfSize + x, -halfSize + y, -halfSize + z}, {uvRect.right, uvRect.bottom}, {1.0f, 0.0f, 0.0f}});
            vertices.emplace_back(MeshVertex{{-halfSize + x, -halfSize + y, halfSize + z}, {uvRect.left, uvRect.bottom}, {1.0f, 0.0f, 0.0f}});
            vertices.emplace_back(MeshVertex{{-halfSize + x, halfSize + y, halfSize + z}, {uvRect.left, uvRect.top}, {1.0f, 0.0f, 0.0f}});
        }

        // Right face
        if (faces.right) {
            auto uvRect = atlas.uvRects[faces.right];
            vertices.emplace_back(MeshVertex{{halfSize + x, halfSize + y, halfSize + z}, {uvRect.left, uvRect.top}, {-1.0f, 0.0f, 0.0f}});
            vertices.emplace_back(MeshVertex{{halfSize + x, halfSize + y, -halfSize + z}, {uvRect.right, uvRect.top}, {-1.0f, 0.0f, 0.0f}});
            vertices.emplace_back(MeshVertex{{halfSize + x, -halfSize + y, -halfSize + z}, {uvRect.right, uvRect.bottom}, {-1.0f, 0.0f, 0.0f}});
            vertices.emplace_back(MeshVertex{{halfSize + x, -halfSize + y, -halfSize + z}, {uvRect.right, uvRect.bottom}, {-1.0f, 0.0f, 0.0f}});
            vertices.emplace_back(MeshVertex{{halfSize + x, -halfSize + y, halfSize + z}, {uvRect.left, uvRect.bottom}, {-1.0f, 0.0f, 0.0f}});
            vertices.emplace_back(MeshVertex{{halfSize + x, halfSize + y, halfSize + z}, {uvRect.left, uvRect.top}, {-1.0f, 0.0f, 0.0f}});
        }

        // Bottom face
        if (faces.bottom) {
            auto uvRect = atlas.uvRects[faces.bottom];
            vertices.emplace_back(MeshVertex{{-halfSize + x, -halfSize + y, -halfSize + z}, {uvRect.left, uvRect.bottom}, {0.0f, 1.0f, 0.0f}});
            vertices.emplace_back(MeshVertex{{halfSize + x, -halfSize + y, -halfSize + z}, {uvRect.right, uvRect.bottom}, {0.0f, 1.0f, 0.0f}});
            vertices.emplace_back(MeshVertex{{halfSize + x, -halfSize + y, halfSize + z}, {uvRect.right, uvRect.top}, {0.0f, 1.0f, 0.0f}});
            vertices.emplace_back(MeshVertex{{halfSize + x, -halfSize + y, halfSize + z}, {uvRect.right, uvRect.top}, {0.0f, 1.0f, 0.0f}});
            vertices.emplace_back(MeshVertex{{-halfSize + x, -halfSize + y, halfSize + z}, {uvRect.left, uvRect.top}, {0.0f, 1.0f, 0.0f}});
            vertices.emplace_back(MeshVertex{{-halfSize + x, -halfSize + y, -halfSize + z}, {uvRect.left, uvRect.bottom}, {0.0f, 1.0f, 0.0f}});
        }

        // Top face
        if (faces.top) {
            auto uvRect = atlas.uvRects[faces.top];
            vertices.emplace_back(MeshVertex{{-halfSize + x, halfSize + y, -halfSize + z}, {uvRect.left, uvRect.bottom}, {0.0f, -1.0f, 0.0f}});
            vertices.emplace_back(MeshVertex{{halfSize + x, halfSize + y, -halfSize + z}, {uvRect.right, uvRect.bottom}, {0.0f, -1.0f, 0.0f}});
            vertices.emplace_back(MeshVertex{{halfSize + x, halfSize + y, halfSize + z}, {uvRect.right, uvRect.top}, {0.0f, -1.0f, 0.0f}});
            vertices.emplace_back(MeshVertex{{halfSize + x, halfSize + y, halfSize + z}, {uvRect.right, uvRect.top}, {0.0f, -1.0f, 0.0f}});
            vertices.emplace_back(MeshVertex{{-halfSize + x, halfSize + y, halfSize + z}, {uvRect.left, uvRect.top}, {0.0f, -1.0f, 0.0f}});
            vertices.emplace_back(MeshVertex{{-halfSize + x, halfSize + y, -halfSize + z}, {uvRect.left, uvRect.bottom}, {0.0f, -1.0f, 0.0f}});
        }
    }

 */