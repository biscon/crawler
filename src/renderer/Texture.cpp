//
// Created by bison on 05-09-23.
//

#include <stdexcept>
#include <memory>
#include "Texture.h"

extern "C" {
#include "glad.h"
}

namespace Renderer {
    static i32 getGLInternalFormat(TextureFormatInternal format) {
        switch(format) {
            case TextureFormatInternal::R8: return GL_R8;
            case TextureFormatInternal::RGB8: return GL_RGB8;
            case TextureFormatInternal::RGBA8: return GL_RGBA8;
            default:
                throw std::runtime_error("Unknown texture internal format");
        }
    }

    static GLenum getGLFormat(TextureFormatData format) {
        switch(format) {
            case TextureFormatData::RED: return GL_RED;
            case TextureFormatData::RGB: return GL_RGB;
            case TextureFormatData::RGBA: return GL_RGBA;
            default:
                throw std::runtime_error("Unknown texture data format");
        }
    }

    u32 CreateTexture() {
        u32 id;
        glGenTextures(1, &id);
        return id;
    }

    void DestroyTexture(u32 textureId) {
        glDeleteTextures(1, &textureId);
    }

    void BindTexture(uint32_t textureId) {
        glBindTexture(GL_TEXTURE_2D, textureId);
    }

    void UnbindTexture() {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void SetFilteringTexture(uint32_t textureId, TextureFiltering filtering) {
        BindTexture(textureId);
        switch(filtering) {
            case TextureFiltering::NEAREST:
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                break;
            case TextureFiltering::LINEAR:
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                break;
        }
    }

    void UploadTexture(u32 textureId, i32 w, i32 h, u8 *data, TextureFormatInternal internalFormat, TextureFormatData format) {
        BindTexture(textureId);
        glTexImage2D(GL_TEXTURE_2D, 0, getGLInternalFormat(internalFormat),
                     w, h, 0, getGLFormat(format), GL_UNSIGNED_BYTE, (const void*) data);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    }

    void LoadTextureFromPng(u32 textureId, const std::string &filename, bool padding) {
        auto pb = std::make_unique<PixelBuffer>(filename.c_str(), padding);
        pb->verticalFlip();
        UploadTexture(textureId, (i32) pb->width, (i32) pb->height, (u8*) pb->pixels, TextureFormatInternal::RGBA8, TextureFormatData::RGBA);
    }

    void LoadTexture(u32 textureId, const PixelBuffer &pb) {
        UploadTexture(textureId, (i32) pb.width, (i32) pb.height, (u8*) pb.pixels, TextureFormatInternal::RGBA8, TextureFormatData::RGBA);
    }

    void LoadTextureGreyscale(u32 textureId, const PixelBuffer &pb) {
        UploadTexture(textureId, (i32) pb.width, (i32) pb.height, (u8*) pb.pixels, TextureFormatInternal::R8, TextureFormatData::RED);
    }

    void GenerateTextureMipmaps(u32 textureId) {
        glBindTexture(GL_TEXTURE_2D, textureId);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
} // Renderer