//
// Created by bison on 12-04-24.
//

#include <cstdio>
#include <stdexcept>
#include "DepthBufferFBO.h"

namespace Renderer {
    DepthBufferFBO::DepthBufferFBO()
    {
        fbo = 0;
        depthTextureId = 0;
        width = 0;
        height = 0;
    }

    DepthBufferFBO::~DepthBufferFBO()
    {
        if (fbo != 0) {
            glDeleteFramebuffers(1, &fbo);
        }

        if (depthTextureId != 0) {
            glDeleteTextures(1, &depthTextureId);
        }
    }

    bool DepthBufferFBO::init(i32 w, i32 h)
    {
        width = w;
        height = h;

        // Create the depth buffer
        glGenTextures(1, &depthTextureId);
        glBindTexture(GL_TEXTURE_2D, depthTextureId);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glBindTexture(GL_TEXTURE_2D, 0);

        // Create the FBO
        glGenFramebuffers(1, &fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTextureId, 0);

        // Disable writes to the color buffer
        glDrawBuffer(GL_NONE);
        // Disable reads from the color buffer
        glReadBuffer(GL_NONE);

        GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

        if (Status != GL_FRAMEBUFFER_COMPLETE) {
            printf("FB error, status: 0x%x\n", Status);
            return false;
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return true;
    }


    void DepthBufferFBO::bindForWriting()
    {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
        glViewport(0, 0, width, height);
    }


    void DepthBufferFBO::bindForReading(GLenum TextureUnit)
    {
        glActiveTexture(TextureUnit);
        glBindTexture(GL_TEXTURE_2D, depthTextureId);
    }

}
