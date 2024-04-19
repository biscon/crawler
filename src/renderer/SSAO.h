//
// Created by bison on 17-04-24.
//

#ifndef CRAWLER_SSAO_H
#define CRAWLER_SSAO_H
#include "defs.h"

extern "C" {
#include "glad.h"
}
#include "glm/glm.hpp"
#include "ShaderProgram.h"

namespace Renderer {

    struct SSAO {
        virtual ~SSAO();
        bool enabled;
        //glm::vec3 samples[64];
        std::vector<glm::vec3> kernel;
        float radius;
        float bias;
        u32 noiseTextureId;
        i32 width;
        i32 height;
        GLuint fbo;
        GLuint ssaoTextureId;

        void init(i32 w, i32 h);
        void setUniforms(ShaderProgram& shader);
        void bindTexture(u32 unit);


    };

} // Renderer

#endif //CRAWLER_SSAO_H
