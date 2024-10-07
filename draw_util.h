#ifndef DRAW_UTIL
#define DRAW_UTIL

#include "vertex_types.h"
#include "frame.h"

#define XDIR  0b100
#define YDIR  0b010
#define XYDIR 0b001
#define NOMIRROR 0
#define FOURWAY XDIR | YDIR
#define EIGHTWAY FOURWAY | XYDIR

#define LINE_WIDTH 1

void clear_screen(sframe* frame) {
    for (int i = 0; i < frame->width * frame->height; i++) { 
        if (i % frame->width < FRAME_WIDTH(frame->width)) {
            frame->pixels[i] = WHITE;
        }
    }
}

void clear_console(sframe* frame) {
    int color = WHITE;
    for (int x = 0; x < CONSOLE_WIDTH(frame->width); x++) {
        for (int y = 0; y < frame->height; y++) {
            if (x < LINE_WIDTH)
                color = BLACK;
            frame->pixels[x + FRAME_WIDTH(frame->width) + y * frame->width] = color;
            color = WHITE;
        }
    }
}

tVertex clear(HWND hwnd, sframe* frame, tVertex vertices) {
    clear_screen(frame);
    InvalidateRect(hwnd, NULL, FALSE);
    UpdateWindow(hwnd);
    return clear_vertices(vertices);
}

void draw_point(sframe* frame, tPointi p, int x_off, int y_off) {
    int x = p[X] + x_off;
    int y = frame->height - p[Y] + y_off;
    if (x >= 0 && x < FRAME_WIDTH(frame->width) && y >= 0 && y < frame->height)
        frame->pixels[(y * frame->width) + x] = BLACK;
}

void mirror(sframe* frame, tPointi origin, int x, int y, char mode) {
    int x_off, y_off;
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            for (int k = 0; k < 2; k++) {
                x_off = x; y_off = y;
                if ((mode & XYDIR) && k == 1) { x_off = y; y_off = x; } 
                if ((mode & XDIR)  && j == 1) { x_off = -x_off; }
                if ((mode & YDIR)  && i == 1) { y_off = -y_off; }
                draw_point(frame, origin, x_off, y_off);
            }
        }
    }

}

// Bresenham's line Algorithm (YouTube NoBS code) (COME BACK TO SEE IF CAN BE ONE FUNCTION)
void draw_lineH(sframe* frame, tPointi p1, tPointi p2) {
    tPointi current;
    int temp, dx, dy, y, dir, D;
    int x0 = p1[X], y0 = p1[Y];
    int x1 = p2[X], y1 = p2[Y];

    if (x0 > x1) {
        temp = x0; x0 = x1; x1 = temp;
        temp = y0; y0 = y1; y1 = temp;
    }

    dx = x1 - x0; dy = y1 - y0;
    dir = (dy < 0) ? -1 : 1;
    dy *= dir;

    if (dx != 0) {
        y = y0;
        D = 2*dy - dx;
        for (int i = 0; i <= dx; i++) {
            current[X] = x0 + i;
            current[Y] = y;
            draw_point(frame, current, 0, 0);
            if (D >= 0) {
                y += dir;
                D -= 2*dx;
            }
            D += 2*dy;
        }
    }
}

void draw_lineV(sframe* frame, tPointi p1, tPointi p2) {
    tPointi current;
    int temp, dx, dy, x, dir, D;
    int x0 = p1[X], y0 = p1[Y];
    int x1 = p2[X], y1 = p2[Y];

    if (y0 > y1) {
        temp = x0; x0 = x1; x1 = temp;
        temp = y0; y0 = y1; y1 = temp;
    }

    dx = x1 - x0; dy = y1 - y0;
    dir = (dx < 0) ? -1 : 1;
    dx *= dir;

    if (dy != 0) {
        x = x0;
        D = 2*dx - dy;
        for (int i = 0; i <= dy; i++) {
            current[X] = x;
            current[Y] = y0 + i;
            draw_point(frame, current, 0, 0);
            if (D >= 0) {
                x += dir;
                D -= 2*dy;
            }
            D += 2*dx;
        }
    }
}

void draw_line(sframe* frame, tPointi p1, tPointi p2) {
    if (abs(p2[X] - p1[X]) > abs(p2[Y] - p1[Y])) {
        draw_lineH(frame, p1, p2);
    } else {
        draw_lineV(frame, p1, p2);
    }
}

void draw_chain(sframe* frame, tVertex vertices) {
    if (!vertices) { return; }
    tVertex current = vertices;
    while (current->next != vertices) {
        draw_line(frame, current->v, current->next->v);
        current = current->next;
    }
}

void draw_polygon(sframe* frame, tVertex vertices) {
    if (!vertices) { return; }
    draw_chain(frame, vertices);
    draw_line(frame, vertices->prev->v, vertices->v);
}

// Midpoint Circle Algorithm (YouTube NoBS Code)
void draw_circle(sframe* frame, tPointi center, int radius, bool fill) {
    int x_temp, x = 0, y = -radius, p = -radius;
    while (x < -y) {
        if (p > 0) {
            p += 2*(x + ++y) + 1;
            if (fill) {
                x_temp = 0;
                while (x_temp < x) {
                    mirror(frame, center, x_temp++, y, EIGHTWAY);
                }
            }
        } else {
            p += 2*x + 1;
        }

        mirror(frame, center, x++, y, EIGHTWAY);
    }
    
    if (fill) {
        for (int i = y; i <= 0; i++) {
            x_temp = 0;
            while (x_temp <= -i) {
                mirror(frame, center, x_temp++, i, EIGHTWAY);
            }
        }
    }
}

void draw_points(sframe* frame, tVertex vertices) {
    if (!vertices) { return; }
    tVertex current = vertices;
    do {
        draw_circle(frame, current->v, 5, TRUE);
        current = current->next;
    } while (current != vertices);
}

#endif