//
// Created by bison on 02-10-23.
//

extern "C" {
#include <glad.h>
}

#include "Viewport.h"

namespace Renderer {
    static Viewport viewport = {};

    void InitViewport(u32 width, u32 height) {
        viewport.screenWidth = width;
        viewport.screenHeight = height;
        viewport.current.x = 0;
        viewport.current.y = 0;
        viewport.current.w = (i32) width;
        viewport.current.h = (i32) height;
    }

    void SizeChangedViewport(u32 newWidth, u32 newHeight) {
        u32 realWidth = newWidth;
        u32 realHeight = newHeight;

        float want_aspect;
        float real_aspect;
        float scale;

        want_aspect = (float) viewport.screenWidth / (float) viewport.screenHeight;
        real_aspect = (float) newWidth / (float) newHeight;


        if (SDL_fabs(want_aspect - real_aspect) < 0.0001) {
            /* The aspect ratios are the same, just scale appropriately */
            viewport.current.x = 0;
            viewport.current.y = 0;
            viewport.current.w = (i32) realWidth;
            viewport.current.h = (i32) realHeight;
            glViewport(viewport.current.x, viewport.current.y, viewport.current.w, viewport.current.h);
            //SDL_Log("Same aspect ratio");
        } else if (want_aspect > real_aspect) {
            /* We want a wider aspect ratio than is available - letterbox it */
            scale = (float) newWidth / (float) viewport.screenWidth;
            viewport.current.x = 0;
            viewport.current.w = (i32) newWidth;
            viewport.current.h = (i32) SDL_ceil((float) viewport.screenHeight * scale);
            viewport.current.y = ((i32) newHeight - viewport.current.h) / 2;
            glViewport(viewport.current.x, viewport.current.y, viewport.current.w, viewport.current.h);
            //SDL_Log("letterbox");
        } else {
            /* We want a narrower aspect ratio than is available - use sidebars */
            scale = (float) newHeight / (float) viewport.screenHeight;
            viewport.current.y = 0;
            viewport.current.h = (i32) newHeight;
            viewport.current.w = (i32) SDL_ceil((float) viewport.screenWidth * scale);
            viewport.current.x = ((i32) newWidth - viewport.current.w) / 2;
            glViewport(viewport.current.x, viewport.current.y, viewport.current.w, viewport.current.h);
            //SDL_Log("sidebars");
        }
    }

    void SetViewport() {
        glViewport(viewport.current.x, viewport.current.y, viewport.current.w, viewport.current.h);
    }

    Viewport &GetViewport() {
        return viewport;
    }
}