#ifndef GEOMETRY
#define GEOMETRY

#include <windows.h>
#include "vertex_types.h"
#include "vertex_funcs.h"
#include "draw_util.h"
#include "frame.h"

bool Xor(bool x, bool y) {
    return !x ^ !y;
}

int Area2(tPointi a, tPointi b, tPointi c) {
    return -((b[X] - a[X]) * (c[Y] - a[Y]) - (c[X] - a[X]) * (b[Y] - a[Y])); // Minus since (0,0) is the top left
}

int Area(tPointi a, tPointi b, tPointi c) {
    return Area2(a, b, c) >> 1;
}

int AreaPoly(tVertex vertices) {
    int sum = 0;
    tVertex current = vertices->next;
    do {
        sum += Area(vertices->v, current->v, current->next->v);
        current = current->next;
    } while (current->next != vertices);
    return sum;
}  

bool Left(tPointi a, tPointi b, tPointi c) {
    return Area2(a, b, c) > 0;
}

bool LeftOn(tPointi a, tPointi b, tPointi c) {
    return Area2(a, b, c) >= 0;
}

bool Right(tPointi a, tPointi b, tPointi c) {
    return !LeftOn(a, b, c);
}

bool RightOn(tPointi a, tPointi b, tPointi c) {
    return !Left(a, b, c);
}

bool Collinear(tPointi a, tPointi b, tPointi c) {
    return Area2(a, b, c) == 0;
}

bool Between(tPointi a, tPointi b, tPointi c) {
    if (!Collinear(a, b, c))
        return FALSE;
    
    if (a[X] != b[X]) 
        return ((a[X] <= c[X]) && (c[X] <= b[X])) || ((a[X] >= c[X]) && (c[X] >= b[X]));
    else
        return ((a[Y] <= c[Y]) && (c[Y] <= b[Y])) || ((a[Y] >= c[Y]) && (c[Y] >= b[Y]));
}

bool IntersectProp(tPointi a, tPointi b, tPointi c, tPointi d) {
    if (Collinear(a, b, c) || Collinear(a, b, d) || Collinear(c, d, a) || Collinear(c, d, b))
        return FALSE;
    return Xor(Left(a, b, c), Left(a, b, d)) && Xor(Left(c, d, a), Left(c, d, b));
}

bool Intersect(tPointi a, tPointi b, tPointi c, tPointi d) {
    if (IntersectProp(a, b, c, d)) 
        return TRUE;
    else if (Between(a, b, c) || Between(a, b, d) || Between(c, d, a) || Between(c, d, b))
        return TRUE;
    else return FALSE;
}

bool Diagonalie(tVertex vertices, tVertex a, tVertex b) {
    tVertex current = vertices, next;
    do {
        next = current->next;
        if ((current != a) && (next != a) && (current != b) && (next != b) && Intersect(a->v, b->v, current->v, next->v))
            return FALSE;
        current = next;
    } while (current != vertices);
    return TRUE;
}

bool InCone(tVertex vertices, tVertex a, tVertex b) {
    tVertex a0, a1;
    a1 = a->next;
    a0 = a->prev;
    if (LeftOn(a->v, a1->v, a0->v))
        return Left(a->v, b->v, a0->v) && Left(b->v, a->v, a1->v);
    return !(LeftOn(a->v, b->v, a1->v) && LeftOn(b->v, a->v, a0->v));
}

bool Diagonal(tVertex vertices, tVertex a, tVertex b) {
    return InCone(vertices, a, b) && Diagonalie(vertices, a, b);
}

void EarInit(tVertex vertices) {
    tVertex prev, current, next;
    current = vertices;
    do {
        next = current->next;
        prev = current->prev;
        current->ear = Diagonal(vertices, prev, next);
        current = next;
    } while (current != vertices);
}

// CREATE IS VALID POLYGON FUNC TO CHECK IF NON INTERSECTING

#endif