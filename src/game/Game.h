//
// Created by bison on 4/4/19.
//

#ifndef GAME_H
#define GAME_H

#include <memory>
#include "glm/ext.hpp"
#include "../input/Input.h"
#include "../renderer/Renderer.h"
#include "animation/Animations.h"
#include "../renderer/LevelRenderer.h"
#include "level/Level.h"

namespace Game {
    struct Game {
        std::shared_ptr<Input::InputContext> inputContext;
        Renderer::LevelRenderer levelRenderer;
        Level level;
        Renderer::Font font;
        bool quitFlag;
        bool debugFlag;

        /*
        u32 animId;
        u32 controllerId;
        Animations animation;
        */
    };

    void InitGame(Game& game);
    void UpdateGame(Game& game, float frameDelta);
    void ShutdownGame(Game& game);
}

#endif
