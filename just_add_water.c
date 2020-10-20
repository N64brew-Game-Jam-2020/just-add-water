#include "weather.h"
#include "line.h"
#include "core.h"
#include "clothManager.h"
#include "text.h"
#include "config.h"
#include "player.h"
#include "day.h"
#include "fps.h"
#include "resources.h"

#include <stdio.h>
#include <libdragon.h>



static u32 clothsPerDay = INIT_TURN_CLOTHS;
static u32 secondsPerDay = INIT_TURN_SECONDS;

void initialiseSubsystems() {
    initLine();
    initPlayer();
    initClothManager();

    init_interrupts();
    display_init(RESOLUTION, COLOUR_DEPTH, 2, GAMMA_NONE, ANTIALIAS_OFF);
    dfs_init(DFS_DEFAULT_LOCATION);
    rdp_init();
    controller_init();
    timer_init();
    initText();
    initResources();
}

void inputStep() {
    controller_scan();
    N64ControllerState keysPressed = get_keys_pressed();
    N64ControllerState keysReleased = get_keys_up();

    if (!secondsPerDay && keysReleased.c[0].A) {
        startNewDay();
    }

    if (handleController(&keysPressed, &keysReleased)) {
        onStateChanged();
    };
};

void renderStep() {
    display_context_t frameId;

    fps_frame();

    while(!(frameId = display_lock()));

    rdp_attach_display(frameId);

    if (!isRenderRequired()) {
        drawBox(DRY_SPRITE, 0, 0, SCREEN_WIDTH, SCREEN_WIDTH);

        if (getPlayer()->isPaused) {
            drawText("PAUSE", 100, 100, 2);
        } else {
            drawDay();
            drawLine();
            drawQueue();
            drawWeather();
            drawPlayer();
        }
    } else {
        onRendered();
    }

#ifdef SHOW_FRAME_COUNT
    string text;
    sprintf(text, "FPS: %d", fps_get());
    drawText(text, 0, 230, 1);
#endif

    rdp_detach_display();
    display_show(frameId);
}

void resetScreen() {
    graphics_fill_screen(1, 0xffffffff);
    graphics_fill_screen(2, 0xffffffff);
}

int main(void) {
    initialiseSubsystems();
    resetScreen();

    #ifdef SHOW_FRAME_COUNT
        new_timer(TIMER_TICKS(1000000), TF_CONTINUOUS, fps_timer);
    #endif

    // Assure RDP is ready for new commands
    rdp_sync(SYNC_PIPE);
    // Remove any clipping windows
    rdp_set_default_clipping();
    // Enable sprite display instead of solid color fill
    rdp_enable_texture_copy();

    startNewDay();

    while(true) {
        inputStep();
        renderStep();
    }
}