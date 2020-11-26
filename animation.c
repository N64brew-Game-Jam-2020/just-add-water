#include "animation.h"
#include "renderer.h"

#define MAX_ANIMATIONS 32
static Animation* _queue[MAX_ANIMATIONS];
static u32 _queueIndex = 0;

void initAnimation() {
    memset(_queue, 0, sizeof(Animation*) * MAX_ANIMATIONS);
}

Animation* newAnimation() {
    Animation* result = calloc(1, sizeof(Animation));
    return result;
}

void drawAnimations() {
    for(u32 i = 0; i < MAX_ANIMATIONS; i++) {
        Animation* animation = _queue[i];
        if (animation) {
            Frame* frame = &animation->frames[animation->currentFrameIndex];
            if (!frame) {
                abandonAnimation(animation);
                continue;
            }

            drawScaledSprite(frame->sprite, frame->x, frame->y, frame->z, frame->scaleX, frame->scaleY);
            frame->remainingCycles--;
            if (frame->remainingCycles == 0) {
                animation->currentFrameIndex++;
            }
        }
    }
}

static void addToQueue(Animation* animation) {
    animation->currentFrameIndex = 0;
    animation->queueIndex = _queueIndex;
    _queue[_queueIndex] = animation;

    _queueIndex++;

    // Thought of coming up with a robust system to allocate new indices,
    // But I'm in a hurry so lets just assume it's not gonna be a big deal if we have to wrap around.
    if (_queueIndex >= MAX_FRAMES) {
        _queueIndex = 0;
    }
}

static void removeFromQueue(Animation* animation) {
    _queue[animation->queueIndex] = 0;
    animation->queueIndex = 0;
}

void startAnimation(Animation* animation) {
    addToQueue(animation);
}

void abandonAnimation(Animation* animation) {
    removeFromQueue(animation);
    free(animation);
    free(0);
    animation = 0;
}

void setSimpleFrame(Frame* frame, SpriteCode sprite, u32 x, u32 y, float seconds) {
    frame->sprite = sprite;
    frame->x = x;
    frame->y = y;
    frame->z = 3;
    frame->scaleX = 1;
    frame->scaleY = 1;
    frame->remainingCycles = seconds * 30; // assumes 30fps
}

