//
// Created by bison on 04-09-23.
//

#include <stdexcept>
#include "VertexBuffer.h"

extern "C" {
#include "glad.h"
}

namespace Renderer {
    static i32 getAttributeSize(VertexAttributeType type) {
        switch(type) {
            case VertexAttributeType::Float:
                return sizeof(float);
            case VertexAttributeType::Double:
                return sizeof(double);
            case VertexAttributeType::Int:
                return sizeof(int);
            default:
                throw std::runtime_error("Unknown vertex attribute type");
        }
    }

    static GLenum getAttributeGLType(VertexAttributeType type) {
        switch(type) {
            case VertexAttributeType::Float:
                return GL_FLOAT;
            case VertexAttributeType::Double:
                return GL_DOUBLE;
            case VertexAttributeType::Int:
                return GL_INT;
            default:
                throw std::runtime_error("Unknown vertex attribute type");
        }
    }

    void VertexAttributes::add(uint32_t index, int32_t size, Renderer::VertexAttributeType type) {
        VertexAttribute attr = {.index = index, .size = size, .type = type};
        attributes.push_back(attr);
        stride += size * getAttributeSize(type);
    }

    VertexBuffer::VertexBuffer(const VertexAttributes& attributes) : attributes(attributes) {
        glGenVertexArrays(1, &arrayId);
        glGenBuffers(1, &bufferId);
        bind();
        glBufferData(GL_ARRAY_BUFFER, attributes.stride, nullptr, GL_DYNAMIC_DRAW);
        size_t offset = 0;
        for(auto& attr : attributes.attributes) {
            glEnableVertexAttribArray(attr.index);
            glVertexAttribPointer(attr.index, attr.size, getAttributeGLType(attr.type), GL_FALSE, attributes.stride, (void*) offset);
            offset += attr.size * getAttributeSize(attr.type);
        }
        unbind();
    }

    VertexBuffer::~VertexBuffer() {
        glDeleteVertexArrays(1, &arrayId);
        glDeleteBuffers(1, &bufferId);
    }

    void VertexBuffer::bind() const {
        glBindVertexArray(arrayId);
        glBindBuffer(GL_ARRAY_BUFFER, bufferId);
    }

    void VertexBuffer::unbind() {
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void VertexBuffer::update(void *data, size_t offset, size_t size) const {
        bind();
        glBufferSubData(GL_ARRAY_BUFFER, (GLintptr) offset, (GLsizeiptr) size, data);
    }

    void VertexBuffer::allocate(void *data, size_t size, VertexAccessType accessType) const {
        bind();
        glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr) size, data, accessType == VertexAccessType::STATIC ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
    }

}

