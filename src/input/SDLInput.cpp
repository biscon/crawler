//
// Created by bison on 10-11-22.
//

#include "SDLInput.h"
#include <SDL_mouse.h>

namespace Input {
    void InitInput() {
        mouseState.x = 0;
        mouseState.y = 0;
        mouseState.lmb = false;
        mouseState.rmb = false;
    }

    void CreateMapping(Mapping &mapping) {
        mappings.emplace_back(mapping);
    }

    void RegisterContext(std::shared_ptr<InputContext> &context) {
        contexts.emplace_back(context);
    }

    static State* findOrCreateState(InputContext& context, Mapping &mapping) {
        for(auto& state : context.states) {
            if(state.id == mapping.mappedId) {
                return &state;
            }
        }
        State state = { .id = mapping.mappedId };
        return &context.states.emplace_back(state);
    }

    static void dispatchActionMapping(Mapping& mapping) {
        for(auto& context : contexts) {
            for(auto& curId : context->actionIds) {
                if(curId == mapping.mappedId) {
                    Action action = { .id = mapping.mappedId };
                    context->actions.emplace_back(action);
                }
            }
        }
    }

    static void dispatchStateMapping(Mapping &mapping, bool active) {
        for(auto& context : contexts) {
            for(auto& curId : context->stateIds) {
                if(curId == mapping.mappedId) {
                    State* state = findOrCreateState(*context, mapping);
                    state->active = active;
                }
            }
        }
    }

    void OnPushKeyUp(SDL_Keycode keycode) {
        for(auto& mapping : mappings) {
            if(mapping.event.type == RawEventType::Keyboard &&
               mapping.event.keycode == keycode &&
               mapping.type == MappingType::State) {
                //SDL_Log("Dispatching state mapping false");
                dispatchStateMapping(mapping, false);
            }
        }
    }

    void OnPushKeyDown(SDL_Keycode keycode) {
        for(auto& mapping : mappings) {
            if(mapping.event.type == RawEventType::Keyboard &&
               mapping.event.keycode == keycode &&
               mapping.type == MappingType::Action) {
                //SDL_Log("Dispatching action mapping");
                dispatchActionMapping(mapping);
            }
            if(mapping.event.type == RawEventType::Keyboard &&
               mapping.event.keycode == keycode &&
               mapping.type == MappingType::State) {
                //SDL_Log("Dispatching state mapping true");
                dispatchStateMapping(mapping, true);
            }
        }
    }

    void OnMouseMotion(float x, float y) {
        mouseState.x = x;
        mouseState.y = y;
    }

    void OnMouseLeftButton(bool pressed) {
        mouseState.lmb = pressed;
        if(pressed) {
            for(auto& mapping : mappings) {
                if(mapping.event.type == RawEventType::Mouse &&
                    mapping.mappedId == INPUT_ACTION_LMB_DOWN &&
                    mapping.type == MappingType::Action) {
                    dispatchActionMapping(mapping);
                }
            }
        } else {
            for(auto& mapping : mappings) {
                if(mapping.event.type == RawEventType::Mouse &&
                   mapping.mappedId == INPUT_ACTION_LMB_UP &&
                   mapping.type == MappingType::Action) {
                    dispatchActionMapping(mapping);
                }
            }
        }
    }

    void OnMouseRightButton(bool pressed) {
        mouseState.rmb = pressed;
        if(pressed) {
            for(auto& mapping : mappings) {
                if(mapping.event.type == RawEventType::Mouse &&
                   mapping.mappedId == INPUT_ACTION_RMB_DOWN &&
                   mapping.type == MappingType::Action) {
                    dispatchActionMapping(mapping);
                }
            }
        } else {
            for(auto& mapping : mappings) {
                if(mapping.event.type == RawEventType::Mouse &&
                   mapping.mappedId == INPUT_ACTION_RMB_UP &&
                   mapping.type == MappingType::Action) {
                    dispatchActionMapping(mapping);
                }
            }
        }
    }

    void ShowMouseCursor(bool show) {
        SDL_ShowCursor(show ? 1 : 0);
    }
}