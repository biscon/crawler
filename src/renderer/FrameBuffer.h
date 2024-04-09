//
// Created by bison on 05-09-23.
//

#ifndef PLATFORMER_FRAMEBUFFER_H
#define PLATFORMER_FRAMEBUFFER_H

#include "defs.h"
#include "Texture.h"
#include <memory>

namespace Renderer {
    u32 CreateFrameBuffer(u32 textureId, TextureFormatInternal internalFormat, TextureFormatData format, i32 width, i32 height);
    void DestroyFrameBuffer(u32 frameBufferId);
    void BindFrameBuffer(u32 frameBufferId);
    void UnbindFrameBuffer();
} // Renderer

#endif //PLATFORMER_FRAMEBUFFER_H
