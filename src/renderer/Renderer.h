//
// Created by bison on 04-10-23.
//

#ifndef DOD_RENDERER_H
#define DOD_RENDERER_H

#include <memory>
#include "ShaderProgram.h"
#include "FrameBuffer.h"
#include "VertexBuffer.h"
#include "RenderBuffer.h"

namespace Renderer {

    struct Renderer {
        std::unique_ptr<ShaderProgram> primitiveShader;
        std::unique_ptr<ShaderProgram> screenShader;
        u32 lightFBO;
        u32 lightFBOTexture;
        u32 bgFBO;
        u32 bgFBOTexture;
        u32 unlitFBO;
        u32 unlitFBOTexture;
        u32 litFBO;
        u32 litFBOTexture;

        std::unique_ptr<VertexBuffer> screenVBO;
        std::unique_ptr<VertexBuffer> primitiveVBO;
        RenderBuffer renderBuffer;
    };

    void InitRenderer(Renderer& r);
    void UpdateRenderer(Renderer& r, float frameDelta);
    void ShutdownRenderer(Renderer& renderer);
    void Render(Renderer& r, RenderBuffer& buffer);
}
#endif //DOD_RENDERER_H
