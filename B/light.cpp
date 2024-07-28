#include "light.h"


std::vector<Light> createLights( nlohmann::json sceneConfig)
{
    std::vector<Light> lights;

    auto res = sceneConfig["directionalLights"];

    //check: add loc
    for (auto& light : res) 
    {
        Light l;
        l.type = LightType::DIRECTIONAL_LIGHT;
        l.direction = Vector3f(light["direction"][0], light["direction"][1], light["direction"][2]);
        l.radiance = Vector3f(light["radiance"][0], light["radiance"][1], light["radiance"][2]);
        lights.push_back(l);
    }

    res = sceneConfig["pointLights"];

    //check:
    for (auto& light : res) 
    {
        Light l;
        l.type = LightType::POINT_LIGHT;
        l.location = Vector3f(light["location"][0], light["location"][1], light["location"][2]);
        l.radiance = Vector3f(light["radiance"][0], light["radiance"][1], light["radiance"][2]);
        lights.push_back(l);
    }

    return lights;
}