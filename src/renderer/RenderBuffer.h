//
// Created by bison on 03-09-23.
//

#ifndef RENDERBUFFER_H
#define RENDERBUFFER_H

#include <memory>
#include "defs.h"
#include <string>
#include <vector>
#include <unordered_map>
#include "glm/glm.hpp"

#include "VertexBuffer.h"
#include "Texture.h"
#include "PixelBuffer.h"
#include "TextureAtlas.h"
#include "Font.h"


namespace Renderer {

    struct Color {
        float r, g, b, a;
    };

    const Color BLACK = {0.0f, 0.0f, 0.0f, 1.0f};
    const Color WHITE = {1.0f, 1.0f, 1.0f, 1.0f};
    const Color RED = {1.0f, 0.0f, 0.0f, 1.0f};
    const Color YELLOW = {1.0f, 1.0f, 0.0f, 1.0f};
    const Color BLUE = {0.0f, 0.0f, 1.0f, 1.0f};
    const Color GREEN = {0.0f, 1.0f, 0.0f, 1.0f};
    const Color GREY = {0.5f, 0.5f, 0.5f, 1.0f};
    const Color DARK_GREY = {0.3f, 0.3f, 0.3f, 1.0f};
    const Color TRANSPARENT = {0.0f, 0.0f, 0.0f, 0.0f};
    
    struct Quad {
        Color                           color;
        float                           left;
        float                           top;
        float                           right;
        float                           bottom;

        void setBounds(FloatRect& bnds) {
            left = bnds.left;
            top = bnds.top;
            right = bnds.right;
            bottom = bnds.bottom;
        }
    };

    struct Vertex {
        float x, y, r, g, b, a, u, v;
    };

    struct AtlasQuad {
        Color                           color;
        u32                             atlasId;
        float                           left;
        float                           top;
        float                           right;
        float                           bottom;
    };

    enum class CommandType : u16 {
        PRIMITIVES = 1,
        TEXTURED_PRIMITIVES = 2,
        BLEND_MODE = 3,
        TRANSFORM = 4,
        TEXT = 5
    };

    enum class Primitive: u16 {
        TRIANGLES = 1,
        LINES = 2,
        TRIANGLE_STRIP = 3,
    };

    enum class BlendMode: u16 {
        NONE,
        ALPHA,
        PREMULTIPLIED_ALPHA,
    };

    struct Command {
        CommandType type;
    };

    struct ClearCommand {
        CommandType type;
        Color color;
    };

    struct DrawCommand {
        CommandType type;
        u32 textureId;
        Primitive primitive;
        i32 offset; // in vertices
        i32 count; // in vertices
    };

    struct TransformCommand {
        CommandType type;
        glm::mat4 matrix;
    };

    struct BlendModeCommand {
        CommandType type;
        BlendMode mode;
    };

    struct RenderBuffer {
        std::vector<Vertex> vertices;
        u8* commands;
        u8* cmdOffset;
        size_t size;
        size_t cmdCount;
        u8* lastCmdOffset;
    };

    // merge to previous buffer if identical data, for instance to successive calls to PushTexturedQuad

    void CreateRenderBuffer(RenderBuffer& buffer, size_t size);
    void DestroyRenderBuffer(RenderBuffer& buffer);

    void PushQuad(RenderBuffer& buffer, const Quad& q);
    void PushTexturedQuad(RenderBuffer& buffer, const Quad& q, u32 textureId);
    void PushAtlasQuad(RenderBuffer &buffer, const AtlasQuad &q, const TextureAtlas& atlas);
    void PushLine(RenderBuffer &buffer, const Vector2& from, const Vector2& to, const Color& color);
    void PushText(RenderBuffer& buffer, const std::string& text, const Font& font, float x, float y, const Color& color);
    void PushTransform(RenderBuffer& buffer, const glm::mat4& matrix);
    void PushBlendMode(RenderBuffer& buffer, BlendMode mode);
    void PushTriangleMesh(RenderBuffer &buffer, const std::vector<Vector2> &verts, const Color &color);
    void CopyToVertexBuffer(RenderBuffer& buffer, VertexBuffer& vertexBuffer);
    void Clear(RenderBuffer &buffer);
}


#endif
