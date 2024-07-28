#pragma once

#include "scene.h"

struct Integrator {
    Integrator(Scene& scene);

    long long render();

    Scene scene;
    Texture outputImage;
};


Vector3f SampleHemisphereUniform();
Vector3f SampleHemisphereCosine();