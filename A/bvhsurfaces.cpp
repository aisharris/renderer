#include "bvhsurfaces.h"

//first node, used
uint rootNodeIdx = 0, nodesUsed = 1;

void BuildBVH(Scene& scn)
{
    // std::vector<Surface> surfaces = scn.surfaces; //!will this work? i defined surface to be a std::vector in bvh.h

    //create a list of surface indices.
    // uint* surfIndices = (uint *)malloc(scn.surfaces.size()*(sizeof(uint)));

    scn.surfIndices.resize(scn.surfaces.size());

    for(int i =0; i < scn.surfaces.size(); i++)
    {
        scn.surfIndices[i] = i;
    }

    //get number of surfaces in the scene(each has an AABB)
    int no_of_surfaces = scn.surfaces.size();
    BVHNode& root = scn.bvhNode[rootNodeIdx];
    root.firstElem = 0;
    root.elemCount = no_of_surfaces;
    root.leftIdx = 0;
    UpdateNodeBounds( rootNodeIdx, scn.surfaces, scn.surfIndices, scn.bvhNode);
    Subdivide( rootNodeIdx, scn.surfaces, scn.surfIndices, scn.bvhNode );
}

void UpdateNodeBounds( uint rootNodeIdx, std::vector<Surface> &surfaces, std::vector<uint>& surfIndices, std::vector<BVHNode>& bvhNode)
{
    BVHNode& node = bvhNode[rootNodeIdx];
    node.aabbMin = Vector3f(1e30f, 1e30f, 1e30f);
    node.aabbMax = Vector3f(-1e30f, -1e30f, -1e30f);

    uint first = node.firstElem;

    for(int i = 0 ; i < node.elemCount; i++)
    {
        uint index = surfIndices[first + i];
        node.aabbMin.x = std::min(node.aabbMin.x, surfaces[index].aabbMin.x);
        node.aabbMin.y = std::min(node.aabbMin.y, surfaces[index].aabbMin.y);
        node.aabbMin.z = std::min(node.aabbMin.z, surfaces[index].aabbMin.z);
        node.aabbMax.x = std::max(node.aabbMax.x, surfaces[index].aabbMax.x);
        node.aabbMax.y = std::max(node.aabbMax.y, surfaces[index].aabbMax.y);
        node.aabbMax.z = std::max(node.aabbMax.z, surfaces[index].aabbMax.z);
    }
}

void Subdivide( uint rootNodeIdx, std::vector<Surface>& surfaces, std::vector<uint>& surfIndices, std::vector<BVHNode>& bvhNode )
{
    BVHNode& node = bvhNode[rootNodeIdx];
    if (node.elemCount <= 2) return; //!we stop when theres only one surface in the array right?

    //split axis
    Vector3f extent = node.aabbMax - node.aabbMin;
    int axis = 0;
    if (extent.y > extent.x) axis = 1;
    if (extent.z > extent[axis]) axis = 2;
    float splitPos = node.aabbMin[axis] + extent[axis] * 0.5f;

    //partition array
    int i = node.firstElem;
    int j = i + node.elemCount - 1;
    while (i <= j)
    {
        if (surfaces[surfIndices[i]].center[axis] < splitPos)
            i++;
        else
            std::swap(surfIndices[i], surfIndices[j--]);
    }

    //abort split if one of the sides is empty
    int leftCount = i - node.firstElem;
    if (leftCount == 0 || leftCount == node.elemCount) return;

    //create child nodes
    int leftChildIdx = nodesUsed++;
    int rightChildIdx = nodesUsed++;


    bvhNode[leftChildIdx].firstElem = node.firstElem;
    bvhNode[leftChildIdx].elemCount = leftCount;
    bvhNode[rightChildIdx].firstElem = i;
    bvhNode[rightChildIdx].elemCount = node.elemCount - leftCount;

    node.leftIdx = leftChildIdx;
    node.elemCount = 0;
    UpdateNodeBounds( leftChildIdx, surfaces, surfIndices, bvhNode );
    UpdateNodeBounds( rightChildIdx, surfaces, surfIndices, bvhNode );
    // recurse
    Subdivide( leftChildIdx, surfaces, surfIndices, bvhNode );
    Subdivide( rightChildIdx, surfaces, surfIndices, bvhNode );
}

Interaction IntersectBVH(Ray &ray, uint nodeIdx, Scene &scene)
{
    Interaction siFinal;

    BVHNode &node = scene.bvhNode[nodeIdx];

    if(!IntersectAABB(ray, node.aabbMin, node.aabbMax))
        return siFinal; 
    
    if(node.isLeaf()) //no more subsurfaces; only one surface, so do interaction
    {
        for(int i = 0; i < node.elemCount; i++)
        {
            if(IntersectAABB(ray, scene.surfaces[scene.surfIndices[node.firstElem + i]].aabbMin, scene.surfaces[scene.surfIndices[node.firstElem + i]].aabbMax))
            {
                uint index = scene.surfIndices[node.firstElem + i];
                
                Interaction si = scene.surfaces[index].rayIntersect(ray);

                if (si.t <= ray.t && si.didIntersect) 
                {    
                    siFinal = si;
                    ray.t = si.t;
                }

            }

        }
    }
    else
    {
        Interaction si = IntersectBVH(ray, node.leftIdx, scene);
        if (si.t <= ray.t && si.didIntersect) 
        {    
            siFinal = si;
            ray.t = si.t;
        }
        si = IntersectBVH(ray, node.leftIdx + 1, scene);
        if (si.t <= ray.t && si.didIntersect) 
        {    
            siFinal = si;
            ray.t = si.t;
        }
    }

    return siFinal;
}