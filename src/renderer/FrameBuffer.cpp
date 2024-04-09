//
// Created by bison on 05-09-23.
//

#include <stdexcept>
#include "FrameBuffer.h"

extern "C" {
#include "glad.h"
}

namespace Renderer {
    u32 CreateFrameBuffer(u32 textureId, TextureFormatInternal internalFormat, TextureFormatData format, i32 width,
                      i32 height) {
        u32 id;
        glGenFramebuffers(1, &id);
        glBindFramebuffer(GL_FRAMEBUFFER, id);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureId, 0);
        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            throw std::runtime_error("FBO could not be completed!!");
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return id;
    }

    void DestroyFrameBuffer(u32 frameBufferId) {
        glDeleteFramebuffers(1, &frameBufferId);
    }

    void BindFrameBuffer(u32 frameBufferId) {
        glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);
    }

    void UnbindFrameBuffer() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
} // Renderer