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

namespace Renderer {
    struct ModelVertex {
        float position[3];
        float textureCoords[2];
        float normal[3];
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
        u32 textureId;
    };

    void LoadModel(Model &model, const std::string &filename, const std::string &textureFile);
    void DestroyModel(Model &model);
}
#endif //CRAWLER_MODEL_H
