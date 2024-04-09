//
// Created by bison on 10-11-22.
//

#ifndef GAME_SDLINPUTDEVICE_H
#define GAME_SDLINPUTDEVICE_H

#include "Input.h"

namespace Input {
    namespace {
        std::vector<std::shared_ptr<InputContext>> contexts;
        std::vector<Mapping> mappings;
        MouseState mouseState;
    }

    void InitInput();
    void CreateMapping(Mapping& mapping);
    void RegisterContext(std::shared_ptr<InputContext> &context);
    void ShowMouseCursor(bool show);

    void OnPushKeyUp(SDL_Keycode keycode);
    void OnPushKeyDown(SDL_Keycode keycode);
    void OnMouseMotion(float x, float y);
    void OnMouseLeftButton(bool pressed);
    void OnMouseRightButton(bool pressed);
}


#endif //GAME_SDLINPUTDEVICE_H
