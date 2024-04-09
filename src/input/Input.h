//
// Created by bison on 10-11-22.
//

#ifndef GAME_INPUT_H
#define GAME_INPUT_H

#include <defs.h>
#include <SDL_keycode.h>
#include "mapped_input.h"
#include <vector>
#include <memory>

namespace Input {
    typedef u32 MappedId;

    enum class RawEventType {
        Keyboard,
        Mouse,
        Controller,
        Touch
    };

    enum class MappingType {
        Action,
        State
    };

    struct RawInputEvent {
        RawEventType               type;
        SDL_Keycode                keycode;
    };

    struct Action {
        MappedId                   id;
    };

    struct State {
        MappedId                   id;
        bool                            active;
    };

    struct Mapping {
        MappingType                type;
        MappedId                   mappedId;
        RawInputEvent              event;
    };

    struct MouseState {
        float x;
        float y;
        bool lmb;
        bool rmb;
    };

    struct InputContext {
        void registerAction(MappedId id);
        void registerState(MappedId id);
        bool pollAction(Action& action);
        bool queryState(u32 id);

        std::vector<MappedId> actionIds;
        std::vector<MappedId> stateIds;
        std::vector<Action> actions;
        std::vector<State> states;
    };
}

#endif //GAME_INPUT_H
