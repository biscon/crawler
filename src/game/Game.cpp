//
// Created by bison on 4/4/19.
//

#include <defs.h>
#include <SDL_log.h>
#include <SDL_mouse.h>
#include "Game.h"
#include "../input/SDLInput.h"

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
            '#',' ','L',' ','L',' ',' ',' ','#',
            '#',' ','Z',' ',' ',' ',' ',' ','#',
            '#','W',' ','M',' ','Z',' ',' ','#',
            '#',' ',' ','S','L',' ','Z',' ','#',
            '#',' ',' ',' ',' ','Z',' ','S','#',
            '#','#','D','#',' ',' ',' ',' ','#',
            '#','P','L','d',' ',' ',' ',' ','#',
            '#','#','#','#','#','#','#','#','#',
    };

    const u8 testMap2[] = {
            '#','#','#','#','#','#','#','#','#','#',
            '#','L','P','#','M','#','S','Z',' ','#',
            '#','M','#','#',' ','#',' ','#','S','#',
            '#','S',' ','Z',' ',' ','M','#','L','#',
            '#','#','#','#','#','#','#','#',' ','#',
            '#','S',' ',' ',' ','S','L','#','M','D',
            '#',' ','#','S','#','#',' ','#',' ','#',
            '#','Z',' ','L','W','#','W',' ','Z','#',
            '#','#','#',' ','#','#','#','#','#','#',
            '#',' ','#',' ','#',' ',' ',' ',' ','#',
            '#',' ','Z',' ','L','M',' ','S',' ','#',
            '#',' ',' ','I',' ',' ','Z',' ',' ','#',
            '#','W',' ','M',' ','W','I','M',' ','#',
            '#',' ',' ','S','L',' ','I',' ',' ','#',
            '#',' ','Z',' ','Z','Z',' ','S',' ','#',
            '#',' ','I',' ','Z','Z','W',' ',' ','#',
            '#','L',' ','S','L',' ',' ','L',' ','#',
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

    void InitGame(Game& game) {
        setupInputMappings();
        setupInputContext(game);
        CreateFont(game.font, "assets/fonts/OpenSans-Semibold.ttf", 32);
        game.quitFlag = false;

        /*
        FloatRect widthInsets(0.25f, 0, 0.25f, 0);
        game.animId = CreateAnimation(game.animation, 32, 48, 10, "assets/sprites/player_walk.png", &widthInsets);
        LoadAnimations(game.animation);
        game.controllerId = PlayAnimation(game.animation, game.animId, RepeatType::Restart, false);
        PlayAnimation(game.animation, game.animId, RepeatType::Once, false);
        */
        //game.levelRenderer = std::make_unique<Renderer::LevelRenderer>();
        InitLevelRenderer(game.levelRenderer);
        u32 torchModel = LoadModel(game.levelRenderer, "assets/models/torch2.obj", "assets/models/torch2.png");
        u32 barrelModel = LoadModel(game.levelRenderer, "assets/models/barrel.obj", "assets/models/barrel.png");
        u32 benchModel = LoadModel(game.levelRenderer, "assets/models/bench.obj", "assets/models/bench.png");

        CreateMonsterBluePrint(game.level, 'M', 'N', "assets/goblin", 50, 64, 2.5f);
        CreateMonsterBluePrint(game.level, 'Z', 'N', "assets/zombie", 59, 95, 2.25f);
        CreateMonsterBluePrint(game.level, 'W', 'N', "assets/dog", 96, 74, 2.0f);
        CreateObjectBluePrint(game.level, 'S', '*', "assets/skeleton", 64, 22, 1.0f);
        CreateObjectBluePrint(game.level, 'I', '*', "assets/pillar", 50, 128, 3.0f);
        //LoadLevel(game.level, game.levelRenderer, testMap2, 10, 18);
        LoadLevel(game.level, game.levelRenderer, testMap1, 9, 9);
        CreateModelInstance(game.level, 1, 1, CubeSide::WEST, 1.0f, torchModel);
        CreateModelInstance(game.level, 1, 1, CubeSide::NORTH, 1.0f, torchModel);
        CreateModelInstance(game.level, 1, 2, CubeSide::WEST, 1.0f, torchModel);

        CreateModelInstance(game.level, 1, 7, CubeSide::WEST, 1.0f, torchModel);
        CreateModelInstance(game.level, 1, 7, CubeSide::NORTH, 1.0f, torchModel);
        CreateModelInstance(game.level, 1, 7, CubeSide::SOUTH, 1.0f, torchModel);

        CreateModelInstance(game.level, 7, 7, CubeSide::EAST, 1.0f, torchModel);
        CreateModelInstance(game.level, 4, 6, CubeSide::WEST, 1.0f, torchModel);

        CreateModelInstance(game.level, 1, 1, CubeSide::BOTTOM, 1.0f, barrelModel);
        CreateModelInstance(game.level, 4, 6, CubeSide::BOTTOM, 1.0f, barrelModel);
        CreateModelInstance(game.level, 5, 4, CubeSide::BOTTOM, 1.0f, barrelModel);
        CreateModelInstance(game.level, 7, 2, CubeSide::BOTTOM, 1.0f, barrelModel);
        CreateModelInstance(game.level, 1, 5, CubeSide::BOTTOM, 1.0f, barrelModel);

        CreateModelInstance(game.level, 3, 1, CubeSide::NORTH, 1.0f, benchModel);
        CreateModelInstance(game.level, 2, 1, CubeSide::NORTH, 1.0f, torchModel);
        CreateModelInstance(game.level, 4, 1, CubeSide::NORTH, 1.0f, torchModel);
        CreateModelInstance(game.level, 5, 1, CubeSide::NORTH, 1.0f, benchModel);
        CreateModelInstance(game.level, 7, 1, CubeSide::EAST, 1.0f, benchModel);

    }

    void UpdateGame(Game& game, float frameDelta) {
        handleMouseInput(game);
        handleInput(game, frameDelta);

        UpdateLevel(game.level, game.levelRenderer, frameDelta);
        UpdateLevelRenderer(game.levelRenderer, frameDelta);
        RenderLevel(game.levelRenderer, frameDelta);

        /*
        UpdateAnimations(game.animation, frameDelta);
        Clear(game.renderer->renderBuffer);
        PushText(game.renderer->renderBuffer, "Hello World", game.font, 100, 100, Renderer::WHITE);
        auto matrix = glm::identity<glm::mat4>();
        matrix = glm::translate(matrix, glm::vec3(550, 250, 0));
        static float angle = 0.0f;
        matrix = glm::rotate(matrix, angle, glm::vec3(0, 0, 1));
        angle += (float) frameDelta;
        matrix = glm::translate(matrix, glm::vec3(-550, -250, 0));
        PushTransform(game.renderer->renderBuffer, matrix);

        PushBlendMode(game.renderer->renderBuffer, Renderer::BlendMode::ALPHA);

        auto col = Renderer::BLUE;
        col.a = 0.25f;

        Renderer::Quad q = { .color = col, .left = 500, .top = 200, .right = 600, .bottom = 300 };
        PushQuad(game.renderer->renderBuffer, q);

        PushBlendMode(game.renderer->renderBuffer, Renderer::BlendMode::ALPHA);

        matrix = glm::identity<glm::mat4>();
        PushTransform(game.renderer->renderBuffer, matrix);

        //RenderScene(game.scene, game.renderer->renderBuffer, frameDelta);

        matrix = glm::identity<glm::mat4>();
        matrix = glm::translate(matrix, glm::vec3(500, 500, 0));
        matrix = glm::scale(matrix, glm::vec3(4.0f, 4.0f, 0.0f));
        matrix = glm::translate(matrix, glm::vec3(-500, -500, 0));
        PushTransform(game.renderer->renderBuffer, matrix);
        RenderAnimation(game.animation, game.controllerId, game.renderer->renderBuffer, 500, 500);
        PushTransform(game.renderer->renderBuffer, glm::identity<glm::mat4>());

        //PushLine(game.renderer->renderBuffer, Vector2(0, 0), Vector2(100, 100), Renderer::YELLOW);
        UpdateRenderer(*game.renderer, frameDelta);
         */
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

