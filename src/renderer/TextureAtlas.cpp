//
// Created by bison on 31-10-22.
//

#include <SDL_log.h>
#include "TextureAtlas.h"
#include "Texture.h"

#define STB_RECT_PACK_IMPLEMENTATION
#include <stb_rect_pack.h>

namespace Renderer {
    TextureAtlasBuilder::TextureAtlasBuilder(i32 width, i32 height, PixelFormat format): format(format), width(width), height(height) {
        noRects = 0;
        nextEntryId = 1;
    }

    u32 TextureAtlasBuilder::add(const PixelBuffer &pb) {
        auto id = nextEntryId++;
        auto newPb = PixelBuffer(pb);
        stbrp_rect rect = {};
        rect.id = (i32) id;
        rect.w = (stbrp_coord) newPb.width;
        rect.h = (stbrp_coord) newPb.height;
        rect.was_packed = 0;
        images.emplace_back(TextureAtlasBuilderImage{id, newPb});
        rects.emplace_back(rect);
        noRects++;
        return id;
    }

    u32 TextureAtlasBuilder::addFromPng(const std::string &filename, bool pad) {
        auto id = nextEntryId++;
        auto newPb = PixelBuffer(filename, pad);
        stbrp_rect rect = {};
        rect.id = (i32) id;
        rect.w = (stbrp_coord) newPb.width;
        rect.h = (stbrp_coord) newPb.height;
        rect.was_packed = 0;
        images.emplace_back(TextureAtlasBuilderImage{id, newPb});
        rects.emplace_back(rect);
        noRects++;
        return id;
    }

    u32 TextureAtlasBuilder::addFromPngSize(const std::string &filename, bool pad, i32& w, i32& h) {
        auto id = nextEntryId++;
        auto newPb = PixelBuffer(filename, pad);
        w = (i32) newPb.width;
        h = (i32) newPb.height;
        stbrp_rect rect = {};
        rect.id = (i32) id;
        rect.w = (stbrp_coord) newPb.width;
        rect.h = (stbrp_coord) newPb.height;
        rect.was_packed = 0;
        images.emplace_back(TextureAtlasBuilderImage{id, newPb});
        rects.emplace_back(rect);
        noRects++;
        return id;
    }

    void TextureAtlasBuilder::build(TextureAtlas &atlas) {
        atlas.uvRects.clear();
        stbrp_context context;
        memset(&context, 0, sizeof(stbrp_context));

        // TODO this might overflow the stack
        i32 nodeCount = width * 2;
        struct stbrp_node nodes[nodeCount];

        stbrp_init_target(&context, width, height, nodes, nodeCount);
        stbrp_pack_rects(&context, rects.data(), (i32) rects.size());

        auto buffer = PixelBuffer((u32) width, (u32) height, format);
        auto atlasWidth = (float) width;
        auto atlasHeight = (float) height;

        for(i32 i = 0; i < (i32) rects.size(); ++i) {
            auto &curRect = rects[i];
            auto &curImage = images[i];
            auto uvRect = FloatRect();
            if (!curRect.was_packed) {
                SDL_Log("Failed to pack image %d", curRect.id);
                continue;
            }
            if (!curImage.pb.padding) {
                uvRect.left = ((curRect.x) / atlasWidth);
                uvRect.right = ((curRect.x + curRect.w) / atlasWidth);
                uvRect.top = ((curRect.y) / atlasHeight);
                uvRect.bottom = ((curRect.y + curRect.h) / atlasHeight);
            } else {
                uvRect.left = ((curRect.x + 1.0f) / atlasWidth);
                uvRect.right = ((curRect.x + curRect.w - 1.0f) / atlasWidth);
                uvRect.top = ((curRect.y + 1.0f) / atlasHeight);
                uvRect.bottom = ((curRect.y + curRect.h - 1.0f) / atlasHeight);
            }
            atlas.uvRects[curRect.id] = uvRect;
            buffer.copyFrom(curImage.pb, UIntRect(0, 0, curImage.pb.width, curImage.pb.height),
                             UIntPos((u32) curRect.x, (u32) curRect.y));
        }
        atlas.textureId = CreateTexture();

        if (format == PixelFormat::RGBA) {
            LoadTexture(atlas.textureId, buffer);
        } else {
            LoadTextureGreyscale(atlas.textureId, buffer);
        }
        SetFilteringTexture(atlas.textureId, TextureFiltering::NEAREST);
    }

    void DestroyTextureAtlas(TextureAtlas &atlas) {
        DestroyTexture(atlas.textureId);
    }
}