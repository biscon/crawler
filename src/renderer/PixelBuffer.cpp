//
// Created by bison on 27-10-22.
//

#include <SDL_log.h>
#include <assert.h>
#include <stdexcept>
#include "PixelBuffer.h"

namespace Renderer {

    namespace {
        u32 upperPowerOfTwo(u32 v)
        {
            v--;
            v |= v >> 1;
            v |= v >> 2;
            v |= v >> 4;
            v |= v >> 8;
            v |= v >> 16;
            v++;
            return v;
        }

        inline u32 getPixelWrapAround(PixelBuffer* buf, u32 x, u32 y)
        {
            if(x < 0)
                x += buf->width;
            if(y < 0)
                y += buf->height;
            u32 *data = (u32*) buf->pixels;
            return data[((y % buf->height)*buf->width)+(x % buf->width)];
        }

        void RGBA_SimpleBlit(const PixelBuffer &src, const UIntRect &src_rect,
                             PixelBuffer &dst, const UIntPos &dst_pos)
        {
            u32 height = src_rect.h;
            u32 *src_ptr = (u32*) src.pixels;
            src_ptr += (src_rect.y * src.width) + src_rect.x;
            u32 *dst_ptr = (u32*) dst.pixels;
            dst_ptr += (dst_pos.y * dst.width) + dst_pos.x;
            u32 src_inc = src.width;
            u32 dst_inc = dst.width;
            u32 src_width_bytes = src_rect.w * sizeof(u32);
            //SDL_Log("width = %d, height = %d, pitch = %d", width, height, pitch);
            for(u32 y = 0; y < height; ++y)
            {
                memcpy(dst_ptr, src_ptr, src_width_bytes);
                src_ptr += src_inc;
                dst_ptr += dst_inc;
            }
        }

        void GREYSCALE_SimpleBlit(const PixelBuffer &src, const UIntRect &src_rect,
                                  PixelBuffer &dst, const UIntPos &dst_pos)
        {
            u32 height = src_rect.h;
            u8 *src_ptr = (u8*) src.pixels;
            src_ptr += (src_rect.y * src.width) + src_rect.x;
            u8 *dst_ptr = (u8*) dst.pixels;
            dst_ptr += (dst_pos.y * dst.width) + dst_pos.x;
            u32 src_inc = src.width;
            u32 dst_inc = dst.width;
            u32 src_width_bytes = src_rect.w;
            //SDL_Log("width = %d, height = %d, pitch = %d", width, height, pitch);
            for(u32 y = 0; y < height; ++y)
            {
                memcpy(dst_ptr, src_ptr, src_width_bytes);
                src_ptr += src_inc;
                dst_ptr += dst_inc;
            }
        }
    }

    PixelBuffer::PixelBuffer(u32 width, u32 height, PixelFormat format) : width(width),
        height(height), pixelFormat(format) {
        padding = false;
        if(format == PixelFormat::RGBA)
            pixels = calloc(1, width * height * sizeof(u32));
        if(format == PixelFormat::GREYSCALE)
            pixels = calloc(1, width * height);
    }

    PixelBuffer::PixelBuffer(const std::string filename, bool pad) : padding(pad) {
        unsigned error;
        if(!pad) {
            error = lodepng_decode32_file((u8 **) &pixels, &width, &height, filename.c_str());
            pixelFormat = PixelFormat::RGBA;

            if (error) {
                SDL_Log("error %u: %s\n", error, lodepng_error_text(error));
                throw std::runtime_error("Could not load image " + filename);
            }
        } else {
            PixelBuffer orig(0, 0, PixelFormat::RGBA);
            error = lodepng_decode32_file((u8**) &orig.pixels, &orig.width, &orig.height, filename.c_str());
            if (error) {
                SDL_Log("error %u: %s\n", error, lodepng_error_text(error));
                throw std::runtime_error("Could not load image " + filename);
            }
            width = orig.width+2;
            height = orig.height+2;
            pixelFormat = orig.pixelFormat;
            pixels = calloc(1, width * height * sizeof(u32));

            copyFrom(orig, UIntRect(0, 0, orig.width, orig.height), UIntPos(1,1));
            // blit the four times to duplicate texture edges, this helps avoiding texture bleeding
            // (supersampling fetching undefined texels) when using textureatlas


            // left side
            copyFrom(orig, UIntRect(0, 0, 1, orig.height), UIntPos(0, 1));
            // right side
            copyFrom(orig, UIntRect(orig.width-1, 0, 1, orig.height), UIntPos(width-1, 1));
            // top side
            copyFrom(orig, UIntRect(0, 0, orig.width, 1), UIntPos(1, 0));
            // bottom side
            copyFrom(orig, UIntRect(0, orig.height-1, orig.width, 1), UIntPos(1, height-1));

            // TODO using memcpy for 1 pixel is overkill, replace with direct 4 byte assignment on calculated buffer indexes
            // top left corner
            copyFrom(orig, UIntRect(0, 0, 1, 1), UIntPos(0, 0));
            // top right corner
            copyFrom(orig, UIntRect(orig.width-1, 0, 1, 1), UIntPos(width-1, 0));

            // bottom left corner
            copyFrom(orig, UIntRect(0, orig.height-1, 1, 1), UIntPos(0, height-1));

            // bottom right corner
            copyFrom(orig, UIntRect(orig.width-1, orig.height-1, 1, 1), UIntPos(width-1, height-1));
        }
    }

    PixelBuffer::~PixelBuffer() {
        if(pixels)
            free(pixels);
    }

    void PixelBuffer::copyFrom(const PixelBuffer &src, const UIntRect &src_rect, const UIntPos &dst_pos) {
        assert(src.pixelFormat == pixelFormat);
        if(src.pixelFormat != pixelFormat) {
            SDL_Log("Can't blit between different pixel formats");
            throw std::runtime_error("Cannot blit between different pixel formats");
        }
        switch(src.pixelFormat) {
            case PixelFormat::RGBA: {
                RGBA_SimpleBlit(src, src_rect, *this, dst_pos);
                break;
            }
            case PixelFormat::GREYSCALE: {
                GREYSCALE_SimpleBlit(src, src_rect, *this, dst_pos);
                break;
            }
        }
    }

    void PixelBuffer::upscaleNPOT() {
        u32 nw = upperPowerOfTwo(width);
        u32 nh = upperPowerOfTwo(height);

        PixelBuffer dst(nw, nh, pixelFormat);
        auto rect = UIntRect(0, 0, width, height);
        auto pos = UIntPos(0, 0);
        dst.copyFrom(*this, rect, pos);

        if(pixels) {
            free(pixels);
        }

        width = nw;
        height = nh;
        copyFrom(dst, rect, pos);

        SDL_Log("New dimensions %d,%d", nw, nh);
    }

    bool PixelBuffer::saveToPNG(std::string filename) {
        return false;
    }

    void PixelBuffer::preMultiplyAlpha() {
        u32 *data = (u32*) pixels;
        u32 size = width * height;
        for(u32 i = 0; i < size; ++i) {
            u32 value = data[i];
            u8 alpha = ALPHA(value);
            u8 r = (u8) (RED(value) * alpha / 255);
            u8 g = (u8) (GREEN(value) * alpha / 255);
            u8 b = (u8) (BLUE(value) * alpha / 255);
            data[i] = (u32) ARGB(alpha, r, g, b);
        }
    }

    UIntRect PixelBuffer::getTrimmedSize() {
        u32 left = 0, right = width-1, top = 0, bottom = height-2;

        // right
        for(u32 x = 0; x < width; x++) {
            for(u32 y = 0; y < height; ++y) {
                u8 alpha = ALPHA(getPixelWrapAround(this, x, y));
                if(alpha > 0) {
                    right = x;
                    break;
                }
            }
        }

        // left
        for(i32 x = width-1; x >= 0; --x) {

            for(u32 y = 0; y < height; ++y) {
                u8 alpha = ALPHA(getPixelWrapAround(this, (u32) x, y));
                if(alpha > 0) {
                    left = (u32) x;
                    break;
                }
            }
        }

        // bottom
        for(u32 y = 0; y < height; ++y) {
            for (u32 x = 0; x < width; x++) {
                u8 alpha = ALPHA(getPixelWrapAround(this, x, y));
                if (alpha > 0) {
                    bottom = y;
                    break;
                }
            }
        }

        // top
        for(i32 y = height-1; y >= 0; --y) {
            for (u32 x = 0; x < width; x++) {
                u8 alpha = ALPHA(getPixelWrapAround(this, x, (u32) y));
                if (alpha > 0) {
                    top = (u32) y;
                    break;
                }
            }
        }
        return UIntRect(left-1, top-1, right-left+1, bottom-top+1);
    }

    PixelBuffer::PixelBuffer(const PixelBuffer &other) {
        width = other.width;
        height = other.height;
        pixelFormat = other.pixelFormat;
        padding = other.padding;
        if(other.pixelFormat == PixelFormat::RGBA) {
            pixels = calloc(1, width * height * sizeof(u32));
            memcpy(pixels, other.pixels, width * height * sizeof(u32));
        }
        if(other.pixelFormat == PixelFormat::GREYSCALE) {
            pixels = calloc(1, width * height);
            memcpy(pixels, other.pixels, width * height);
        }
    }

    void PixelBuffer::verticalFlip() {
        u32 *data = (u32*) pixels;
        u32 *temp = (u32*) calloc(1, width * height * sizeof(u32));
        for(u32 y = 0; y < height; ++y) {
            memcpy(&temp[y*width], &data[(height-y-1)*width], width * sizeof(u32));
        }
        memcpy(data, temp, width * height * sizeof(u32));
        free(temp);
    }

}