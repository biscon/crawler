//
// Created by bison on 12-04-24.
//

#ifndef CRAWLER_DEPTHBUFFERFBO_H
#define CRAWLER_DEPTHBUFFERFBO_H

#include "defs.h"

extern "C" {
#include "glad.h"
}
#include "glm/ext.hpp"
#include <glm/gtx/rotate_vector.hpp>

namespace Renderer {
    class DepthBufferFBO
    {
    public:
        DepthBufferFBO();
        ~DepthBufferFBO();
        bool init(i32 w, i32 h);
        void bindForWriting();
        void bindForReading(GLenum TextureUnit);

    private:
        i32 width;
        i32 height;
        GLuint fbo;
        GLuint depthTextureId;
    };
}
#endif
