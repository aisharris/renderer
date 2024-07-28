// void UpdateNodeBounds can be reused
// void Subdivide can be reused

#include "surface.h"
#include "bvhsurfaces.h"

//first node, used
uint rootnodeIndex = 0, NodesUsed = 1;

void BuildTriBVH(Surface& surface)
{
    rootnodeIndex = 0, NodesUsed = 1;
    // std::vector<Surface> surfaces = scn.surfaces; //!will this work? i defined surface to be a std::vector in bvh.h

    //create a list of surface indices.
    // uint* surfIndices = (uint *)malloc(scn.surfaces.size()*(sizeof(uint)));

    surface.triIndices.resize(surface.triangles.size());    

    for(int i =0; i < surface.triangles.size(); i++)
    {
        surface.triIndices[i] = i;
    }

    //get number of surfaces in the scene(each has an AABB)
    int no_of_triangles = surface.triangles.size();
    BVHNode& root = surface.bvhNode[rootnodeIndex];
    root.firstElem = 0;
    root.elemCount = no_of_triangles;
    root.leftIdx = 0;
    TriUpdateNodeBounds( rootnodeIndex, surface.triangles , surface.triIndices, surface.bvhNode);
    TriSubdivide( rootnodeIndex, surface.triangles, surface.triIndices, surface.bvhNode );
}

void TriUpdateNodeBounds( uint rootnodeIndex, std::vector<Tri>& triangles, std::vector<uint>& triIndices, std::vector<BVHNode>& bvhNode )
{
    BVHNode& node = bvhNode[rootnodeIndex];
    node.aabbMin = Vector3f(1e30f, 1e30f, 1e30f);
    node.aabbMax = Vector3f(-1e30f, -1e30f, -1e30f);

    uint first = node.firstElem;

    for(int i = 0 ; i < node.elemCount; i++)
    {
        uint index = triIndices[first + i];
        node.aabbMin.x = std::min(node.aabbMin.x, triangles[index].vertexone.x);
        node.aabbMin.x = std::min(node.aabbMin.x, triangles[index].vertextwo.x);
        node.aabbMin.x = std::min(node.aabbMin.x, triangles[index].vertexthree.x);
        node.aabbMin.y = std::min(node.aabbMin.y, triangles[index].vertexone.y);
        node.aabbMin.y = std::min(node.aabbMin.y, triangles[index].vertextwo.y);
        node.aabbMin.y = std::min(node.aabbMin.y, triangles[index].vertexthree.y);
        node.aabbMin.z = std::min(node.aabbMin.z, triangles[index].vertexone.z);
        node.aabbMin.z = std::min(node.aabbMin.z, triangles[index].vertextwo.z);
        node.aabbMin.z = std::min(node.aabbMin.z, triangles[index].vertexthree.z);

        node.aabbMax.x = std::max(node.aabbMax.x, triangles[index].vertexone.x);
        node.aabbMax.x = std::max(node.aabbMax.x, triangles[index].vertextwo.x);
        node.aabbMax.x = std::max(node.aabbMax.x, triangles[index].vertexthree.x);
        node.aabbMax.y = std::max(node.aabbMax.y, triangles[index].vertexone.y);
        node.aabbMax.y = std::max(node.aabbMax.y, triangles[index].vertextwo.y);
        node.aabbMax.y = std::max(node.aabbMax.y, triangles[index].vertexthree.y);
        node.aabbMax.z = std::max(node.aabbMax.z, triangles[index].vertexone.z);
        node.aabbMax.z = std::max(node.aabbMax.z, triangles[index].vertextwo.z);
        node.aabbMax.z = std::max(node.aabbMax.z, triangles[index].vertexthree.z);
    }
}

void TriSubdivide( uint rootnodeIndex, std::vector<Tri>& triangles, std::vector<uint>& triIndices, std::vector<BVHNode>& bvhNode )
{
    BVHNode& node = bvhNode[rootnodeIndex];
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
        if (triangles[triIndices[i]].centroid[axis] < splitPos)
            i++;
        else
            std::swap(triIndices[i], triIndices[j--]);
    }

    //abort split if one of the sides is empty
    int leftCount = i - node.firstElem;
    if (leftCount == 0 || leftCount == node.elemCount) return;

    //create child nodes
    int leftChildIdx = NodesUsed++;
    int rightChildIdx = NodesUsed++;


    bvhNode[leftChildIdx].firstElem = node.firstElem;
    bvhNode[leftChildIdx].elemCount = leftCount;
    bvhNode[rightChildIdx].firstElem = i;
    bvhNode[rightChildIdx].elemCount = node.elemCount - leftCount;

    node.leftIdx = leftChildIdx;
    node.elemCount = 0;
    TriUpdateNodeBounds( leftChildIdx, triangles, triIndices, bvhNode );
    TriUpdateNodeBounds( rightChildIdx, triangles, triIndices, bvhNode );
    // recurse
    TriSubdivide( leftChildIdx, triangles, triIndices, bvhNode );
    TriSubdivide( rightChildIdx, triangles, triIndices, bvhNode );
}

Interaction IntersectHigherBVH(Ray &ray, uint nodeIdx, Scene &scene)
{
    Interaction siFinal;

    BVHNode &node = scene.bvhNode[nodeIdx];

    if(!IntersectAABB(ray, node.aabbMin, node.aabbMax))
        return siFinal; 
    
    if(node.isLeaf()) //no more subsurfaces; only one surface, so do interaction
    {
        for(int i = 0; i < node.elemCount; i++)
        {
            Vector3f min, max;
            min = scene.surfaces[scene.surfIndices[node.firstElem + i]].aabbMin;
            max = scene.surfaces[scene.surfIndices[node.firstElem + i]].aabbMax;

            if(IntersectAABB(ray, min, max))
            {
                //check triangle tree intersection
                Interaction si = IntersectLowerBVH(scene.surfaces[scene.surfIndices[node.firstElem + i]], ray, 0, scene);

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
        Interaction si = IntersectHigherBVH(ray, node.leftIdx, scene);
        if (si.t <= ray.t && si.didIntersect) 
        {    
            siFinal = si;
            ray.t = si.t;
        }
        si = IntersectHigherBVH(ray, node.leftIdx + 1, scene);
        if (si.t <= ray.t && si.didIntersect) 
        {    
            siFinal = si;
            ray.t = si.t;
        }
    }

    return siFinal;
}

Interaction IntersectLowerBVH(Surface &surface, Ray &ray, uint nodeIdx, Scene &scene)
{
    Interaction siFinal;
    //iterate through surface's tree and check intersection with triangles
    BVHNode &node = surface.bvhNode[nodeIdx];

    if(!IntersectAABB(ray, node.aabbMin, node.aabbMax))
        return siFinal; 

    if(node.isLeaf()) //no more subtriangles; only one triangle, so do interaction
    {
        for(int i = 0; i < node.elemCount; i++)
        {
            Vector3f vone, vtwo, vthree, vnormal;
            vone = surface.triangles[surface.triIndices[node.firstElem + i]].vertexone;
            vtwo = surface.triangles[surface.triIndices[node.firstElem + i]].vertextwo;
            vthree = surface.triangles[surface.triIndices[node.firstElem + i]].vertexthree;
            vnormal = surface.triangles[surface.triIndices[node.firstElem + i]].normal;

            Interaction si = surface.rayTriangleIntersect(ray, vone, vtwo, vthree, vnormal);
            
            if (si.t <= ray.t && si.didIntersect) 
            {    
                siFinal = si;
                ray.t = si.t;
            }
        }
    }
    else
    {
        Interaction si = IntersectLowerBVH(surface, ray, node.leftIdx, scene);
        if (si.t <= ray.t && si.didIntersect) 
        {    
            siFinal = si;
            ray.t = si.t;
        }
        si = IntersectLowerBVH(surface, ray, node.leftIdx + 1, scene);
        if (si.t <= ray.t && si.didIntersect) 
        {    
            siFinal = si;
            ray.t = si.t;
        }
    }

    return siFinal;
}