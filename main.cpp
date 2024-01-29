#include "utils/SVO.hpp"
#include "VoxelEngine/Renderer.hpp"

// #include <GLFW/glfw3.h>
// #include <GLUT/glut.h>
// #include <GL/gl.h>
#include <string.h>
#include <fstream>
#include <sstream>

// #include "VoxelEngine/Debug.hpp"

int main(int argc, char** argv) {
    int res = 150;
    int maxBounce = 2;
    if (argc > 2) {
        res = atoi(argv[1]);
        maxBounce = atoi(argv[2]);
    }

    VoxelEngine* voxelEngine = new VoxelEngine();
    SVO* svo = new SVO();
    _Renderer renderer = _Renderer(voxelEngine, 300, 300, svo, maxBounce);

    if(voxelEngine->ModelLoader.loadVoxModel("../models/vox_files/untitled.vox")) {
        voxelEngine->chunks[0]->dataChunk.print(0);
        renderer.render();
    }
}