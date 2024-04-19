//
// Created by bison on 28-01-2018.
//

#include <SDL_log.h>
#include <memory>
#include "ArrayTexture.h"
extern "C" {
#include "glad.h"
}
//glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA8, width, height, layers, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
namespace Renderer {
    void ArrayTexture::create(i32 width, i32 height, i32 layers) {
        glGenTextures(1, &tex);
        bind();

        this->width = width;
        this->height = height;
        this->layers = layers;

        // Allocate storage for the texture
        glTexStorage3D(GL_TEXTURE_2D_ARRAY, 5, GL_RGBA8, width, height, layers);
        //glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA8, width, height, layers, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

        SDL_Log("Allocating array texture %dx%d with %d layers", width, height, layers);
    }

    void ArrayTexture::bind() {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D_ARRAY, tex);
    }

    void ArrayTexture::bind(u32 tex_unit) {
        glActiveTexture(tex_unit);
        glBindTexture(GL_TEXTURE_2D_ARRAY, tex);
    }

    void ArrayTexture::uploadLayer(const std::string& filename, i32 layer) {
        auto pb = std::make_unique<PixelBuffer>(filename.c_str(), false);
        //pb->verticalFlip();
        uploadLayer(*pb, layer);
    }

    void ArrayTexture::uploadLayer(PixelBuffer& pb, i32 layer) {
        // Upload texture data for a specific layer
        bind();
        glTexSubImage3D(GL_TEXTURE_2D_ARRAY,
                        0,                       // Mipmap number
                        0, 0, layer,            // xoffset, yoffset, zoffset
                        width, height, 1,       // width, height, depth
                        GL_RGBA,                // format
                        GL_UNSIGNED_BYTE,       // type
                        (void *) pb.pixels);   // pointer to data
    }

    static void setAnisotrophy() {
        float aniso = 16.0f;
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &aniso);
        //aniso = 8.0f;
        glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_ANISOTROPY, aniso);
    }

    void ArrayTexture::setFilteringTexture(TextureFiltering filtering) {
        bind();
        switch(filtering) {
            case TextureFiltering::NEAREST:
                glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                break;
            case TextureFiltering::LINEAR:
                glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                break;
            case TextureFiltering::NEAREST_MIPMAP:
                setAnisotrophy();
                glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
                glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                break;
            case TextureFiltering::LINEAR_MIPMAP:
                setAnisotrophy();
                glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

                break;
        }
    }

    void ArrayTexture::generateMipmaps() {
        bind();
        glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
    }
}