#pragma once
#include "../utils/Vec3.hpp"

struct Ray {
public:
    Vec3 origin;
    Vec3 direction;
    Vec3 invDir;
    float t;
    
    Vec3 at() {
        return (direction * t) + origin;
    }

    Vec3 at(float _t) {
        return (direction * _t) + origin;
    };

    void invert() {
        invDir.x = 1.0f / direction.x;
        invDir.y = 1.0f / direction.y;
        invDir.z = 1.0f / direction.z;
    }
};