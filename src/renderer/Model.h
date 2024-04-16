//
// Created by bison on 07-04-24.
//

#ifndef CRAWLER_MODEL_H
#define CRAWLER_MODEL_H

#include <vector>
#include <memory>
#include <unordered_map>
#include "defs.h"
#include "VertexBuffer.h"
#include <glm/glm.hpp>

namespace Renderer {
    struct ModelVertex {
        glm::vec3 position;
        glm::vec2 textureCoords;
        glm::vec3 normal;
        glm::vec3 tangent;
    };

    struct ModelObject {
        std::string name;
        u32 offset;
        u32 count;
    };

    struct Model {
        std::shared_ptr<VertexBuffer> vbo;
        std::vector<ModelVertex> vertices;
        std::unordered_map<std::string, ModelObject> objects;
        u32 diffuseMapId;
        u32 normalMapId;
        u32 specularMapId;
        bool hasNormalMap;
        bool hasSpecularMap;
    };

    void LoadModel(Model &model, const std::string &filename, bool generateTangents);
    void LoadDiffuseMap(Model &model, const std::string &filename);
    void LoadNormalMap(Model &model, const std::string &filename);
    void LoadSpecularMap(Model &model, const std::string &filename);
    void DestroyModel(Model &model);
}
#endif //CRAWLER_MODEL_H
