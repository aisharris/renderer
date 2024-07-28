#pragma once

#include "scene.h" //contains surace and camera h files
#include "common.h"



#define N 64

//make the surface list  global


void UpdateNodeBounds( uint rootNodeIdx, std::vector<Surface>& surfaces, std::vector<uint>& surfIndices, std::vector<BVHNode>& bvhNode );
void TriUpdateNodeBounds( uint rootNodeIdx, std::vector<Tri>& triangles, std::vector<uint>& triIndices, std::vector<BVHNode>& bvhNode );

void BuildBVH(Scene& scn);
void BuildTriBVH(Surface& surface);

void Subdivide( uint rootNodeIdx, std::vector<Surface>& surfaces, std::vector<uint>& surfIndices, std::vector<BVHNode>& bvhNode );
void TriSubdivide( uint rootNodeIdx, std::vector<Tri>& triangles, std::vector<uint>& triIndices, std::vector<BVHNode>& bvhNode );

Interaction IntersectBVH(Ray &ray, uint nodeIdx, Scene &scene);
Interaction IntersectLowerBVH(Surface &surface, Ray &ray, uint nodeIdx, Scene &scene);
Interaction IntersectHigherBVH(Ray &ray, uint nodeIdx, Scene &scene);
