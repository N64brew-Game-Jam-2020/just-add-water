#include "line.h"
#include "cloth.h"
#include "weather.h"
#include <string.h>

#define OUTSIDE_LINE_SIZE 16
#define INSIDE_LINE_SIZE 8

static Cloth* outsideLine[OUTSIDE_LINE_SIZE];
static Cloth* insideLine[INSIDE_LINE_SIZE];

void initLine() {}

void updateHangingCloths(Weather weather) {
    int i = 0;

    // First pass - resize the cloths.
    while (i < OUTSIDE_LINE_SIZE) {
        if (outsideLine[i]) {
            u32 oldSize = outsideLine[i]->size;
            updateCloth(outsideLine[i], weather);
            i += oldSize;
        } else {
            i++;
        }
    }

    i = 0;
    while (i < OUTSIDE_LINE_SIZE) {
        if (insideLine[i]) {
            u32 oldSize = outsideLine[i]->size;
            // Inside is always considered 'cloudy'
            updateCloth(insideLine[i], WEATHER_CLOUDY);
            i += oldSize;
        } else {
            i++;
        }
    }

    // Second pass - shuffle them along with the change in size and dump any that fell off.

    Cloth* outsideTemp[OUTSIDE_LINE_SIZE];
    memcpy(outsideTemp, outsideLine, sizeof(Cloth*) * OUTSIDE_LINE_SIZE);

    u32 iSource = 0;
    u32 iDest = 0;

    Cloth* lastCloth = 0;

    while (iSource < OUTSIDE_LINE_SIZE) {
        // Copy the cloths back to the line, but at the new size.
        if (outsideTemp[iSource] && lastCloth != outsideTemp[iSource]) {
            lastCloth = outsideTemp[iSource];

            // If cloth can't fit on the line, it falls and becomes dirty.
            if (iDest + lastCloth->size > OUTSIDE_LINE_SIZE) {
                lastCloth->dryingState = DRYING_DIRTY;
            } else {
                for (u32 j = 0; j < lastCloth->size; j++) {
                    outsideLine[iDest] = lastCloth;
                    iDest++;
                }
            }
        }

        iSource++;
    }

    // Do the same for the inside line.
    iSource = 0;
    iDest = 0;

    Cloth* insideTemp[INSIDE_LINE_SIZE];
    memcpy(insideTemp, insideLine, sizeof(Cloth*) * INSIDE_LINE_SIZE);

    while (iSource < INSIDE_LINE_SIZE) {
        // Copy the cloths back to the line, but at the new size.
        if (insideTemp[iSource] && lastCloth != insideTemp[iSource]) {
            lastCloth = insideTemp[iSource];

            // If cloth can't fit on the line, it falls and becomes dirty.
            if (iDest + lastCloth->size > INSIDE_LINE_SIZE) {
                lastCloth->dryingState = DRYING_DIRTY;
            } else {
                for (u32 j = 0; j < lastCloth->size; j++) {
                    insideTemp[iDest] = lastCloth;
                    iDest++;
                }
            }
        }

        iSource++;
    }
}