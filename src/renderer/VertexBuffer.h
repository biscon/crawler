//
// Created by bison on 04-09-23.
//

#ifndef PLATFORMER_VERTEXBUFFER_H
#define PLATFORMER_VERTEXBUFFER_H

#include "defs.h"
#include <vector>

namespace Renderer {
    enum class VertexAttributeType {
        Float,
        Double,
        Int,
    };

    enum class VertexAccessType {
        STATIC,
        DYNAMIC,
    };

    struct VertexAttribute {
        u32 index;
        i32 size;
        VertexAttributeType type;
    };

    class VertexAttributes {
    public:
        VertexAttributes() : stride(0) {}
        void add(u32 index, i32 size, VertexAttributeType type);

    private:
        std::vector<VertexAttribute> attributes;
        i32 stride;

        friend class VertexBuffer;
    };


    class VertexBuffer {
    public:
        explicit VertexBuffer(const VertexAttributes& attributes);
        ~VertexBuffer();
        void bind() const;
        static void unbind();
        void update(void *data, size_t offset, size_t size) const;
        void allocate(void *data, size_t size, VertexAccessType accessType) const;

    private:
        VertexAttributes attributes;
        u32 arrayId = 0;
        u32 bufferId = 0;
    };
}

#endif //PLATFORMER_VERTEXBUFFER_H
