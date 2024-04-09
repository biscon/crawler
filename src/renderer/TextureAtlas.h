//
// Created by bison on 31-10-22.
//

#ifndef GAME_TEXTUREATLAS_H
#define GAME_TEXTUREATLAS_H

#include <defs.h>
#include <unordered_map>
#include <vector>
#include <memory>
#include "Vector2.h"
#include "PixelBuffer.h"
#include <stb_rect_pack.h>

namespace Renderer {
    struct TextureAtlas {
        u32 textureId;
        std::unordered_map<u32, FloatRect> uvRects;
    };

    struct TextureAtlasBuilderImage {
        u32 id;
        PixelBuffer pb;
    };

    class TextureAtlasBuilder {
    public:
        TextureAtlasBuilder(i32 width, i32 height, PixelFormat format);
        ~TextureAtlasBuilder() = default;
        u32 add(const PixelBuffer& pb);
        u32 addFromPng(const std::string& filename, bool pad);
        u32 addFromPngSize(const std::string &filename, bool pad, i32& w, i32& h);
        void build(TextureAtlas& atlas);

    private:
        std::vector<TextureAtlasBuilderImage> images;
        std::vector<stbrp_rect> rects;
        PixelFormat format;
        i32 noRects;
        i32 width;
        i32 height;
        u32 nextEntryId;
    };

    void DestroyTextureAtlas(TextureAtlas& atlas);
}



#endif //GAME_TEXTUREATLAS_H
