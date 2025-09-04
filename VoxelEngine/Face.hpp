#pragma once
#include "../utils/Vec3.hpp"
#include <vector>

class Face {
public:
    Vec3 center;
    std::vector<Vec3> vertices;
    Vec3 normal;
    Vec3 color;

    Face() {
        
    }

    Face(std::vector<Vec3> _vertices, Vec3 _normal, Vec3 _color) {
        center = (_vertices[0] + _vertices[1] + _vertices[2]) / 3;
        vertices = _vertices;
        normal = _normal;
        color = Vec3(1, 1, 1);
    }
};