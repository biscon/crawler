//
// Created by bison on 28-01-2018.
//

#ifndef GAME_OGLARRAYTEXTURE_H
#define GAME_OGLARRAYTEXTURE_H

#include "defs.h"
#include "PixelBuffer.h"
#include "Texture.h"

namespace Renderer {
    class ArrayTexture {
    public:
        void create(i32 width, i32 height, i32 layers);

        void uploadLayer(const std::string& filename, i32 layer);
        void uploadLayer(PixelBuffer& pb, i32 layer);
        void setFilteringTexture(TextureFiltering filtering);
        void bind();
        void bind(u32 TextureUnit);
        void generateMipmaps();

        u32 tex;
        i32 width;
        i32 height;
        i32 layers;
    private:
    };
}
#endif //GAME_OGLARRAYTEXTURE_H
