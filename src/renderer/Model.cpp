//
// Created by bison on 07-04-24.
//

#include <iostream>
#include <SDL_log.h>
#include "Model.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#include "Texture.h"
#include "glm/vec3.hpp"
#include "glm/geometric.hpp"
#include "glm/gtc/type_ptr.hpp"

tinyobj::ObjReaderConfig reader_config;

namespace Renderer {
    static glm::vec3 calculateNormal(const ModelVertex& vertex1, const ModelVertex& vertex2, const ModelVertex& vertex3) {
        glm::vec3 edge1(vertex2.position[0] - vertex1.position[0],
                        vertex2.position[1] - vertex1.position[1],
                        vertex2.position[2] - vertex1.position[2]);
        glm::vec3 edge2(vertex3.position[0] - vertex1.position[0],
                        vertex3.position[1] - vertex1.position[1],
                        vertex3.position[2] - vertex1.position[2]);
        glm::vec3 normal = glm::cross(edge1, edge2);
        return glm::normalize(normal);
    }

    static float calculateThreshold(const std::vector<ModelVertex>& vertices) {
        // Find the maximum extents of the model along x, y, and z axes
        float maxX = std::numeric_limits<float>::lowest();
        float maxY = std::numeric_limits<float>::lowest();
        float maxZ = std::numeric_limits<float>::lowest();
        float minX = std::numeric_limits<float>::max();
        float minY = std::numeric_limits<float>::max();
        float minZ = std::numeric_limits<float>::max();

        for (const auto& vertex : vertices) {
            for (int i = 0; i < 3; ++i) {
                maxX = std::max(maxX, vertex.position[0]);
                maxY = std::max(maxY, vertex.position[1]);
                maxZ = std::max(maxZ, vertex.position[2]);
                minX = std::min(minX, vertex.position[0]);
                minY = std::min(minY, vertex.position[1]);
                minZ = std::min(minZ, vertex.position[2]);
            }
        }

        // Calculate the model's bounding box dimensions
        float modelWidth = maxX - minX;
        float modelHeight = maxY - minY;
        float modelDepth = maxZ - minZ;

        // Choose the maximum dimension of the bounding box as the threshold
        //float threshold = std::max(std::max(modelWidth, modelHeight), modelDepth);

        // Alternatively, you can use a fraction of the maximum dimension as the threshold
        float threshold = std::max(std::max(modelWidth, modelHeight), modelDepth) * 0.5f; // Adjust 0.1f according to your needs

        return threshold;
    }


    // Function to smooth normals for a given vertex
    static void smoothNormals(std::vector<ModelVertex>& vertices) {
        auto threshold = calculateThreshold(vertices);
        //auto threshold = 0.1f;
        SDL_Log("Threshold: %f", threshold);
        for (size_t i = 0; i < vertices.size(); ++i) {
            glm::vec3 averageNormal(0.0f);
            int count = 0;

            // Loop through neighboring polygons (triangles)
            for (size_t j = 0; j < vertices.size(); ++j) {
                if (i != j) {
                    float distance = glm::distance(glm::make_vec3(vertices[i].position), glm::make_vec3(vertices[j].position));
                    if (distance < threshold) { // Threshold to consider neighboring vertices
                        auto normal = glm::vec3(vertices[j].normal[0], vertices[j].normal[1], vertices[j].normal[2]);
                        averageNormal += normal;
                        count++;
                    }
                }
            }

            if (count > 0) {
                // Average the normals and normalize the result
                auto normal = glm::normalize(averageNormal / static_cast<float>(count));
                vertices[i].normal[0] = normal.x;
                vertices[i].normal[1] = normal.y;
                vertices[i].normal[2] = normal.z;
            }
        }
    }

    static void calculateGeometryTangents(std::vector<ModelVertex>& vertices) {
        for(i32 i = 0 ; i < vertices.size(); i += 3) {
            ModelVertex& v0 = vertices[i];
            ModelVertex& v1 = vertices[i+1];
            ModelVertex& v2 = vertices[i+2];

            glm::vec3 Edge1 = v1.position - v0.position;
            glm::vec3 Edge2 = v2.position - v0.position;

            float DeltaU1 = v1.textureCoords.x - v0.textureCoords.x;
            float DeltaV1 = v1.textureCoords.y - v0.textureCoords.y;
            float DeltaU2 = v2.textureCoords.x - v0.textureCoords.x;
            float DeltaV2 = v2.textureCoords.y - v0.textureCoords.y;

            float f = 1.0f / (DeltaU1 * DeltaV2 - DeltaU2 * DeltaV1);

            glm::vec3 Tangent;
            Tangent.x = f * (DeltaV2 * Edge1.x - DeltaV1 * Edge2.x);
            Tangent.y = f * (DeltaV2 * Edge1.y - DeltaV1 * Edge2.y);
            Tangent.z = f * (DeltaV2 * Edge1.z - DeltaV1 * Edge2.z);

            Tangent = glm::normalize(Tangent);
            v0.tangent = Tangent;
            v1.tangent = Tangent;
            v2.tangent = Tangent;
        }
    }

    void LoadModel(Model &model, const std::string &filename, bool generateTangents) {
        model.hasNormalMap = false;
        model.hasSpecularMap = false;
        model.diffuseMapId = 0;
        model.normalMapId = 0;
        model.specularMapId = 0;
        model.vertices.clear();
        reader_config.mtl_search_path = "./assets/models"; // Path to material files
        tinyobj::ObjReader reader;

        if (!reader.ParseFromFile(filename, reader_config)) {
            if (!reader.Error().empty()) {
                std::cerr << "TinyObjReader: " << reader.Error();
            }
            exit(1);
        }

        if (!reader.Warning().empty()) {
            //std::cout << "TinyObjReader: " << reader.Warning();
        }

        auto& attrib = reader.GetAttrib();
        auto& shapes = reader.GetShapes();

        /*
        SDL_Log("Number of vertices: %d", int(attrib.vertices.size() / 3));
        SDL_Log("Number of normals: %d", int(attrib.normals.size() / 3));
        SDL_Log("Number of texcoords: %d", int(attrib.texcoords.size() / 2));
        */
        SDL_Log("Number of shapes: %d", int(shapes.size()));

        // Loop over shapes
        for (size_t s = 0; s < shapes.size(); s++) {
            ModelObject modelObject;
            modelObject.name = shapes[s].name;
            modelObject.offset = model.vertices.size();
            SDL_Log("Shape: %s", shapes[s].name.c_str());
            // Loop over faces(polygon)
            size_t index_offset = 0;
            for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
                auto fv = size_t(shapes[s].mesh.num_face_vertices[f]);

                ModelVertex modelVertex{};

                // Loop over vertices in the face.
                for (size_t v = 0; v < fv; v++) {
                    // access to vertex
                    tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
                    tinyobj::real_t vx = attrib.vertices[3 * size_t(idx.vertex_index) + 0];
                    tinyobj::real_t vy = attrib.vertices[3 * size_t(idx.vertex_index) + 1];
                    tinyobj::real_t vz = attrib.vertices[3 * size_t(idx.vertex_index) + 2];
                    modelVertex.position[0] = vx;
                    modelVertex.position[1] = vy;
                    modelVertex.position[2] = vz;
                    // Check if `normal_index` is zero or positive. negative = no normal data
                    if (idx.normal_index >= 0) {
                        tinyobj::real_t nx = attrib.normals[3 * size_t(idx.normal_index) + 0];
                        tinyobj::real_t ny = attrib.normals[3 * size_t(idx.normal_index) + 1];
                        tinyobj::real_t nz = attrib.normals[3 * size_t(idx.normal_index) + 2];
                        modelVertex.normal[0] = nx;
                        modelVertex.normal[1] = ny;
                        modelVertex.normal[2] = nz;
                    }

                    // Check if `texcoord_index` is zero or positive. negative = no texcoord data
                    if (idx.texcoord_index >= 0) {
                        tinyobj::real_t tx = attrib.texcoords[2 * size_t(idx.texcoord_index) + 0];
                        tinyobj::real_t ty = attrib.texcoords[2 * size_t(idx.texcoord_index) + 1];
                        modelVertex.textureCoords[0] = tx;
                        modelVertex.textureCoords[1] = ty;
                    }

                    // Optional: vertex colors
                    // tinyobj::real_t red   = attrib.colors[3*size_t(idx.vertex_index)+0];
                    // tinyobj::real_t green = attrib.colors[3*size_t(idx.vertex_index)+1];
                    // tinyobj::real_t blue  = attrib.colors[3*size_t(idx.vertex_index)+2];
                    model.vertices.emplace_back(modelVertex);
                }
                index_offset += fv;
                modelObject.count = model.vertices.size() - modelObject.offset;
                model.objects[shapes[s].name] = modelObject;
            }
        }
        SDL_Log("Number of vertices in model: %zu", model.vertices.size());
        // calculate normals
        /*
        for (size_t i = 0; i < model.vertices.size(); i += 3) {
            auto& v1 = model.vertices[i];
            auto& v2 = model.vertices[i + 1];
            auto& v3 = model.vertices[i + 2];
            glm::vec3 normal = calculateNormal(v1, v2, v3);
            v1.normal[0] = normal.x;
            v1.normal[1] = normal.y;
            v1.normal[2] = normal.z;
            v2.normal[0] = normal.x;
            v2.normal[1] = normal.y;
            v2.normal[2] = normal.z;
            v3.normal[0] = normal.x;
            v3.normal[1] = normal.y;
            v3.normal[2] = normal.z;
        }
        smoothNormals(model.vertices);
         */

        if(generateTangents) {
            calculateGeometryTangents(model.vertices);
            SDL_Log("Generated tangents");
        }

        // vbo
        VertexAttributes attrs;
        attrs.add(0, 3, VertexAttributeType::Float); // position
        attrs.add(1, 2, VertexAttributeType::Float); // tex coords
        attrs.add(2, 3, VertexAttributeType::Float); // normals
        attrs.add(3, 3, VertexAttributeType::Float); // tangents
        model.vbo = std::make_shared<VertexBuffer>(attrs);
        model.vbo->allocate(model.vertices.data(), model.vertices.size() * sizeof(ModelVertex), VertexAccessType::STATIC);
    }

    void DestroyModel(Model &model) {
        DestroyTexture(model.diffuseMapId);
    }

    void LoadDiffuseMap(Model &model, const std::string &filename) {
        model.diffuseMapId = CreateTexture();
        LoadTextureFromPng(model.diffuseMapId, filename, false);
        SetFilteringTexture(model.diffuseMapId, TextureFiltering::LINEAR_MIPMAP);
        GenerateTextureMipmaps(model.diffuseMapId);
    }

    void LoadNormalMap(Model &model, const std::string &filename) {
        model.normalMapId = CreateTexture();
        LoadTextureFromPng(model.normalMapId, filename, false);
        SetFilteringTexture(model.normalMapId, TextureFiltering::LINEAR_MIPMAP);
        GenerateTextureMipmaps(model.normalMapId);
        model.hasNormalMap = true;
    }

    void LoadSpecularMap(Model &model, const std::string &filename) {
        model.specularMapId = CreateTexture();
        LoadTextureFromPng(model.specularMapId, filename, false);
        SetFilteringTexture(model.specularMapId, TextureFiltering::NEAREST_MIPMAP);
        GenerateTextureMipmaps(model.specularMapId);
        model.hasSpecularMap = true;
    }
}