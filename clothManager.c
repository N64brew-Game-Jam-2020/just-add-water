#include "clothManager.h"
#include "cloth.h"
#include "text.h"
#include "config.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define CLOTH_QUEUE_SIZE 15

static Cloth** masterClothList;
static u32 clothListLength;
static Cloth* clothQueue[CLOTH_QUEUE_SIZE];
static u32 queueIndex = 0;

// Can be extended if necessary.
static u32 clothListMaxLength = 32;

void initClothManager() {
    masterClothList = calloc(sizeof(Cloth*), clothListMaxLength);
    clothListLength = 0;
    enqueueCloth();
}

static SpriteCode getClothSprite(Cloth* cloth) {
    switch(cloth->dryingState) {
        case DRYING_SPUN:
            return SPUN_SPRITE;
        case DRYING_DAMP:
            return DAMP_SPRITE;
        case DRYING_DRENCHED:
            return DRENCHED_SPRITE;
        case DRYING_DRY:
        case DRYING_COMPLETE:
            return DRY_SPRITE;
        default:
            // TODO throw error.
            return ROOF_SPRITE;
    }
}

void drawQueue() {
    drawText("NEXT", QUEUE_MARGIN, QUEUE_MARGIN, 1);

    Cloth* next = clothQueue[0];
    if (!next) {
        return;
    }

    SpriteCode spriteId = getClothSprite(next);

    u32 baseX = QUEUE_MARGIN;
    u32 baseY = QUEUE_MARGIN + STANDARD_MARGIN;

    for (u32 i = 0; i < next->size; i++) {
        drawSprite(
            spriteId,
            baseX + i * TILE_WIDTH,
            baseY,
            1
        );
        drawSprite(
            spriteId,
            baseX + i * TILE_WIDTH,
            baseY + TILE_WIDTH,
            1
        );
    }

    drawText(next->text, baseX, baseY, 1);
}

/**
 * Return the cloth at the head of the queue and shuffle the rest along.
 */
Cloth* dequeueCloth() {
    Cloth* result = clothQueue[0];

    for (u32 i = 0; i < CLOTH_QUEUE_SIZE - 1; i++) {
        clothQueue[i] = clothQueue[i+1];
    }

    clothQueue[CLOTH_QUEUE_SIZE - 1] = 0;

    queueIndex--;

    return result;
}

static void initNewCloth(Cloth* cloth) {
    cloth->dryingState = DRYING_SPUN;

    // Will be randomised - increasing in complexity as time goes on.
    cloth->size = 2;
    cloth->growthFactor = 1;
    cloth->growthType = GROWTH_LINEAR;
    buildClothText(cloth);
}

/**
 * Create a new cloth and add it to the upcoming queue
 * @return true if successful, false if the queue has overflowed.
 */
bool enqueueCloth() {
    if (queueIndex >= CLOTH_QUEUE_SIZE) {
        return false;
    }

    Cloth* newCloth = calloc(sizeof(Cloth), 1);

    initNewCloth(newCloth);

    masterClothList[clothListLength] = newCloth;

    clothListLength++;
    if (clothListLength >= clothListMaxLength) {
        u32 newMaxLength = clothListMaxLength * 2;
        Cloth** temp = masterClothList;
        masterClothList = calloc(sizeof(Cloth*), newMaxLength);
        memcpy(masterClothList, temp, clothListMaxLength * sizeof(Cloth*));
        clothListMaxLength = newMaxLength;

        free(temp);
    }

    clothQueue[queueIndex] = newCloth;
    queueIndex++;

    return true;
}

/**
 * Recursively shifts cleaned up cloths out of existence.
 */
void removeFinishedCloths(u32 startIndex) {
    bool freedFound = false;
    for (u32 i = startIndex; i < clothListLength; i++) {
        if (!masterClothList[i]) {
            removeFinishedCloths(i + 1);
            freedFound = true;
        }

        if (freedFound) {
            masterClothList[i] = masterClothList[i + 1];
        }
    }
}

/**
 * Checks if cloths can be freed from memory, then does so.
 */
void processFinishedCloths() {
    u32 removedCloths = 0;

    for(u32 i = 0; i < clothListLength; i++) {
        if (masterClothList[i]->isFreeable) {
            // Do some fiddling with scores or something.
            free(masterClothList[i]);
            masterClothList[i] = 0;
            removedCloths++;
        }
    }

    clothListLength = clothListLength - removedCloths;

    removeFinishedCloths(0);
}

