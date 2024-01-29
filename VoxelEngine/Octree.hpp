#include <Voxel.hpp>
#include <cstdint>

#define TopLeftFront 0
#define TopRightFront 1
#define BottomRightFront 2
#define BottomLeftFront 3
#define TopLeftBottom 4
#define TopRightBottom 5
#define BottomRightBack 6
#define BottomLeftBack 7

using namespace std;

// Octree
class Octree {
public:
    Octree* parent;
    Octree* children[8];
    uint8_t activeChildren;
    Voxel* voxelData;


    Octree(Octree* _parent, Octree* _children[8]) {
        parent = _parent;
        for (int i = 0; i < 8; i++) {
            // If this child exists
            if(children[i] != nullptr) {
                // Mark it as active in activeChildren mask
                activeChildren += 1 << i;
            }
            children[i] = _children[i];
        }
    }
};