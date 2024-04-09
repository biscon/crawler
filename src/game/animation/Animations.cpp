//
// Created by bison on 17-10-23.
//

#include <SDL_log.h>
#include "Animations.h"

namespace Game {

    void InitAnimations(Animations &anim) {
        anim.nextId = 1;
        
    }

    void ShutdownAnimations(Animations &anim) {
        
    }

    static void buildAnim(Animations &anim, AnimationInfo& info, Renderer::TextureAtlasBuilder& builder, LoadedAnimation& la, const FloatRect* insets) {
        Renderer::PixelBuffer sheet_pb(info.filename, false);
        u32 widthInFrames = sheet_pb.width / info.frameWidth;
        u32 heightInFrames = sheet_pb.height / info.frameHeight;
        la.id = info.id;
        la.frameCount = widthInFrames * heightInFrames;
        la.frameWidth = info.frameWidth;
        la.frameHeight = info.frameHeight;

        for(u32 y = 0; y < heightInFrames; ++y) {
            for(u32 x = 0; x < widthInFrames; ++x) {
                auto pb = std::make_unique<Renderer::PixelBuffer>(info.frameWidth+2, info.frameHeight+2, Renderer::PixelFormat::RGBA);
                pb->padding = true;
                pb->copyFrom(sheet_pb, UIntRect(x * info.frameWidth, info.frameHeight * y, info.frameWidth, info.frameHeight), UIntPos(1,1));

                auto size = pb->getTrimmedSize();
                UIntRect sizeI = UIntRect(0, 0, pb->width - 2, pb->height - 2);

                Frame frame;
                frame.textureAtlasId = builder.add(*pb);
                frame.combatBox = FloatRect((float) size.x, (float) size.y, (float) size.x + (float) size.w, (float) size.y + (float) size.h);
                frame.delay = 1.0f / (float) info.fps;

                if(insets != nullptr) {
                    sizeI.x = (u32) ((float) sizeI.w * insets->left);
                    sizeI.w -= (u32) ((float) sizeI.w * insets->right) + sizeI.x;
                    sizeI.y = (u32) ((float) sizeI.h * insets->top);
                    sizeI.h -= (u32) ((float) sizeI.h * insets->bottom) + sizeI.y;
                    SDL_Log("Anim frame insetted: %d,%d,%d,%d", sizeI.x, sizeI.y, sizeI.w, sizeI.h);
                    frame.box = FloatRect((float) sizeI.x, (float) sizeI.y, (float) sizeI.x + (float) sizeI.w, (float) sizeI.y + (float) sizeI.h);

                } else {
                    frame.box = frame.combatBox;
                }
                anim.frames.emplace_back(frame);
                // or framekey together based on animation id and frame id
                u32 frameId = (u32) anim.frames.size() - 1;
                u64 frameKey = ((u64) info.id << 32) | (u64) frameId;
                anim.animationFrames.insert(std::pair(frameKey, frameId));
            }
        }
    }

    void LoadAnimations(Animations &anim) {
        auto builder = Renderer::TextureAtlasBuilder(1024, 1024, Renderer::PixelFormat::RGBA);
        for(auto& info : anim.animations) {
            LoadedAnimation la{};
            auto insets = anim.insets.find(info.id);
            if(insets != anim.insets.end()) {
                buildAnim(anim, info, builder, la, &*insets);
            } else {
                buildAnim(anim, info, builder, la, nullptr);
            }
            anim.loadedAnimations.insert(la.id, la);
        }
        anim.insets.clear();
        anim.animations.clear();
        builder.build(anim.textureAtlas);
        SDL_Log("Loaded %zu animation(s)", anim.loadedAnimations.size());
    }

    void UpdateAnimations(Animations &anim, float delta) {
        for(auto& id : anim.playingControllers) {
            auto controller = anim.controllers.find(id);
            if(controller == anim.controllers.end()) {
                SDL_Log("Animations controller %d not found", id);
                continue;
            }
            u64 frameKey = ((u64) controller->animationId << 32) | (u64) controller->currentFrame;
            auto frameId = anim.animationFrames.find(frameKey);
            if(frameId == anim.animationFrames.end()) {
                SDL_Log("Animations frame %ld not found", frameKey);
                continue;
            }

            if(controller->timer >= anim.frames[frameId->second].delay) {
                controller->timer = 0;
                if(controller->reverse) {
                    if(controller->currentFrame == 0) {
                        if(controller->repeatType == RepeatType::ReverseOnce) {
                            anim.stoppedControllers.emplace_back(id);
                            SDL_Log("Animations %d stopped", id);
                            continue;
                        } else if(controller->repeatType == RepeatType::Reverse) {
                            controller->currentFrame = controller->frameCount - 1;
                        } else {
                            controller->currentFrame++;
                        }
                    } else {
                        controller->currentFrame--;
                    }
                } else {
                    if(controller->currentFrame == controller->frameCount - 1) {
                        if(controller->repeatType == RepeatType::Once) {
                            anim.stoppedControllers.emplace_back(id);
                            SDL_Log("Animations %d stopped", id);
                            continue;
                        } else if(controller->repeatType == RepeatType::Restart) {
                            controller->currentFrame = 0;
                        } else if(controller->repeatType == RepeatType::Reverse || controller->repeatType == RepeatType::ReverseOnce) {
                            controller->reverse = true;
                            controller->currentFrame--;
                        } else {
                            controller->currentFrame++;
                        }
                    } else {
                        controller->currentFrame++;
                    }
                }
            } else {
                controller->timer += delta;
            }
        }
        for(auto& id : anim.stoppedControllers) {
            auto controller = anim.controllers.find(id);
            if(controller == anim.controllers.end()) {
                SDL_Log("Animations controller %d not found", id);
                continue;
            }
            SDL_Log("Removing animation controller %d", id);
            anim.playingControllers.erase(std::remove(anim.playingControllers.begin(), anim.playingControllers.end(), id), anim.playingControllers.end());
            anim.controllers.remove(id);
        }
        anim.stoppedControllers.clear();
    }


    u32 CreateAnimation(Animations &anim, u32 frameWidth, u32 frameHeight, u32 fps, const std::string &filename, const FloatRect* insets) {
        AnimationInfo info;
        info.id = anim.nextId++;
        info.frameWidth = frameWidth;
        info.frameHeight = frameHeight;
        info.filename = filename;
        info.fps = fps;
        anim.animations.insert(info.id, info);
        if(insets) {
            anim.insets.insert(info.id, *insets);
        }
        return info.id;
    }

    void DestroyAnimation(Animations &anim, u32 id) {

    }

    u32 PlayAnimation(Animations &anim, u32 id, RepeatType repeatType, bool reverse) {
        AnimationController controller{};
        controller.id = anim.nextId++;
        controller.animationId = id;
        controller.repeatType = repeatType;
        controller.reverse = reverse;
        controller.currentFrame = 0;
        controller.timer = 0;
        controller.frameCount = anim.loadedAnimations.find(id)->frameCount;
        anim.controllers.insert(controller.id, controller);
        anim.playingControllers.emplace_back(controller.id);
        SDL_Log("Playing animation controller %d", controller.id);
        return controller.id;
    }

    void StopAnimation(Animations &anim, u32 controllerId) {
        anim.stoppedControllers.emplace_back(controllerId);
    }

    void RenderAnimation(Animations &anim, u32 controllerId, Renderer::RenderBuffer &renderBuffer, float x, float y) {
        auto controller = anim.controllers.find(controllerId);
        if(controller == anim.controllers.end()) {
            SDL_Log("Animations controller %d not found", controllerId);
            return;
        }
        u64 frameKey = ((u64) controller->animationId << 32) | (u64) controller->currentFrame;
        auto frameId = anim.animationFrames.find(frameKey);
        if(frameId == anim.animationFrames.end()) {
            SDL_Log("Animations frame %ld not found", frameKey);
            return;
        }
        auto& frame = anim.frames[frameId->second];
        auto la = anim.loadedAnimations.find(controller->animationId);
        if(la == anim.loadedAnimations.end()) {
            SDL_Log("Animations loaded animation %d not found", controller->animationId);
            return;
        }
        Renderer::AtlasQuad q = {
            .color = Renderer::WHITE,
            .atlasId = frame.textureAtlasId,
            .left = x,
            .top = y,
            .right = x + (float) la->frameWidth,
            .bottom = y + (float) la->frameHeight,
        };
        Renderer::PushAtlasQuad(renderBuffer, q, anim.textureAtlas);
    }
}
