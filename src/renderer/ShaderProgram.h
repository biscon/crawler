//
// Created by bison on 04-09-23.
//

#ifndef PLATFORMER_SHADERPROGRAM_H
#define PLATFORMER_SHADERPROGRAM_H

#include <unordered_map>
#include <memory>
#include "defs.h"
#include <string>
#include "glm/glm.hpp"

namespace Renderer {
    class ShaderProgram {
    public:
        ShaderProgram(const std::string& vertFilename, const std::string& fragFilename);
        ~ShaderProgram();
        void use() const;
        void setupUniform(const std::string& name);
        void setUniform(const std::string& name, float value);
        void setUniform(const std::string& name, i32 value);
        void setUniform(const std::string& name, glm::mat3 value);
        void setUniform(const std::string& name, glm::mat4 value);
        void setUniform(const std::string& name, glm::vec3 value);
        void setUniform(const std::string& name, glm::vec4 value);

        void setInt(const std::string &name, int value);
        void setFloat(const std::string &name, float value);
        void setVec2(const std::string &name, const glm::vec2 &value);
        void setVec2(const std::string &name, float x, float y);
        void setVec3(const std::string &name, const glm::vec3 &value);
        void setVec3(const std::string &name, float x, float y, float z);
        void setVec4(const std::string &name, const glm::vec4 &value);
        void setVec4(const std::string &name, float x, float y, float z, float w);
        void setMat2(const std::string &name, const glm::mat2 &mat);
        void setMat3(const std::string &name, const glm::mat3 &mat);
        void setMat4(const std::string &name, const glm::mat4 &mat);

        void setVec3Array(const std::string &name, const glm::vec3 *values, u32 count);

    private:
        u32 id = 0;
        std::unordered_map<std::string, i32> uniforms;
    };

}



#endif //PLATFORMER_SHADERPROGRAM_H
