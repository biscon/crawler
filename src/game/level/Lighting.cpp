//
// Created by bison on 30-03-24.
//

#include <algorithm>
#include "Lighting.h"

namespace Game {
    // 1.0	0.7	1.8
    // 1.0	0.35	0.44
    // 1.0	0.22	0.20
    // 1.0	0.14	0.07
    // 1.0	0.09	0.032
    // 1.0	0.07	0.017
    void GetLightByLevel(Renderer::Light& l, i32 level) {
        auto whiteColor = glm::vec3(1.0f, 0.95f, 0.90f);
        auto warmWhite = glm::vec3(1.0f, 0.9f, 0.8f);
        auto white = glm::vec3(1.0f, 1.0f, 1.0f);
        auto torchColor = glm::vec3(1.0f, 222.0f/255.0f, 156.0f/255.0f);
        auto ambientColor = glm::vec3(0.01f, 0.01f, 0.01f);
        switch(level) {
            case 1:
                l.ambient = ambientColor;
                l.diffuse = warmWhite;
                l.specular = warmWhite;
                l.linear = 0.7f;
                l.quadratic = 1.8f;
                break;
            case 2:
                l.ambient = ambientColor;
                l.diffuse = warmWhite;
                l.specular = warmWhite;
                l.linear = 0.35f;
                l.quadratic = 0.44f;
                break;
            case 3:
                l.ambient = ambientColor;
                l.diffuse = warmWhite;
                l.specular = warmWhite;
                l.linear = 0.22f;
                l.quadratic = 0.20f;
                break;
            case 4:
                l.ambient = ambientColor;
                l.diffuse = torchColor;
                l.specular = white;
                l.linear = 0.14f;
                l.quadratic = 0.07f;
                break;
            case 5:
                l.ambient = ambientColor;
                l.diffuse = warmWhite;
                l.specular = warmWhite;
                l.linear = 0.09f;
                l.quadratic = 0.032f;
                break;
            case 6:
                l.ambient = ambientColor;
                l.diffuse = warmWhite;
                l.specular = warmWhite;
                l.linear = 0.07f;
                l.quadratic = 0.017f;
                break;
            case 7:
                l.ambient = ambientColor;
                l.diffuse = torchColor;
                l.specular = white;
                l.linear = 0.045f;
                l.quadratic = 0.0075f;
                break;
            case 8:
                l.ambient = ambientColor;
                l.diffuse = warmWhite;
                l.specular = warmWhite;
                l.linear = 0.027f;
                l.quadratic = 0.0028f;
                break;
            case 9:
                l.ambient = ambientColor;
                l.diffuse = warmWhite;
                l.specular = warmWhite;
                l.linear = 0.022f;
                l.quadratic = 0.0019f;
                break;
            case 0:
                l.ambient = ambientColor;
                l.diffuse = warmWhite;
                l.specular = warmWhite;
                l.linear = 0.0014f;
                l.quadratic = 0.0007f;
                break;
            default:
                l.ambient = ambientColor;
                l.diffuse = warmWhite;
                l.specular = warmWhite;
                l.linear = 0.7f;
                l.quadratic = 1.8f;
                break;
        }
    }
}