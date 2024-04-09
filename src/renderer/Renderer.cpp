//
// Created by bison on 04-10-23.
//

#include <SDL_log.h>
#include "Renderer.h"
#include "Viewport.h"
#include "glm/ext.hpp"

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
        const std::string SCREEN_TEXTURE = "screenTexture";
        const std::string LIGHT_TEXTURE = "lightTexture";
        const std::string USE_LIGHT = "useLight";
        const std::string USE_CRT = "useCrt";
    } ScreenShaderUniforms;

    struct {
        const std::string MODEL = "model";
        const std::string VIEW = "view";
        const std::string PROJ = "proj";
        const std::string USE_TEXTURE = "use_texture";
        const std::string IS_FONT = "is_font";
        const std::string USE_LUT = "useLut";
        const std::string TEXTURE = "tex";
        const std::string LUT_TEXTURE_1 = "lutTexture";
        const std::string LUT_TEXTURE_2 = "lutTexture2";
        const std::string LUT_MIX = "lutMix";
        const std::string MATTE = "matte";
        const std::string MATTE_COLOR = "matteColor";
        const std::string ENGINE_TIME = "engineTime";
        const std::string USE_WIND = "useWind";
        const std::string WIND_SPEED = "windSpeed";
        const std::string WIND_MIN_STRENGTH = "windMinStrength";
        const std::string WIND_MAX_STRENGTH = "windMaxStrength";
        const std::string WIND_STRENGTH_SCALE = "windStrengthScale";
        const std::string WIND_INTERVAL = "windInterval";
        const std::string WIND_DETAIL = "windDetail";
        const std::string WIND_DISTORTION = "windDistortion";
        const std::string WIND_HEIGHT_OFFSET = "windHeightOffset";
    } PrimitiveShaderUniforms;

    static void loadShaderPrograms(Renderer &r) {
        r.screenShader = std::make_unique<ShaderProgram>("shaders/screen_vertex.glsl",
                                                       "shaders/screen_crt_fragment.glsl");
        r.screenShader->setupUniform(ScreenShaderUniforms.SCREEN_TEXTURE);
        r.screenShader->setupUniform(ScreenShaderUniforms.LIGHT_TEXTURE);
        r.screenShader->setupUniform(ScreenShaderUniforms.USE_LIGHT);
        r.screenShader->setupUniform(ScreenShaderUniforms.USE_CRT);

        r.primitiveShader = std::make_unique<ShaderProgram>("shaders/plain_vertex.glsl",
                                                          "shaders/plain_fragment.glsl");
        r.primitiveShader->setupUniform(PrimitiveShaderUniforms.MODEL);
        r.primitiveShader->setupUniform(PrimitiveShaderUniforms.VIEW);
        r.primitiveShader->setupUniform(PrimitiveShaderUniforms.PROJ);
        r.primitiveShader->setupUniform(PrimitiveShaderUniforms.USE_TEXTURE);
        r.primitiveShader->setupUniform(PrimitiveShaderUniforms.IS_FONT);
        r.primitiveShader->setupUniform(PrimitiveShaderUniforms.TEXTURE);
        r.primitiveShader->setupUniform(PrimitiveShaderUniforms.USE_LUT);
        r.primitiveShader->setupUniform(PrimitiveShaderUniforms.LUT_TEXTURE_1);
        r.primitiveShader->setupUniform(PrimitiveShaderUniforms.LUT_TEXTURE_2);
        r.primitiveShader->setupUniform(PrimitiveShaderUniforms.LUT_MIX);
        r.primitiveShader->setupUniform(PrimitiveShaderUniforms.MATTE);
        r.primitiveShader->setupUniform(PrimitiveShaderUniforms.MATTE_COLOR);
        r.primitiveShader->setupUniform(PrimitiveShaderUniforms.ENGINE_TIME);
        r.primitiveShader->setupUniform(PrimitiveShaderUniforms.USE_WIND);
        r.primitiveShader->setupUniform(PrimitiveShaderUniforms.WIND_SPEED);
        r.primitiveShader->setupUniform(PrimitiveShaderUniforms.WIND_MIN_STRENGTH);
        r.primitiveShader->setupUniform(PrimitiveShaderUniforms.WIND_MAX_STRENGTH);
        r.primitiveShader->setupUniform(PrimitiveShaderUniforms.WIND_STRENGTH_SCALE);
        r.primitiveShader->setupUniform(PrimitiveShaderUniforms.WIND_INTERVAL);
        r.primitiveShader->setupUniform(PrimitiveShaderUniforms.WIND_DETAIL);
        r.primitiveShader->setupUniform(PrimitiveShaderUniforms.WIND_DISTORTION);
        r.primitiveShader->setupUniform(PrimitiveShaderUniforms.WIND_HEIGHT_OFFSET);
    }

    static void setupFrameBuffers(Renderer &r) {
        auto vp = GetViewport();
        auto width = (i32) vp.screenWidth;
        auto height = (i32) vp.screenHeight;

        // setup framebuffer for light rendering
        r.lightFBOTexture = CreateTexture();
        SetFilteringTexture(r.lightFBOTexture, TextureFiltering::LINEAR);
        UploadTexture(r.lightFBOTexture, width, height, nullptr, TextureFormatInternal::RGB8, TextureFormatData::RGB);
        r.lightFBO = CreateFrameBuffer(r.lightFBOTexture, TextureFormatInternal::RGB8, TextureFormatData::RGB, width, height);

        // setup framebuffer for the background, no lighting
        r.bgFBOTexture = CreateTexture();
        SetFilteringTexture(r.bgFBOTexture, TextureFiltering::LINEAR);
        UploadTexture(r.bgFBOTexture, width, height, nullptr, TextureFormatInternal::RGB8, TextureFormatData::RGB);
        r.bgFBO = CreateFrameBuffer(r.bgFBOTexture, TextureFormatInternal::RGB8, TextureFormatData::RGB, width, height);

        // setup framebuffer for stuff not affected by light
        r.unlitFBOTexture = CreateTexture();
        SetFilteringTexture(r.unlitFBOTexture, TextureFiltering::LINEAR);
        UploadTexture(r.unlitFBOTexture, width, height, nullptr, TextureFormatInternal::RGBA8, TextureFormatData::RGBA);
        r.unlitFBO = CreateFrameBuffer(r.unlitFBOTexture, TextureFormatInternal::RGBA8, TextureFormatData::RGBA, width, height);

        // Setup framebuffer for rendering of stuff affected by light
        r.litFBOTexture = CreateTexture();
        SetFilteringTexture(r.litFBOTexture, TextureFiltering::LINEAR);
        UploadTexture(r.litFBOTexture, width, height, nullptr, TextureFormatInternal::RGBA8, TextureFormatData::RGBA);
        r.litFBO = CreateFrameBuffer(r.litFBOTexture, TextureFormatInternal::RGBA8, TextureFormatData::RGBA, width, height);
    }

    static void setupVertexBuffers(Renderer &renderer) {
        // setup offscreen rendering VertexBuffer
        VertexAttributes attrs;
        attrs.add(0, 2, VertexAttributeType::Float); // position
        attrs.add(1, 2, VertexAttributeType::Float); // tex coords
        renderer.screenVBO = std::make_unique<VertexBuffer>(attrs);
        renderer.screenVBO->allocate((void *) &screenQuadVertices, sizeof(screenQuadVertices),
                                     VertexAccessType::STATIC);

        // setup primitive rendering VertexBuffer
        VertexAttributes attrs2;
        attrs2.add(0, 2, VertexAttributeType::Float); // position
        attrs2.add(1, 4, VertexAttributeType::Float); // color
        attrs2.add(2, 2, VertexAttributeType::Float); // tex coords
        renderer.primitiveVBO = std::make_unique<VertexBuffer>(attrs2);
    }

    
    void InitRenderer(Renderer &r) {
        loadShaderPrograms(r);
        setupFrameBuffers(r);
        setupVertexBuffers(r);
        CreateRenderBuffer(r.renderBuffer, KILOBYTES(128));
    }

    inline static void enableRegularAlpha() {
        // straight alpha
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    inline static void enablePremultipliedAlpha() {
        // premultiplied alpha
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    }


    static void renderToTexture(Renderer &r) {
        auto vp = GetViewport();
        glViewport(0, 0, (i32) vp.screenWidth, (i32) vp.screenHeight);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        BindFrameBuffer(r.unlitFBO);
        r.primitiveShader->use();

        glClear(GL_COLOR_BUFFER_BIT);

        CopyToVertexBuffer(r.renderBuffer, *r.primitiveVBO);

        // setup matrices, right now we only really need the projection matrix
        auto model = glm::identity<glm::mat4>();
        auto view = glm::identity<glm::mat4>();
        glm::mat4 proj = glm::ortho(0.0f, (float) vp.screenWidth, (float) vp.screenHeight, 0.0f, -100.0f, 100.0f);

        // set uniforms
        r.primitiveShader->setUniform(PrimitiveShaderUniforms.MODEL, model);
        r.primitiveShader->setUniform(PrimitiveShaderUniforms.VIEW, view);
        r.primitiveShader->setUniform(PrimitiveShaderUniforms.PROJ, proj);

        // bind diffuse texture to unit 0
        r.primitiveShader->setUniform(PrimitiveShaderUniforms.TEXTURE, 0);
        r.primitiveShader->setUniform(PrimitiveShaderUniforms.USE_LUT, 0);

        // disable depth testing since we're using blending and painters algorithm
        glDisable(GL_DEPTH_TEST);

        // enable alpha blending
        glEnable(GL_BLEND);
        // regular ass alpha blending
        enableRegularAlpha();
        glEnable(GL_LINE_SMOOTH);
        glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

        Render(r, r.renderBuffer);
    }

    static void renderToScreen(Renderer &r) {
        auto vp = GetViewport();
        // set actual viewport based on current window size
        glViewport(vp.current.x, vp.current.y, vp.current.w, vp.current.h);

        // second pass, render offscreen texture as a quad filling up the entire screen
        glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        r.screenShader->use();

        // bind lit texture unit 0
        r.screenShader->setUniform(ScreenShaderUniforms.SCREEN_TEXTURE, 0);
        // bind light texture to unit 1
        r.screenShader->setUniform(ScreenShaderUniforms.LIGHT_TEXTURE, 1);
        r.screenShader->setUniform(ScreenShaderUniforms.USE_CRT, 0);

        r.screenVBO->bind();
        glDisable(GL_DEPTH_TEST);

        // render background
        glEnable(GL_BLEND);
        enableRegularAlpha();
        r.screenShader->setUniform(ScreenShaderUniforms.USE_LIGHT, 0);

        glActiveTexture(GL_TEXTURE0);
        BindTexture(r.bgFBOTexture);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Render lit geometry, blending is done in the geometryShader
        //glDisable(GL_BLEND);
        enablePremultipliedAlpha();
        r.screenShader->setUniform(ScreenShaderUniforms.USE_LIGHT, 1);
        glActiveTexture(GL_TEXTURE1);
        BindTexture(r.lightFBOTexture);
        glActiveTexture(GL_TEXTURE0);
        BindTexture(r.litFBOTexture);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // render unlit geometry in front
        glEnable(GL_BLEND);
        enablePremultipliedAlpha();
        r.screenShader->setUniform(ScreenShaderUniforms.USE_LIGHT, 0);
        glActiveTexture(GL_TEXTURE0);
        BindTexture(r.unlitFBOTexture);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    void UpdateRenderer(Renderer &r, float frameDelta) {
        renderToTexture(r);
        renderToScreen(r);
    }

    void ShutdownRenderer(Renderer &renderer) {
        DestroyRenderBuffer(renderer.renderBuffer);
        DestroyFrameBuffer(renderer.lightFBO);
        DestroyFrameBuffer(renderer.bgFBO);
        DestroyFrameBuffer(renderer.unlitFBO);
        DestroyFrameBuffer(renderer.litFBO);
        DestroyTexture(renderer.lightFBOTexture);
        DestroyTexture(renderer.bgFBOTexture);
        DestroyTexture(renderer.unlitFBOTexture);
        DestroyTexture(renderer.litFBOTexture);
    }

    static inline void drawPrimitives(Primitive primitive, i32 offset, i32 count) {
        switch (primitive) {
            case Primitive::TRIANGLES:
                glDrawArrays(GL_TRIANGLES, offset, count);
                break;
            case Primitive::LINES:
                glDrawArrays(GL_LINES, offset, count);
                break;
            case Primitive::TRIANGLE_STRIP:
                glDrawArrays(GL_TRIANGLE_STRIP, offset, count);
                break;
        }
    }

    void Render(Renderer& r, RenderBuffer &buffer) {
        if(buffer.vertices.empty())
            return;
        //SDL_Log("Renderer: %zu vertices, %zu commands", buffer.vertices.size(), buffer.cmdCount);

        u8 *cur_ptr = buffer.commands;
        while(cur_ptr < buffer.cmdOffset) {
            auto* command = (Command*) cur_ptr;
            switch (command->type) {
                case CommandType::TEXT: {
                    auto* cmd = (DrawCommand*) cur_ptr;
                    cur_ptr += sizeof(DrawCommand);
                    glBindTexture(GL_TEXTURE_2D, cmd->textureId);
                    r.primitiveShader->setUniform(PrimitiveShaderUniforms.USE_TEXTURE, 1);
                    r.primitiveShader->setUniform(PrimitiveShaderUniforms.IS_FONT, 1);
                    glDrawArrays(GL_TRIANGLES, cmd->offset, cmd->count);
                    break;
                }
                case CommandType::PRIMITIVES: {
                    auto* cmd = (DrawCommand*) cur_ptr;
                    cur_ptr += sizeof(DrawCommand);
                    glBindTexture(GL_TEXTURE_2D, 0);
                    r.primitiveShader->setUniform(PrimitiveShaderUniforms.USE_TEXTURE, 0);
                    r.primitiveShader->setUniform(PrimitiveShaderUniforms.IS_FONT, 0);
                    drawPrimitives(cmd->primitive, cmd->offset, cmd->count);
                    break;
                }
                case CommandType::TEXTURED_PRIMITIVES: {
                    auto* cmd = (DrawCommand*) cur_ptr;
                    cur_ptr += sizeof(DrawCommand);
                    glBindTexture(GL_TEXTURE_2D, cmd->textureId);
                    r.primitiveShader->setUniform(PrimitiveShaderUniforms.USE_TEXTURE, 1);
                    r.primitiveShader->setUniform(PrimitiveShaderUniforms.IS_FONT, 0);
                    drawPrimitives(cmd->primitive, cmd->offset, cmd->count);
                    break;
                }
                case CommandType::TRANSFORM: {
                    auto* cmd = (TransformCommand*) cur_ptr;
                    cur_ptr += sizeof(TransformCommand);
                    r.primitiveShader->setUniform(PrimitiveShaderUniforms.MODEL, cmd->matrix);
                    break;
                }
                case CommandType::BLEND_MODE: {
                    auto* cmd = (BlendModeCommand*) cur_ptr;
                    cur_ptr += sizeof(BlendModeCommand);
                    switch (cmd->mode) {
                        case BlendMode::NONE:
                            glDisable(GL_BLEND);
                            break;
                        case BlendMode::ALPHA:
                            glEnable(GL_BLEND);
                            enableRegularAlpha();
                            break;
                        case BlendMode::PREMULTIPLIED_ALPHA:
                            glEnable(GL_BLEND);
                            enablePremultipliedAlpha();
                            break;
                    }
                    break;
                }
                default:
                    auto* cmd = (Command*) cur_ptr;
                    SDL_Log("Unknown command %hu", cmd->type);
                    break;
            }
        }
    }
}