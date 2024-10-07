#ifndef FRAME
#define FRAME

#include <stdint.h>

#define FRAME_RATIO 0.8
#define CONSOLE_RATIO (1.0 - FRAME_RATIO)
#define CONSOLE_WIDTH(size) (int)(CONSOLE_RATIO * size)
#define FRAME_WIDTH(size) (int)(FRAME_RATIO * size + 1)

#define BLACK 0x00000000
#define WHITE 0x00FFFFFF

#define POINTS_MODE 0
#define CHAIN_MODE 1
#define POLYGON_MODE 2

typedef struct sframe {
    int width;
    int height;
    uint32_t *pixels;
} sframe;

#endif