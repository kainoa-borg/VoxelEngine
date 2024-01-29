#pragma once
#include <vector>
#include "../utils/Vec3.hpp"

namespace Shapes {
void CreatePlane(Vec3 center, int size, Vec3 color, bool isEmissive, std::vector< std::tuple<Vec3, Vec3, bool> > &voxelPositions) {
    // **Plane**
    for (int i = center.x - size/2; i < center.x + size/2; i++) {
        for (int j = center.z - size/2; j < center.z + size/2; j++) {
            Vec3 voxelPos = Vec3(i, center.y, j);
            voxelPositions.push_back(std::tuple<Vec3, Vec3, bool>(voxelPos, color, false));
        }
    }
}

void CreateCube(Vec3 center, int radius, Vec3 color, bool isEmissive, std::vector< std::tuple<Vec3, Vec3, bool> > &voxelPositions) {
    for (int i = center.x - radius; i < center.x + radius; i++) {
        for (int j = center.y - radius; j < center.y + radius; j++) {
            for (int k = center.z - radius; k < center.z + radius; k++) {
                voxelPositions.push_back(std::tuple<Vec3, Vec3, bool>(Vec3(i,j,k), color, isEmissive));
            }
        }
    }
}

}
