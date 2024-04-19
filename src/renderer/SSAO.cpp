//
// Created by bison on 17-04-24.
//

#include <random>
#include <iostream>
#include "SSAO.h"

namespace Renderer {
    void SSAO::init(i32 w, i32 h) {
        kernel.clear();
        radius = 0.5f;
        bias = 0.025f;
        width = w;
        height = h;
        std::uniform_real_distribution<float> randomFloats(0.0, 1.0); // random floats between [0.0, 1.0]
        std::default_random_engine generator;
        for (u32 i = 0; i < 64; ++i)
        {
            glm::vec3 sample(
                    randomFloats(generator) * 2.0 - 1.0,
                    randomFloats(generator) * 2.0 - 1.0,
                    randomFloats(generator)
            );
            sample  = glm::normalize(sample);
            sample *= randomFloats(generator);
            kernel.push_back(sample);
        }
        std::vector<glm::vec3> ssaoNoise;
        for (u32 i = 0; i < 16; i++)
        {
            glm::vec3 noise(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, 0.0f); // rotate around z-axis (in tangent space)
            ssaoNoise.push_back(noise);
        }

        glGenTextures(1, &noiseTextureId);
        glBindTexture(GL_TEXTURE_2D, noiseTextureId);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 4, 4, 0, GL_RGB, GL_FLOAT, ssaoNoise.data());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        // setup fbo
        glGenTextures(1, &ssaoTextureId);
        glBindTexture(GL_TEXTURE_2D, ssaoTextureId);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoTextureId, 0);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "SSAO Framebuffer not complete!" << std::endl;


        glBindTexture(GL_TEXTURE_2D, 0);

        // Create the FBO
        glGenFramebuffers(1, &fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, ssaoTextureId, 0);

        // Disable writes to the color buffer
        glDrawBuffer(GL_NONE);
        // Disable reads from the color buffer
        glReadBuffer(GL_NONE);

        GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

        if (Status != GL_FRAMEBUFFER_COMPLETE) {
            printf("FB error, status: 0x%x\n", Status);
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    SSAO::~SSAO() {
        glDeleteTextures(1, &noiseTextureId);
    }

    void SSAO::setUniforms(ShaderProgram &shader) {
        shader.setInt("ssao.enabled", enabled ? 1 : 0);
        shader.setFloat("ssao.radius", radius);
        shader.setFloat("ssao.bias", bias);
        shader.setInt("ssao.noiseTexture", 5);
        shader.setVec3Array("ssao.samples", kernel.data(), kernel.size());
        shader.setInt("ssao.kernelSize", kernel.size());
    }

    void SSAO::bindTexture(u32 unit) {
        glActiveTexture(unit);
        glBindTexture(GL_TEXTURE_2D, noiseTextureId);
    }
} // Renderer