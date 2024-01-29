#include "utils/Vec3.hpp"

class Voxel {
    Vec3 pos;
    
    // Empty Constructor
    Voxel() {
        // Initialize at origin
        pos = Vec3();
    }
    Voxel(Vec3& _position) {
        pos = _position;
    }
};