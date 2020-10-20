#define STANDARD_MARGIN 10

#define INIT_TURN_SECONDS 0
#define INIT_TURN_CLOTHS 2

// Weather frame
// TODO

// Queue Frame
#define QUEUE_MARGIN 10

// Lines frame
#define LEFT_MARGIN 64
#define OUTSIDE_LINE_POSITION 40
#define ROOF_POSITION 80
#define INSIDE_LINE_POSITION 140
#define OUTSIDE_LINE_SIZE 24
#define INSIDE_LINE_SIZE 24

#define CLOTH_QUEUE_SIZE 15
#define CLOTH_QUEUE_SHOWN 3

#define TILE_WIDTH 8

//#define RESOLUTION RESOLUTION_640x480
#define RESOLUTION RESOLUTION_320x240
#if RESOLUTION==RESOLUTION_320x240 
    #define SCREEN_WIDTH 320
    #define SCREEN_HEIGHT 240
#else
    #define SCREEN_WIDTH 640
    #define SCREEN_HEIGHT 480
#endif

#define COLOUR_DEPTH DEPTH_16_BPP

#define RANDOMISE_CLOTHS 1
#define SHOW_FRAME_COUNT 1