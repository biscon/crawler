//
// Created by bison on 02-10-23.
//

#ifndef DOD_VIEWPORT_H
#define DOD_VIEWPORT_H

#include <SDL_rect.h>
#include "defs.h"

namespace Renderer {
    struct Viewport {
        u32 screenWidth;
        u32 screenHeight;
        SDL_Rect current;
    };

    void InitViewport(u32 width, u32 height);
    void SizeChangedViewport(u32 newWidth, u32 newHeight);
    Viewport& GetViewport();
}
#endif //DOD_VIEWPORT_H
