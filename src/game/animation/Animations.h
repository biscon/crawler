//
// Created by bison on 17-10-23.
//

#ifndef DOD_ANIMATIONS_H
#define DOD_ANIMATIONS_H

#include <vector>
#include <unordered_map>
#include "Vector2.h"
#include "../../renderer/RenderBuffer.h"
#include "SparseVector.h"

namespace Game {
    enum class RepeatType {
        Once,
        Restart,
        Reverse,
        ReverseOnce
    };

    struct Frame {
        float delay;
        u32 textureAtlasId;
        FloatRect box;
        FloatRect combatBox;
    };

    struct AnimationInfo {
        u32 id;
        u16 frameWidth;
        u16 frameHeight;
        u16 fps;
        std::string filename;
    };

    struct LoadedAnimation {
        u32 id;
        u16 frameWidth;
        u16 frameHeight;
        u16 frameCount;
    };

    struct AnimationController {
        u32 id;
        u32 animationId;
        RepeatType repeatType;
        bool reverse;
        u32 currentFrame;
        u16 frameCount;
        float timer;
    };

    struct Animations {
        Renderer::TextureAtlas textureAtlas;
        u32 nextId;
        SparseVector<FloatRect> insets;
        std::vector<Frame> frames;
        std::unordered_map<u64, u32> animationFrames;
        SparseVector<AnimationInfo> animations;
        SparseVector<LoadedAnimation> loadedAnimations;
        SparseVector<AnimationController> controllers;
        std::vector<u32> playingControllers;
        std::vector<u32> stoppedControllers;
    };

    void InitAnimations(Animations& anim);
    void ShutdownAnimations(Animations& anim);
    void LoadAnimations(Animations& anim);
    void UpdateAnimations(Animations & anim, float delta);

    u32 CreateAnimation(Animations& anim, u32 frameWidth, u32 frameHeight, u32 fps, const std::string& filename, const FloatRect* insets);
    void DestroyAnimation(Animations& anim, u32 id);

    u32 PlayAnimation(Animations& anim, u32 id, RepeatType repeatType, bool reverse);
    void StopAnimation(Animations& anim, u32 controllerId);

    void RenderAnimation(Animations& anim, u32 controllerId, Renderer::RenderBuffer& renderBuffer, float x, float y);
}
#endif //DOD_ANIMATIONS_H
