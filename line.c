#include "line.h"
#include "core.h"
#include "cloth.h"
#include "weather.h"
#include "config.h"
#include "text.h"
#include <stdbool.h>
#include <string.h>


typedef struct {
    Cloth** cloths;
    u32 length;
} Line;

static Cloth* _outsideLine[OUTSIDE_LINE_SIZE];
static Cloth* _insideLine[INSIDE_LINE_SIZE];

static Line outsideLine;
static Line insideLine;

void initLine() {
    outsideLine.cloths = _outsideLine;
    outsideLine.length = OUTSIDE_LINE_SIZE;

    insideLine.cloths = _insideLine;
    insideLine.length = INSIDE_LINE_SIZE;
}

static void drawLine(Line* line, u32 x, u32 y) {
    for (u32 i = 0; i < line->length; i++) {
        if (line->cloths[i]) {
            drawCloth(
                line->cloths[i],
                x + i * TILE_WIDTH,
                y
            );
            i += line->cloths[i]->size - 1;
        }
    }
}

void drawLines() {
    drawLine(&outsideLine, LEFT_MARGIN, OUTSIDE_LINE_POSITION);
    drawLine(&insideLine, LEFT_MARGIN, INSIDE_LINE_POSITION);

    for (u32 i = LEFT_MARGIN; i < LEFT_MARGIN + OUTSIDE_LINE_SIZE * TILE_WIDTH; i += TILE_WIDTH) {
        drawSprite(OUTSIDE_LINE, i, OUTSIDE_LINE_POSITION, 1);
    }
    for (u32 i = LEFT_MARGIN; i < LEFT_MARGIN + INSIDE_LINE_SIZE * TILE_WIDTH; i += TILE_WIDTH) {
        drawSprite(INSIDE_LINE, i, INSIDE_LINE_POSITION, 1);
    }
    for (u32 i = LEFT_MARGIN; i < LEFT_MARGIN + SCREEN_WIDTH - LEFT_MARGIN; i += TILE_WIDTH) {
        drawSprite(ROOF_SPRITE, i, ROOF_POSITION, 1);
    }
}

bool hangCloth(u32 lineId, u32 x, Cloth* cloth) {
    Cloth** cloths;
    if (lineId == 0) {
        cloths = outsideLine.cloths;
    } else {
        cloths = insideLine.cloths;
    }

    // If there's already a cloth where we want to put this new one
    for (u32 i = x; i < x + cloth->size; i++) {
        if (cloths[i]) {
            return false;
        }
    }

    // Otherwise hang the new cloth
    for (u32 i = x; i < x + cloth->size; i++) {
        cloths[i] = cloth;
    }

    return true;
}

static Cloth* takeClothFromLine(Line* line, u32 x) {
    if (line->cloths[x]) {
        Cloth* result = line->cloths[x];

        // Remove from the line.
        for (u32 i = 0; i < line->length; i++) {
            if (line->cloths[i] == result) {
                line->cloths[i] = 0;
            }
        }

        return result;
    } else {
        return 0;
    }
}

Cloth* takeCloth(u32 lineId, u32 x) {
    Cloth** line;
    if (lineId == 0) {
        return takeClothFromLine(&outsideLine, x);
    } else {
        return takeClothFromLine(&insideLine, x);
    }
}

static void updateClothsOnLine(Line* line, Weather weather) {
    u32 i = 0;
    // First pass - resize the cloths.
    while (i < line->length) {
        if (line->cloths[i]) {
            u32 oldSize = line->cloths[i]->size;
            updateCloth(line->cloths[i], weather);
            i += oldSize;
        } else {
            i++;
        }
    }

    // Second pass - shuffle them along with the change in size and dump any that fell off.
    u32 iSource = 0;
    u32 iDest = 0;
    Cloth* lastCloth = 0;

    Cloth* temp[line->length];
    memcpy(temp, line->cloths, sizeof(Cloth*) * line->length);
    memset(line->cloths, 0x00, sizeof(Cloth*) * line->length);

    while (iSource < line->length) {
        // Copy the cloths back to the line, but at the new size.
        if (temp[iSource] && lastCloth != temp[iSource]) {
            lastCloth = temp[iSource];

            // If cloth can't fit on the line, it falls and becomes dirty.
            if ((iDest + lastCloth->size) > line->length) {
                lastCloth->dryingState = DRYING_DIRTY;
            } else {
                for (u32 j = 0; j < lastCloth->size; j++) {
                    line->cloths[iDest] = lastCloth;
                    iDest++;
                }
            }
        } else {
            iDest++;
        }

        iSource++;
    }



}

void updateHangingCloths(Weather weather) {
    updateClothsOnLine(&outsideLine, weather);

    // Inside is always considered 'cloudy'
    updateClothsOnLine(&insideLine, WEATHER_CLOUDY);
}