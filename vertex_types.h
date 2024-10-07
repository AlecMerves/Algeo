#ifndef VERTEX_T
#define VERTEX_T

#define X 0
#define Y 1

#define DIM 2
typedef int tPointi[DIM];

typedef struct tVertexStructure tsVertex;
typedef tsVertex *tVertex;
struct tVertexStructure {
    int         vnum;
    tPointi     v;
    bool        ear;
    tVertex     next, prev;
};

#endif