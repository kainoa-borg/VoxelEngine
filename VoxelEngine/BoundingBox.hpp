#include "utils/Vec3.hpp"
#include <iostream>

#define InvalidBoundaries 505

struct BoundingBox {
    Vec3 topLeftFront;
    Vec3 bottomRightBack;

    BoundingBox(Vec3 _topLeftFront, Vec3 _bottomRightBack) {
        Vec3 difference = _bottomRightBack - _topLeftFront;
        if (difference.x < 0 || difference.y < 0 || difference.z < 0) {
            std::cout << "Invalid Boundaries!\n";
            throw InvalidBoundaries;
        }
    }
};