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
#include "title.h"

#include <stdio.h>
#include <libdragon.h>


static void initialiseSubsystems() {
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

static void inputStep() {
    controller_scan();
    N64ControllerState keysPressed = get_keys_pressed();
    N64ControllerState keysReleased = get_keys_up();

    if (!INIT_TURN_SECONDS && keysReleased.c[0].Z) {
        startNewDay();
    }

    handleController(&keysPressed, &keysReleased);
};

static void drawPause() {
    drawText("PAUSED", 128, 128, 2);
    drawWeatherGuide(224);
}

static void resetScreen() {
    graphics_fill_screen(1, 0xffffffff);
    graphics_fill_screen(2, 0xffffffff);
}

display_context_t nextFrame = 0;

void renderFrame() {
    nextFrame = display_lock();
    if (!nextFrame) { return; }

    disable_interrupts();
    rdp_sync(SYNC_PIPE);

    fps_frame();

    // while(!(nextFrame = display_lock()))

    rdp_attach_display(nextFrame);

    drawBox(BG_SPRITE, 0, 0, SCREEN_WIDTH, SCREEN_WIDTH);

    switch (getPlayer()->state) {
        case STATE_TITLE:
            drawTitle();
            break;
        case STATE_PAUSE:
            drawPause();
            break;
        case STATE_PLAY:
            drawDay();
            drawLines();
            drawQueue();
            drawWeather();
            drawPlayer();
            break;
        case STATE_GAMEOVER:
            drawText("Game Over", 100, 100, 2);
            break;
        default:
            drawText("Error", 100, 100, 2);
            break;
    }


#ifdef SHOW_FRAME_COUNT
    string text;
    sprintf(text, "FPS: %d", fps_get());
    drawText(text, 0, 460, 1);
#endif

    rdp_detach_display();
    display_show(nextFrame);

    enable_interrupts();
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

    initTitle();

    // Render a frame 30 times a second.
    new_timer(TIMER_TICKS(TICKS_PER_SECOND) / 30, TF_CONTINUOUS, renderFrame);

    while(true) {
        inputStep();
    }
}