//
// Created by bison on 03-09-23.
//

#include "RenderBuffer.h"
#include "../util/string_util.h"
#include <SDL_log.h>
#include <cassert>

extern "C" {
#include "glad.h"
}


namespace Renderer {
    namespace {
        const size_t FLOATS_PER_VERTEX = 8;
        const size_t VERTS_PER_QUAD = 6;
    }

    void CreateRenderBuffer(RenderBuffer &buffer, size_t size) {
        buffer.commands = static_cast<uint8_t *>(calloc(1, size));
        buffer.size = size;
        buffer.cmdOffset = buffer.commands;
        buffer.cmdCount = 0;
        buffer.lastCmdOffset = nullptr;
    }

    void DestroyRenderBuffer(RenderBuffer &buffer) {
        free(buffer.commands);
        buffer.commands = nullptr;
        buffer.size = 0;
        buffer.cmdOffset = nullptr;
    }

    inline static void addDrawCommand(RenderBuffer &buffer, u32 textureId, CommandType type, Primitive primitive, i32 offset, i32 count) {
        // if command is the same as the last one, just increase the vertex count
        if(buffer.lastCmdOffset != nullptr) {
            auto* lastCmdType = (CommandType*) buffer.lastCmdOffset;
            if(*lastCmdType == type) {
                auto* lastCmd = (DrawCommand*) buffer.lastCmdOffset;
                if(lastCmd->textureId == textureId && lastCmd->primitive == primitive) {
                    lastCmd->count += count;
                    return;
                }
            }
        }
        // else add a new command
        DrawCommand cmd = {
                .type = type,
                .textureId = textureId,
                .primitive = primitive,
                .offset = offset,
                .count = count
        };

        memcpy(buffer.cmdOffset, &cmd, sizeof(DrawCommand));
        buffer.lastCmdOffset = buffer.cmdOffset;
        buffer.cmdOffset += sizeof(DrawCommand);
        assert(buffer.cmdOffset - buffer.commands > 0);
        buffer.cmdCount++;
    }

    void PushQuad(RenderBuffer& buffer, const Quad& q) {
        i32 offset = (i32) buffer.vertices.size();
        buffer.vertices.emplace_back(Vertex{ q.right, q.top, q.color.r, q.color.g, q.color.b, q.color.a, 1.0f,0.0f }); // top right
        buffer.vertices.emplace_back(Vertex{ q.right, q.bottom, q.color.r, q.color.g, q.color.b, q.color.a, 1.0f,1.0f }); // bottom right
        buffer.vertices.emplace_back(Vertex{ q.left, q.top, q.color.r, q.color.g, q.color.b, q.color.a, 0.0f,0.0f }); // top left
        buffer.vertices.emplace_back(Vertex{ q.right, q.bottom, q.color.r, q.color.g, q.color.b, q.color.a, 1.0f,1.0f }); // bottom right
        buffer.vertices.emplace_back(Vertex{ q.left, q.bottom, q.color.r, q.color.g, q.color.b, q.color.a, 0.0f,1.0f }); // bottom left
        buffer.vertices.emplace_back(Vertex{ q.left, q.top, q.color.r, q.color.g, q.color.b, q.color.a, 0.0f,0.0f }); // top left
        i32 count = (i32) buffer.vertices.size() - offset;
        addDrawCommand(buffer, 0, CommandType::PRIMITIVES, Primitive::TRIANGLES, offset, count);
    }

    void PushTexturedQuad(RenderBuffer &buffer, const Quad &q, u32 textureId) {
        i32 offset = (i32) buffer.vertices.size();
        buffer.vertices.emplace_back(Vertex{ q.right, q.top, q.color.r, q.color.g, q.color.b, q.color.a, 1.0f,0.0f }); // top right
        buffer.vertices.emplace_back(Vertex{ q.right, q.bottom, q.color.r, q.color.g, q.color.b, q.color.a, 1.0f,1.0f }); // bottom right
        buffer.vertices.emplace_back(Vertex{ q.left, q.top, q.color.r, q.color.g, q.color.b, q.color.a, 0.0f,0.0f }); // top left
        buffer.vertices.emplace_back(Vertex{ q.right, q.bottom, q.color.r, q.color.g, q.color.b, q.color.a, 1.0f,1.0f }); // bottom right
        buffer.vertices.emplace_back(Vertex{ q.left, q.bottom, q.color.r, q.color.g, q.color.b, q.color.a, 0.0f,1.0f }); // bottom left
        buffer.vertices.emplace_back(Vertex{ q.left, q.top, q.color.r, q.color.g, q.color.b, q.color.a, 0.0f,0.0f }); // top left
        i32 count = (i32) buffer.vertices.size() - offset;
        addDrawCommand(buffer, textureId, CommandType::TEXTURED_PRIMITIVES, Primitive::TRIANGLES, offset, count);
    }

    void PushAtlasQuad(RenderBuffer &buffer, const AtlasQuad &q, const TextureAtlas& atlas) {
        i32 offset = (i32) buffer.vertices.size();
        FloatRect uv = atlas.uvRects.at(q.atlasId);
        buffer.vertices.emplace_back(Vertex{ q.right, q.top, q.color.r, q.color.g, q.color.b, q.color.a, uv.right, uv.top }); // top right
        buffer.vertices.emplace_back(Vertex{ q.right, q.bottom, q.color.r, q.color.g, q.color.b, q.color.a, uv.right, uv.bottom }); // bottom right
        buffer.vertices.emplace_back(Vertex{ q.left, q.top, q.color.r, q.color.g, q.color.b, q.color.a, uv.left, uv.top }); // top left
        buffer.vertices.emplace_back(Vertex{ q.right, q.bottom, q.color.r, q.color.g, q.color.b, q.color.a, uv.right, uv.bottom }); // bottom right
        buffer.vertices.emplace_back(Vertex{ q.left, q.bottom, q.color.r, q.color.g, q.color.b, q.color.a, uv.left, uv.bottom }); // bottom left
        buffer.vertices.emplace_back(Vertex{ q.left, q.top, q.color.r, q.color.g, q.color.b, q.color.a, uv.left, uv.top }); // top left
        i32 count = (i32) buffer.vertices.size() - offset;
        addDrawCommand(buffer, atlas.textureId, CommandType::TEXTURED_PRIMITIVES, Primitive::TRIANGLES, offset, count);
    }

    void PushTriangleMesh(RenderBuffer &buffer, const std::vector<Vector2> &verts, const Color &color) {
        i32 offset = (i32) buffer.vertices.size();
        for(auto& v : verts) {
            buffer.vertices.emplace_back(Vertex{ v.x, v.y, color.r, color.g, color.b, color.a, 0.0f,0.0f }); // top right
        }
        i32 count = (i32) buffer.vertices.size() - offset;
        addDrawCommand(buffer, 0, CommandType::PRIMITIVES, Primitive::TRIANGLES, offset, count);
    }

    void PushLine(RenderBuffer &buffer, const Vector2& from, const Vector2& to, const Color& color) {
        i32 offset = (i32) buffer.vertices.size();
        buffer.vertices.emplace_back(Vertex{ from.x, from.y, color.r, color.g, color.b, color.a, 0.0f,0.0f }); // from
        buffer.vertices.emplace_back(Vertex{ to.x, to.y, color.r, color.g, color.b, color.a, 0.0f,0.0f }); // to
        i32 count = (i32) buffer.vertices.size() - offset;
        addDrawCommand(buffer, 0, CommandType::PRIMITIVES, Primitive::LINES, offset, count);
    }

    static void drawTextAtlasQuad(RenderBuffer &buffer, const AtlasQuad &q, const TextureAtlas& atlas) {
        i32 offset = (i32) buffer.vertices.size();
        FloatRect uv = atlas.uvRects.at(q.atlasId);
        buffer.vertices.emplace_back(Vertex{ q.right, q.top, q.color.r, q.color.g, q.color.b, q.color.a, uv.right, uv.top }); // top right
        buffer.vertices.emplace_back(Vertex{ q.right, q.bottom, q.color.r, q.color.g, q.color.b, q.color.a, uv.right, uv.bottom }); // bottom right
        buffer.vertices.emplace_back(Vertex{ q.left, q.top, q.color.r, q.color.g, q.color.b, q.color.a, uv.left, uv.top }); // top left
        buffer.vertices.emplace_back(Vertex{ q.right, q.bottom, q.color.r, q.color.g, q.color.b, q.color.a, uv.right, uv.bottom }); // bottom right
        buffer.vertices.emplace_back(Vertex{ q.left, q.bottom, q.color.r, q.color.g, q.color.b, q.color.a, uv.left, uv.bottom }); // bottom left
        buffer.vertices.emplace_back(Vertex{ q.left, q.top, q.color.r, q.color.g, q.color.b, q.color.a, uv.left, uv.top }); // top left
        i32 count = (i32) buffer.vertices.size() - offset;
        addDrawCommand(buffer, atlas.textureId, CommandType::TEXT, Primitive::TRIANGLES, offset, count);
    }

    void PushText(RenderBuffer& buffer, const std::string& text, const Font& font, float x, float y,
                  const Color& color) {
        size_t len = text.size();
        std::vector<AtlasQuad> quads;

        for (size_t i = 0; i < len;) {
            // TODO(Brett): if the character is null, attempt to fetch it from the font
            u32 cpLen;
            u32 cp = DecodeCodePoint(&cpLen, &text[i]);

            const Glyph *c = &font.glyphs.at(cp);

            //assert(c != NULL);
            i += cpLen;
            if (c == nullptr)
                continue;
            float xp, yp, h, w;
            xp = x + c->bearing[0];
            yp = y - c->bearing[1];
            w = c->size[0];
            h = c->size[1];
            AtlasQuad quad = {
                    .color      = {color.r, color.g, color.b, color.a},
                    .atlasId    = c->atlasId,
                    .left       = xp,
                    .top        = yp,
                    .right      = xp + w,
                    .bottom     = yp + h
            };
            quads.emplace_back(quad);
            x += (float) c->advance;
        }

        //SDL_Log("Generated %d glyph quads", quadStore.noItems);
        for(auto& q : quads) {
            drawTextAtlasQuad(buffer, q, font.atlas);
        }
    }

    void PushTransform(RenderBuffer& buffer, const glm::mat4& matrix) {
        TransformCommand cmd = {
                .type = CommandType::TRANSFORM,
                .matrix = matrix
        };
        memcpy(buffer.cmdOffset, &cmd, sizeof(TransformCommand));
        buffer.lastCmdOffset = buffer.cmdOffset;
        buffer.cmdOffset += sizeof(TransformCommand);
        assert(buffer.cmdOffset - buffer.commands > 0);
        buffer.cmdCount++;
    }

    void PushBlendMode(RenderBuffer& buffer, BlendMode mode) {
        BlendModeCommand cmd = {
                .type = CommandType::BLEND_MODE,
                .mode = mode
        };
        memcpy(buffer.cmdOffset, &cmd, sizeof(BlendModeCommand));
        buffer.lastCmdOffset = buffer.cmdOffset;
        buffer.cmdOffset += sizeof(BlendModeCommand);
        assert(buffer.cmdOffset - buffer.commands > 0);
        buffer.cmdCount++;
    }

    void CopyToVertexBuffer(RenderBuffer &buffer, VertexBuffer &vertexBuffer) {
        if(buffer.vertices.empty())
            return;
        vertexBuffer.bind();
        vertexBuffer.allocate(buffer.vertices.data(), buffer.vertices.size() * sizeof(Vertex), VertexAccessType::STATIC);
    }

    void Clear(RenderBuffer &buffer) {
        buffer.vertices.clear();
        memset(buffer.commands, 0, buffer.size);
        buffer.cmdOffset = buffer.commands;
        buffer.cmdCount = 0;
        buffer.lastCmdOffset = nullptr;
    }
}
