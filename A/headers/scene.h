#pragma once
#include "camera.h"
#include "surface.h"


struct Scene {
    std::vector<Surface> surfaces;
    uint* surfaceIndices;
    Camera camera;
    Vector2i imageResolution;
    std::vector<BVHNode> bvhNode;
    std::vector<uint> surfIndices;
    uint mode;

    Scene() {};
    Scene(std::string sceneDirectory, std::string sceneJson);
    Scene(std::string pathToJson);
    
    void parse(std::string sceneDirectory, nlohmann::json sceneConfig);

    Interaction rayIntersect(Ray& ray);
};