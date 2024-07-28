// #include "scene.h" //its there in bvhsurfaces
#include "bvhsurfaces.h"

Scene::Scene(std::string sceneDirectory, std::string sceneJson)
{
    nlohmann::json sceneConfig;
    try {
        sceneConfig = nlohmann::json::parse(sceneJson);
    }
    catch (std::runtime_error e) {
        std::cerr << "Could not parse json." << std::endl;
        exit(1);
    }

    
    //first node, used
    uint rootNodeIdx = 0, nodesUsed = 1;
    this->parse(sceneDirectory, sceneConfig);
    
    bvhNode.resize(this->surfaces.size() * 2 - 1);
    BuildBVH(*this);
}

Scene::Scene(std::string pathToJson)
{
    std::string sceneDirectory;

#ifdef _WIN32
    const size_t last_slash_idx = pathToJson.rfind('\\');
#else
    const size_t last_slash_idx = pathToJson.rfind('/');
#endif

    if (std::string::npos != last_slash_idx) {
        sceneDirectory = pathToJson.substr(0, last_slash_idx);
    }

    nlohmann::json sceneConfig;
    try {
        std::ifstream sceneStream(pathToJson.c_str());
        sceneStream >> sceneConfig;
    }
    catch (std::runtime_error e) {
        std::cerr << "Could not load scene .json file." << std::endl;
        exit(1);
    }

    //first node, used
    uint rootNodeIdx = 0, nodesUsed = 1;
    
    this->parse(sceneDirectory, sceneConfig);

    bvhNode.resize(this->surfaces.size() * 2 - 1);
    BuildBVH(*this);
    
}

void Scene::parse(std::string sceneDirectory, nlohmann::json sceneConfig)
{
    // Output
    try {
        auto res = sceneConfig["output"]["resolution"];
        this->imageResolution = Vector2i(res[0], res[1]);
    }
    catch (nlohmann::json::exception e) {
        std::cerr << "\"output\" field with resolution, filename & spp should be defined in the scene file." << std::endl;
        exit(1);
    }

    // Cameras
    try {
        auto cam = sceneConfig["camera"];

        this->camera = Camera(
            Vector3f(cam["from"][0], cam["from"][1], cam["from"][2]),
            Vector3f(cam["to"][0], cam["to"][1], cam["to"][2]),
            Vector3f(cam["up"][0], cam["up"][1], cam["up"][2]),
            float(cam["fieldOfView"]),
            this->imageResolution
        );
    }
    catch (nlohmann::json::exception e) {
        std::cerr << "No camera(s) defined. Atleast one camera should be defined." << std::endl;
        exit(1);
    }

    // Surface
    try {
        auto surfacePaths = sceneConfig["surface"];

        uint32_t surfaceIdx = 0;
        for (std::string surfacePath : surfacePaths) {
            surfacePath = sceneDirectory + "/" + surfacePath;

            auto surf = createSurfaces(surfacePath, /*isLight=*/false, /*idx=*/surfaceIdx);
            this->surfaces.insert(this->surfaces.end(), surf.begin(), surf.end());

            surfaceIdx = surfaceIdx + surf.size();
        }
    }
    catch (nlohmann::json::exception e) {
        std::cout << "No surfaces defined." << std::endl;
    }
}

Interaction Scene::rayIntersect(Ray& ray)
{
    if(this->mode == 0)
    {
        Interaction siFinal;

        for (auto& surface : this->surfaces) {
            
            Interaction si = surface.rayIntersect(ray);
            if (si.t <= ray.t) {    
                siFinal = si;
                ray.t = si.t;
            }
        }

        return siFinal;

    }

    if(this->mode == 1)
    {
        Interaction siFinal;

        for (auto& surface : this->surfaces) {

            if(IntersectAABB(ray, surface.aabbMin, surface.aabbMax))
            {
                Interaction si = surface.rayIntersect(ray);
                if (si.t <= ray.t) {    
                    siFinal = si;
                    ray.t = si.t;
                }
            }
            
        }

        return siFinal;

    }

    if(this->mode == 2)
    {
        Interaction siFinal;
        
        siFinal = IntersectBVH(ray, 0, *this);

        return siFinal;

    }

    if(this->mode == 3)
    {
        Interaction siFinal;
        
        siFinal = IntersectHigherBVH(ray, 0, *this);

        return siFinal;
    }

}