#ifndef VERTEX_FN
#define VERTEX_FN

#include <stdio.h>
#include "vertex_types.h"

#define EXIT_FAILURE 1

#define NEW(p, type) \
        if ((p = (type*)malloc(sizeof(type))) == NULL) {\
            printf("NEW: Out of Memory!\n");\
            exit(EXIT_FAILURE);\
        }

#define ADD(head, p) \
        if (head) { \
            p->next = head; \
            p->prev = head->prev; \
            head->prev = p; \
            p->prev->next = p; \
        } else { \
            head = p; \
            head->next = head->prev = p; \
        }

#define FREE(p) if (p) { free ((char*)p); p = NULL; }

tVertex remove_vertex(tVertex vertex) {
    vertex->prev->next = vertex->next;
    vertex->next->prev = vertex->prev;
    FREE(vertex)
    return vertex;
}

tVertex clear_vertices(tVertex vertices) {
    if (!vertices) { return vertices; }
    while (vertices != vertices->next) {
        remove_vertex(vertices->next);
    }
    FREE(vertices)
    return vertices;
}

void print_vertex(char* out, tVertex vertex) {
    char temp[100];
    sprintf(temp, " Vertex %d | (%d, %d)\n", vertex->vnum, vertex->v[X], vertex->v[Y]);
    strcat(out, temp);
}

void print_vertices(char* out, tVertex vertices) {
    if (!vertices) { sprintf(out, " No vertices exist.\n\n"); return; }
    tVertex vertex = vertices;
    do {
        print_vertex(out, vertex);
        vertex = vertex->next;
    } while (vertex != vertices);
    strcat(out, "\n");
}

// REWORK TO ADD VERTEX CREATION INSIDE
void create_vertex(tVertex vertices, tVertex vertex, int x, int y) {
    vertex->v[X] = x;
    vertex->v[Y] = y;
    vertex->vnum = (vertices) ? vertices->prev->vnum + 1 : 0;
    vertex->ear = FALSE;
}

// ADD
void copy_vertices(tVertex original, tVertex copy) {
    
}

// CREATE A NEAR VERTEX FUNC AND ONLY PLACE VERTICES WIHTIN A CERTAIN DISTANCE, CAN ALSO HELP IMPLEMENT VERTEX HIGHLIGHTING AND EDITING

#endif