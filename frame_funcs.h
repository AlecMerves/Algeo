#ifndef FRAME_FN
#define FRAME_FN

#include <windows.h>
#include "frame.h"
#include "draw_util.h"

void get_frame_region(sframe* frame, RECT* frame_region) {
    frame_region->top = 0;
    frame_region->bottom = frame->height;
    frame_region->left = 0;
    frame_region->right = FRAME_WIDTH(frame->width) - 1;
}

void get_console_region(sframe* frame, RECT* console_region) {
    console_region->top = 0;
    console_region->bottom = frame->height;
    console_region->left = FRAME_WIDTH(frame->width);
    console_region->right = frame->width;
}

void update_frame(HWND hwnd, sframe* frame, tVertex vertices, int mode) {
    RECT frame_region;
    switch (mode) {
        case POINTS_MODE:
            break;
        case CHAIN_MODE:
            draw_chain(frame, vertices);
            break;
        case POLYGON_MODE:
            draw_polygon(frame, vertices);
            break;
    }
    draw_points(frame, vertices);
    get_frame_region(frame, &frame_region);
    InvalidateRect(hwnd, &frame_region, FALSE);
    UpdateWindow(hwnd);
}

void update_console(HWND hwnd, sframe* frame) {
    RECT console_region;
    get_console_region(frame, &console_region);
    InvalidateRect(hwnd, &console_region, FALSE);
    UpdateWindow(hwnd);
}

#endif