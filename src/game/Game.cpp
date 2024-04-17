//
// Created by bison on 4/4/19.
//

#include <defs.h>
#include <SDL_log.h>
#include <SDL_mouse.h>
#include "Game.h"
#include "../input/SDLInput.h"
#include "imgui.h"
#include "../util/string_util.h"

namespace Game {

    const u8 testMap3[] = {
            '#','#','#','#','#','#','#','#','#',
            '#','L',' ','#','L','#',' ','M','#',
            '#',' ',' ',' ',' ',' ',' ',' ','#',
            '#',' ','M',' ','L',' ','M',' ','#',
            '#','#',' ',' ','M',' ',' ',' ','#',
            '#','M',' ',' ',' ',' ',' ',' ','#',
            '#',' ',' ','M','P',' ',' ',' ','#',
            '#','L',' ',' ','L',' ','M','L','#',
            '#','#','#','#','#','#','#','#','#',
    };

    const u8 testMap1[] = {
            '#','#','#','#','#','#','#','#','#',
            '#','3',' ',' ',' ',' ',' ','2','#',
            '#',' ',' ','#',' ','#',' ',' ','#',
            '#','Z',' ',' ',' ',' ','Z',' ','#',
            '#',' ',' ','#',' ','#',' ','#','#',
            '#',' ',' ',' ',' ','Z',' ','Z','#',
            '#','#','D','#',' ',' ',' ','Z','#',
            '#','P',' ','d','4',' ',' ',' ','#',
            '#','#','#','#','#','#','#','#','#',
    };

    const u8 testMap2[] = {
            '#','#','#','#','#','#','#','#','#','#',
            '#',' ','P','#','M','#',' ',' ',' ','#',
            '#','D','#','#',' ','#',' ','#',' ','#',
            '#',' ','4',' ',' ',' ',' ','#',' ','#',
            '#','#','#','#','#','#','#','#',' ','#',
            '#',' ',' ',' ',' ',' ',' ','#',' ','#',
            '#','4','#',' ','#','#',' ','#',' ','#',
            '#','Z',' ',' ','W','#',' ','d','4','#',
            '#','#','#',' ','#','#','#','#','#','#',
            '#',' ','#','D','#',' ',' ',' ',' ','#',
            '#',' ',' ',' ',' ',' ',' ',' ',' ','#',
            '#',' ',' ',' ',' ',' ',' ',' ',' ','#',
            '#',' ',' ',' ',' ',' ',' ','Z',' ','#',
            '#',' ','M','#',' ',' ','#','Z',' ','#',
            '#',' ',' ',' ',' ',' ',' ',' ',' ','#',
            '#',' ',' ','Z',' ','7',' ',' ',' ','#',
            '#',' ',' ',' ',' ',' ',' ',' ',' ','#',
            '#','#','#','#','#','#','#','#','#','#',
    };

    static void createMapping(Input::MappingType type, Input::MappedId mappedId, Input::RawEventType rawEventType, SDL_Keycode keycode) {
        Input::Mapping mapping{};
        mapping.type = type;
        mapping.mappedId = mappedId;
        mapping.event.type = rawEventType;
        mapping.event.keycode = keycode;
        Input::CreateMapping(mapping);
    }
    
    static void setupInputMappings() {
        createMapping(Input::MappingType::Action, INPUT_ACTION_ENTER, Input::RawEventType::Keyboard, SDLK_RETURN);
        createMapping(Input::MappingType::Action, INPUT_ACTION_ESCAPE, Input::RawEventType::Keyboard, SDLK_ESCAPE);

        createMapping(Input::MappingType::State, INPUT_STATE_FORWARD, Input::RawEventType::Keyboard, SDLK_w);
        createMapping(Input::MappingType::State, INPUT_STATE_BACK, Input::RawEventType::Keyboard, SDLK_s);
        createMapping(Input::MappingType::State, INPUT_STATE_LEFT, Input::RawEventType::Keyboard, SDLK_a);
        createMapping(Input::MappingType::State, INPUT_STATE_RIGHT, Input::RawEventType::Keyboard, SDLK_d);

        createMapping(Input::MappingType::Action, INPUT_ACTION_SPACE, Input::RawEventType::Keyboard, SDLK_SPACE);
        createMapping(Input::MappingType::Action, INPUT_ACTION_FORWARD, Input::RawEventType::Keyboard, SDLK_w);
        createMapping(Input::MappingType::Action, INPUT_ACTION_BACK, Input::RawEventType::Keyboard, SDLK_s);
        createMapping(Input::MappingType::Action, INPUT_ACTION_LEFT, Input::RawEventType::Keyboard, SDLK_a);
        createMapping(Input::MappingType::Action, INPUT_ACTION_RIGHT, Input::RawEventType::Keyboard, SDLK_d);
        createMapping(Input::MappingType::Action, INPUT_ACTION_TURN_LEFT, Input::RawEventType::Keyboard, SDLK_q);
        createMapping(Input::MappingType::Action, INPUT_ACTION_TURN_RIGHT, Input::RawEventType::Keyboard, SDLK_e);
        createMapping(Input::MappingType::Action, INPUT_ACTION_TOGGLE_FPS, Input::RawEventType::Keyboard, SDLK_F1);
        createMapping(Input::MappingType::Action, INPUT_ACTION_TOGGLE_FREECAM, Input::RawEventType::Keyboard, SDLK_F4);
    }

    static void setupInputContext(Game& game) {
        game.inputContext = std::make_shared<Input::InputContext>();
        Input::RegisterContext(game.inputContext);
        game.inputContext->registerAction(INPUT_ACTION_TOGGLE_FPS);
        game.inputContext->registerAction(INPUT_ACTION_TOGGLE_EDIT);
        game.inputContext->registerAction(INPUT_ACTION_ESCAPE);
        game.inputContext->registerAction(INPUT_ACTION_SAVE);
        game.inputContext->registerAction(INPUT_ACTION_LOAD);
        game.inputContext->registerAction(INPUT_ACTION_FORWARD);
        game.inputContext->registerAction(INPUT_ACTION_BACK);
        game.inputContext->registerAction(INPUT_ACTION_LEFT);
        game.inputContext->registerAction(INPUT_ACTION_RIGHT);
        game.inputContext->registerAction(INPUT_ACTION_TURN_LEFT);
        game.inputContext->registerAction(INPUT_ACTION_TURN_RIGHT);
        game.inputContext->registerAction(INPUT_ACTION_TOGGLE_FREECAM);
        game.inputContext->registerAction(INPUT_ACTION_SPACE);

        game.inputContext->registerState(INPUT_STATE_FORWARD);
        game.inputContext->registerState(INPUT_STATE_BACK);
        game.inputContext->registerState(INPUT_STATE_LEFT);
        game.inputContext->registerState(INPUT_STATE_RIGHT);
    }

    static void handleInput(Game& game, float frameDelta) {
        Input::Action action{};
        while(game.inputContext->pollAction(action)) {
            switch(action.id) {
                case INPUT_ACTION_FORWARD:
                    if(!game.level.freeCam) MoveForward(game.level, *game.levelRenderer.camera);
                    break;
                case INPUT_ACTION_BACK:
                    if(!game.level.freeCam) MoveBackward(game.level, *game.levelRenderer.camera);
                    break;
                case INPUT_ACTION_LEFT:
                    if(!game.level.freeCam) MoveLeft(game.level, *game.levelRenderer.camera);
                    break;
                case INPUT_ACTION_RIGHT:
                    if(!game.level.freeCam) MoveRight(game.level, *game.levelRenderer.camera);
                    break;
                case INPUT_ACTION_TURN_LEFT:
                    if(!game.level.freeCam) TurnLeft(game.level, *game.levelRenderer.camera);
                    break;
                case INPUT_ACTION_TURN_RIGHT:
                    if(!game.level.freeCam) TurnRight(game.level, *game.levelRenderer.camera);
                    break;
                case INPUT_ACTION_TOGGLE_FPS:
                    game.debugFlag = !game.debugFlag;
                    break;
                case INPUT_ACTION_TOGGLE_FREECAM:
                    game.level.freeCam = !game.level.freeCam;
                    break;
                case INPUT_ACTION_SPACE:
                    OpenDoor(game.level);
                    break;
                case INPUT_ACTION_ESCAPE:
                    game.quitFlag = true;
                    break;
                case INPUT_ACTION_SAVE:
                    break;
                case INPUT_ACTION_LOAD:
                    break;
                default:
                    break;
            }
        }
        if(game.level.freeCam) {
            if(game.inputContext->queryState(INPUT_STATE_FORWARD)) {
                game.levelRenderer.camera->ProcessKeyboard(Camera_Movement::FORWARD, frameDelta);
            }
            if(game.inputContext->queryState(INPUT_STATE_BACK)) {
                game.levelRenderer.camera->ProcessKeyboard(Camera_Movement::BACKWARD, frameDelta);
            }
            if(game.inputContext->queryState(INPUT_STATE_LEFT)) {
                game.levelRenderer.camera->ProcessKeyboard(Camera_Movement::LEFT, frameDelta);
            }
            if(game.inputContext->queryState(INPUT_STATE_RIGHT)) {
                game.levelRenderer.camera->ProcessKeyboard(Camera_Movement::RIGHT, frameDelta);
            }
        }
    }

    static void handleMouseInput(Game& game) {
        if(game.level.freeCam) {
            int mouseX, mouseY;
            auto state = SDL_GetRelativeMouseState(&mouseX, &mouseY);
            if(state & SDL_BUTTON(SDL_BUTTON_RIGHT)) {
                game.levelRenderer.camera->ProcessMouseMovement((float) mouseX, (float) -mouseY);
            }
        }
    }

    static void createTestMap1(Game &game) {
        u32 torchModel = LoadModel(
                game.levelRenderer,
                "assets/models/torch.obj",
                "assets/models/torch.png",
                "assets/models/torch_n.png",
                "assets/models/torch_s.png"
        );
        u32 barrelModel = LoadModel(
            game.levelRenderer,
            "assets/models/big_barrel.obj",
            "assets/models/big_barrel.png",
            "assets/models/big_barrel_n.png",
            "assets/models/big_barrel_s.png"
        );
        u32 benchModel = LoadModel(
                game.levelRenderer,
                "assets/models/bench.obj",
                "assets/models/bench.png",
                "assets/models/bench_n.png",
                "assets/models/bench_s.png"
        );
        LoadLevel(game.level, game.levelRenderer, testMap1, 9, 9);

        CreateModelInstance(game.level, 1, 1, CubeSide::WEST, 1.0f, torchModel);
        CreateModelInstance(game.level, 1, 1, CubeSide::NORTH, 1.0f, torchModel);
        CreateModelInstance(game.level, 1, 2, CubeSide::WEST, 1.0f, torchModel);

        CreateModelInstance(game.level, 1, 7, CubeSide::WEST, 1.0f, torchModel);
        CreateModelInstance(game.level, 1, 7, CubeSide::NORTH, 1.0f, torchModel);
        CreateModelInstance(game.level, 1, 7, CubeSide::SOUTH, 1.0f, torchModel);

        CreateModelInstance(game.level, 7, 7, CubeSide::EAST, 1.0f, torchModel);
        CreateModelInstance(game.level, 4, 6, CubeSide::WEST, 1.0f, torchModel);

        CreateModelInstance(game.level, 1, 2, CubeSide::BOTTOM, 1.0f, barrelModel);
        CreateModelInstance(game.level, 6, 6, CubeSide::BOTTOM, 1.0f, barrelModel);
        CreateModelInstance(game.level, 5, 4, CubeSide::BOTTOM, 1.0f, barrelModel);
        CreateModelInstance(game.level, 4, 3, CubeSide::BOTTOM, 1.0f, barrelModel);
        CreateModelInstance(game.level, 1, 5, CubeSide::BOTTOM, 1.0f, barrelModel);
        CreateModelInstance(game.level, 7, 3, CubeSide::BOTTOM, 1.0f, barrelModel);

        CreateModelInstance(game.level, 3, 1, CubeSide::NORTH, 1.0f, benchModel);
        CreateModelInstance(game.level, 2, 1, CubeSide::NORTH, 1.0f, torchModel);
        CreateModelInstance(game.level, 4, 1, CubeSide::NORTH, 1.0f, torchModel);
        CreateModelInstance(game.level, 5, 1, CubeSide::NORTH, 1.0f, benchModel);
        CreateModelInstance(game.level, 7, 1, CubeSide::EAST, 1.0f, benchModel);
        CreateModelInstance(game.level, 7, 5, CubeSide::EAST, 1.0f, benchModel);
        CreateModelInstance(game.level, 7, 6, CubeSide::EAST, 1.0f, benchModel);
    }

    static void createTestMap2(Game &game) {
        u32 torchModel = LoadModel(
                game.levelRenderer,
                "assets/models/torch.obj",
                "assets/models/torch.png",
                "assets/models/torch_n.png",
                "assets/models/torch_s.png"
        );
        u32 barrelModel = LoadModel(
                game.levelRenderer,
                "assets/models/big_barrel.obj",
                "assets/models/big_barrel.png",
                "assets/models/big_barrel_n.png",
                "assets/models/big_barrel_s.png"
        );
        u32 benchModel = LoadModel(
                game.levelRenderer,
                "assets/models/bench.obj",
                "assets/models/bench.png",
                "assets/models/bench_n.png",
                "assets/models/bench_s.png"
        );
        LoadLevel(game.level, game.levelRenderer, testMap2, 10, 18);
        CreateModelInstance(game.level, 1, 1, CubeSide::WEST, 1.0f, torchModel);
        CreateModelInstance(game.level, 1, 3, CubeSide::WEST, 1.0f, benchModel);

        CreateModelInstance(game.level, 2, 3, CubeSide::SOUTH, 1.0f, torchModel);
        CreateModelInstance(game.level, 4, 2, CubeSide::EAST, 1.0f, benchModel);
        CreateModelInstance(game.level, 4, 2, CubeSide::WEST, 1.0f, benchModel);

        CreateModelInstance(game.level, 6, 2, CubeSide::EAST, 1.0f, benchModel);
        CreateModelInstance(game.level, 6, 3, CubeSide::BOTTOM, 1.0f, barrelModel);

        CreateModelInstance(game.level, 8, 3, CubeSide::WEST, 1.0f, benchModel);
        CreateModelInstance(game.level, 8, 5, CubeSide::WEST, 1.0f, benchModel);

        CreateModelInstance(game.level, 8, 7, CubeSide::SOUTH, 1.0f, torchModel);

        CreateModelInstance(game.level, 2, 16, CubeSide::SOUTH, 1.0f, benchModel);
        CreateModelInstance(game.level, 3, 16, CubeSide::BOTTOM, 1.0f, barrelModel);
        CreateModelInstance(game.level, 3, 14, CubeSide::BOTTOM, 1.0f, barrelModel);
        CreateModelInstance(game.level, 6, 14, CubeSide::BOTTOM, 1.0f, barrelModel);
        CreateModelInstance(game.level, 4, 16, CubeSide::SOUTH, 1.0f, benchModel);
        CreateModelInstance(game.level, 5, 16, CubeSide::SOUTH, 1.0f, benchModel);
        CreateModelInstance(game.level, 6, 16, CubeSide::SOUTH, 1.0f, benchModel);
        CreateModelInstance(game.level, 7, 17, CubeSide::BOTTOM, 1.0f, barrelModel);
        CreateModelInstance(game.level, 8, 15, CubeSide::EAST, 1.0f, benchModel);
        CreateModelInstance(game.level, 8, 11, CubeSide::EAST, 1.0f, benchModel);
        CreateModelInstance(game.level, 8, 13, CubeSide::BOTTOM, 1.0f, barrelModel);
        CreateModelInstance(game.level, 4, 13, CubeSide::WEST, 1.0f, benchModel);
        CreateModelInstance(game.level, 5, 13, CubeSide::EAST, 1.0f, benchModel);
        CreateModelInstance(game.level, 4, 13, CubeSide::BOTTOM, 1.0f, barrelModel);
        CreateModelInstance(game.level, 5, 10, CubeSide::BOTTOM, 1.0f, barrelModel);
        CreateModelInstance(game.level, 4, 10, CubeSide::NORTH, 1.0f, benchModel);
        CreateModelInstance(game.level, 2, 10, CubeSide::NORTH, 1.0f, benchModel);
    }

    void InitGame(Game& game) {
        setupInputMappings();
        setupInputContext(game);
        CreateFont(game.font, "assets/fonts/OpenSans-Semibold.ttf", 32);
        game.quitFlag = false;
        game.debugFlag = false;

        /*
        FloatRect widthInsets(0.25f, 0, 0.25f, 0);
        game.animId = CreateAnimation(game.animation, 32, 48, 10, "assets/sprites/player_walk.png", &widthInsets);
        LoadAnimations(game.animation);
        game.controllerId = PlayAnimation(game.animation, game.animId, RepeatType::Restart, false);
        PlayAnimation(game.animation, game.animId, RepeatType::Once, false);
        */
        //game.levelRenderer = std::make_unique<Renderer::LevelRenderer>();
        InitLevelRenderer(game.levelRenderer);


        CreateMonsterBluePrint(game.level, 'M', 'N', "assets/goblin", 50, 64, 2.5f);
        CreateMonsterBluePrint(game.level, 'Z', 'N', "assets/zombie", 59, 95, 2.25f);
        CreateMonsterBluePrint(game.level, 'W', 'N', "assets/dog", 96, 74, 2.0f);
        CreateObjectBluePrint(game.level, 'S', '*', "assets/skeleton", 64, 22, 1.0f);
        CreateObjectBluePrint(game.level, 'I', '*', "assets/pillar", 50, 128, 3.0f);

        createTestMap2(game);
    }

    static void showDebugOverlay(Game& game, bool* p_open, float frameDelta) {
        float fps = 1.0f / frameDelta;
        static int location = 0;
        ImGuiIO& io = ImGui::GetIO();
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
        if (location >= 0)
        {
            const float PAD = 10.0f;
            const ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImVec2 work_pos = viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
            ImVec2 work_size = viewport->WorkSize;
            ImVec2 window_pos, window_pos_pivot;
            window_pos.x = (location & 1) ? (work_pos.x + work_size.x - PAD) : (work_pos.x + PAD);
            window_pos.y = (location & 2) ? (work_pos.y + work_size.y - PAD) : (work_pos.y + PAD);
            window_pos_pivot.x = (location & 1) ? 1.0f : 0.0f;
            window_pos_pivot.y = (location & 2) ? 1.0f : 0.0f;
            ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
            window_flags |= ImGuiWindowFlags_NoMove;
        }
        else if (location == -2)
        {
            // Center window
            ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
            window_flags |= ImGuiWindowFlags_NoMove;
        }
        ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background
        if (ImGui::Begin("DebugWindow", p_open, window_flags))
        {
            ImGui::Text("FPS: %.2f\nCells rendered: %d\nCamera pos: %.2f,%.2f,%.2f\n",
                        fps,
                        game.levelRenderer.cellsRendered,
                        game.levelRenderer.camera->Position.x,
                        game.levelRenderer.camera->Position.y,
                        game.levelRenderer.camera->Position.z
            );

            ImGui::Separator();
            if (ImGui::IsMousePosValid())
                ImGui::Text("Mouse Position: (%.1f,%.1f)", io.MousePos.x, io.MousePos.y);
            else
                ImGui::Text("Mouse Position: <invalid>");
            if (ImGui::BeginPopupContextWindow())
            {
                if (ImGui::MenuItem("Top-left",     NULL, location == 0)) location = 0;
                if (ImGui::MenuItem("Top-right",    NULL, location == 1)) location = 1;
                if (ImGui::MenuItem("Bottom-left",  NULL, location == 2)) location = 2;
                if (ImGui::MenuItem("Bottom-right", NULL, location == 3)) location = 3;
                if (p_open && ImGui::MenuItem("Close")) *p_open = false;
                ImGui::EndPopup();
            }
        }
        ImGui::End();
    }

    void UpdateGame(Game& game, float frameDelta) {
        handleMouseInput(game);
        handleInput(game, frameDelta);
        UpdateLevel(game.level, game.levelRenderer, frameDelta);
        UpdateLevelRenderer(game.levelRenderer, frameDelta);
        RenderLevel(game.levelRenderer, frameDelta);
        if(game.debugFlag) {
            showDebugOverlay(game, &game.debugFlag, frameDelta);
        }
    }
    
    void ShutdownGame(Game& game) {
        ShutdownLevel(game.level);
        ShutdownLevelRenderer(game.levelRenderer);
        /*
        ShutdownAnimations(game.animation);
        ShutdownRenderer(*game.renderer);
        */
        DestroyFont(game.font);
    }
}

