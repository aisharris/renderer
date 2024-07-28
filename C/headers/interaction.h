#pragma once

#include "vec.h"

// Forward declaration of BSDF class
class BSDF;

struct Interaction {
    // Position of interaction
    Vector3f p;
    // Normal of the surface at interaction
    Vector3f n;
    // The uv co-ordinates at the intersection point
    Vector2f uv;
    // The viewing direction in local shading frame
    Vector3f wi; 
    // Distance of intersection point from origin of the ray
    float t = 1e30f; 
    // Used for light intersection, holds the radiance emitted by the emitter.
    Vector3f emissiveColor = Vector3f(0.f, 0.f, 0.f);
    // BSDF at the shading point
    BSDF* bsdf;
    // Vectors defining the orthonormal basis
    Vector3f a, b, c;

    bool didIntersect = false;

    //matrix multiplying
    //as rowVector * matrix = transformed rowVector
    //this is why the matrix given in the slides for world to local works as local to world here(they use column vectors)
    //x' = a, y' = b, z' = c

    Vector3f toWorld(Vector3f w) {

        Vector3f result;
        result.x = this->a.x * w.x + this->b.x * w.y + this->c.x * w.z;
        result.y = this->a.y * w.x + this->b.y * w.y + this->c.y * w.z;
        result.z = this->a.z * w.x + this->b.z * w.y + this->c.z * w.z;
        return result;
        //return Vector3f(0, 0, 0);
    }

    Vector3f toLocal(Vector3f w) {

        Vector3f result;
        result.x = Dot(w, this->a);
        result.y = Dot(w, this->b);
        result.z = Dot(w, this->c);
        return result;
        //return Vector3f(0, 0, 0);
    }
};