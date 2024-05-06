#include "../utils/Vec3.hpp"
#include <vector>

class Face {
public:
    Vec3 center;
    Vec3 vertices[3];
    Vec3 normal;
    Vec3 color;

    Face() {
        
    }

    Face(Vec3 _vertices[3], Vec3 _normal, Vec3 _color) {
        center = (_vertices[0] + _vertices[1] + _vertices[2]) / 3;
        for (int i = 0; i < 3; i++) {
            vertices[i] = _vertices[i];
        }
        normal = _normal;
        color = _color;
    }
};