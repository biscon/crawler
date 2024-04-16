//
// Created by bison on 30-03-24.
//

#ifndef CRAWLER_LIGHTING_H
#define CRAWLER_LIGHTING_H

#include "defs.h"
#include "glm/ext.hpp"
#include "../../renderer/LevelRenderer.h"
#include <vector>

namespace Game {
    void GetLightByLevel(Renderer::Light& l, i32 level);
}
#endif //CRAWLER_LIGHTING_H
