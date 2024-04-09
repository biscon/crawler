//
// Created by bison on 10-11-22.
//

#include "Input.h"

namespace Input {
    void InputContext::registerAction(MappedId id) {
        actionIds.emplace_back(id);
    }

    void InputContext::registerState(MappedId id) {
        stateIds.emplace_back(id);

    }

    bool InputContext::pollAction(Action &action) {
        if(actions.empty()) {
            return false;
        }
        Action& src = actions.back();
        actions.pop_back();
        action = src;
        return true;
    }

    bool InputContext::queryState(u32 id) {
        for(auto& state : states) {
            if(state.id == id) {
                return state.active;
            }
        }
        return false;
    }
}