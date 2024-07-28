#pragma once

#include "common.h"
#include "texture.h"
#include "bvh.h"

struct Tri {
    Vector3f vertexone;
    Vector3f vertextwo;
    Vector3f vertexthree;
    Vector3f centroid;
    Vector3f normal;
    // Vector3f aabbMin, aabbMax;
};

struct Surface {
    std::vector<Vector3f> vertices, normals;
    std::vector<Vector3i> indices;
    std::vector<Vector2f> uvs;
    Vector3f aabbMin, aabbMax;
    Vector3f center;
    std::vector<Tri> triangles;
    std::vector<uint> triIndices;
    std::vector<BVHNode> bvhNode;

    bool isLight;
    uint32_t shapeIdx;

    Vector3f diffuse;
    float alpha;

    Texture diffuseTexture, alphaTexture;

    Interaction rayPlaneIntersect(Ray ray, Vector3f p, Vector3f n);
    Interaction rayTriangleIntersect(Ray ray, Vector3f v1, Vector3f v2, Vector3f v3, Vector3f n);
    Interaction rayIntersect(Ray ray);

private:
    bool hasDiffuseTexture();
    bool hasAlphaTexture();
};

std::vector<Surface> createSurfaces(std::string pathToObj, bool isLight, uint32_t shapeIdx);

