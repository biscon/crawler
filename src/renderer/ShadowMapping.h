//
// Created by bison on 12-04-24.
//

#ifndef CRAWLER_SHADOWMAPPING_H
#define CRAWLER_SHADOWMAPPING_H

#include "defs.h"

extern "C" {
#include "glad.h"
}
#include "glm/ext.hpp"
#include <glm/gtx/rotate_vector.hpp>

#define NUM_OF_LAYERS 6

namespace Renderer {
    struct CameraDirection
    {
        GLenum CubemapFace;
        glm::vec3 Target;
        glm::vec3 Up;
    };

    class ShadowCubeMapFBO
    {
    public:

        ShadowCubeMapFBO();

        ~ShadowCubeMapFBO();

        bool Init(u32 size);

        void BindForWriting(GLenum CubeFace);

        void BindForReading(GLenum TextureUnit);

    private:

        u32 m_size = 0;
        GLuint m_fbo;
        GLuint m_shadowCubeMap;
        GLuint m_depth;
    };
}
#endif //CRAWLER_SHADOWMAPPING_H
