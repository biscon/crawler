//
// Created by bison on 05-09-23.
//

#ifndef PLATFORMER_TEXTURE_H
#define PLATFORMER_TEXTURE_H

#include "defs.h"
#include "PixelBuffer.h"

namespace Renderer {

    enum class TextureFormatInternal {
        R8,
        RGB8,
        RGBA8,
    };

    enum class TextureFormatData {
        RED,
        RGB,
        RGBA,
    };

    enum class TextureFiltering {
        NEAREST,
        LINEAR,
        NEAREST_MIPMAP,
        LINEAR_MIPMAP,
    };

    u32 CreateTexture();
    void DestroyTexture(u32 textureId);
    void BindTexture(u32 textureId);
    void UnbindTexture();
    void SetFilteringTexture(u32 textureId, TextureFiltering filtering);
    void UploadTexture(u32 textureId, i32 w, i32 h, u8 *data, TextureFormatInternal internalFormat, TextureFormatData format);

    void LoadTextureFromPng(u32 textureId, const std::string& filename, bool padding);
    void LoadTexture(u32 textureId, const PixelBuffer &pb);
    void LoadTextureGreyscale(u32 textureId, const PixelBuffer &pb);
    void GenerateTextureMipmaps(u32 textureId);

} // Renderer

#endif //PLATFORMER_TEXTURE_H
