
#include "common.h"

struct BVHNode
{
    Vector3f aabbMin, aabbMax;
    uint leftIdx, firstElem;    
    int elemCount;
    bool isLeaf() { return elemCount > 0; }
};
