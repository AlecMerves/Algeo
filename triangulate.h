#ifndef TRIANGLE
#define TRIANGLE

#include "geometry.h"
#include "draw_util.h"

void Triangulate(sframe* frame, tVertex* ptvertices) {
    if (!(*ptvertices)) { return; }

    tVertex v0, v1, v2, v3, v4;
    int n = (*ptvertices)->prev->vnum + 1;

    EarInit(*ptvertices);
    while (n > 3) {
        v2 = *ptvertices;
        do {
            if (v2->ear) {
                v3 = v2->next; v4 = v3->next;
                v1 = v2->prev; v0 = v1->prev;

                draw_line(frame, v1->v, v3->v);

                v1->ear = Diagonal(*ptvertices, v0, v3);
                v3->ear = Diagonal(*ptvertices, v1, v4);

                v1->next = v3;
                v3->prev = v1;
                FREE(v2)
                *ptvertices = v3;
                n--;
                break;
            }
            v2 = v2->next;
        } while (v2 != *ptvertices);
    }
}

#endif