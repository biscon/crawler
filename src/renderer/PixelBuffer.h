//
// Created by bison on 27-10-22.
//

#ifndef GAME_PIXELBUFFER_H
#define GAME_PIXELBUFFER_H

#include <defs.h>
#include <string>
#include "Vector2.h"

#define ALPHA(rgb) (uint8_t)(rgb >> 24)
#define BLUE(rgb)   (uint8_t)(rgb >> 16)
#define GREEN(rgb) (uint8_t)(rgb >> 8)
#define RED(rgb)  (uint8_t)(rgb)
#define ARGB(a, r, g, b) (a << 24) | ((b & 0xFF) << 16) | ((g & 0xFF) << 8) | (r & 0xFF)

extern "C" {
    #include "../util/lodepng.h"
}

namespace Renderer {

    enum class PixelFormat {
        RGBA,
        GREYSCALE
    };

    struct PixelBuffer {
        u32 width;
        u32 height;
        PixelFormat pixelFormat;
        bool padding;
        void* pixels;

        PixelBuffer(u32 width, u32 height, PixelFormat format);
        PixelBuffer(std::string filename, bool pad);
        ~PixelBuffer();

        PixelBuffer(const PixelBuffer& other);

        void copyFrom(const PixelBuffer& src, const UIntRect& src_rect, const UIntPos& dst_pos);
        void upscaleNPOT();
        bool saveToPNG(std::string filename);
        void preMultiplyAlpha();
        void verticalFlip();
        UIntRect getTrimmedSize();
    };

}


#endif //GAME_PIXELBUFFER_H
