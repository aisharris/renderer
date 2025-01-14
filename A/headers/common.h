#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <chrono>
#include <cmath>

#include "vec.h"

#include "json/include/nlohmann/json.hpp"

#ifndef M_PI
#define M_PI 3.14159263f
#endif

struct Ray {
    Vector3f o, d;
    float t = 1e30f;
    float tmax = 1e30f;


    Ray(Vector3f origin, Vector3f direction, float t = 1e30f, float tmax = 1e30f)
        : o(origin), d(direction), t(t), tmax(tmax) {};
};

struct Interaction {
    Vector3f p, n;
    float t = 1e30f;
    bool didIntersect = false;
};

bool IntersectAABB(Ray ray, Vector3f bmin, Vector3f bmax );