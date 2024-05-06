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
    int res = 300;
    int resY;
    int maxBounce = 2;
    int numThreads = std::thread::hardware_concurrency();

    if (argc > 1) res = atoi(argv[1]);
    if (argc > 2) resY = atoi(argv[2]);
    if (argc > 3) maxBounce = atoi(argv[3]);
    if (argc > 4) numThreads = atoi(argv[4]);

    if (resY <= 0 || resY > 10000) resY = res;

    std::cout << "res: " << res << "x" << resY << std::endl;
    std::cout << "maxBounce: " << maxBounce << std::endl;
    std::cout << "numThreads: " << numThreads << std::endl;

    VoxelEngine* voxelEngine = new VoxelEngine();
    SVO* svo = new SVO();
    _Renderer renderer = _Renderer(voxelEngine, res, resY, svo, maxBounce);

    // loadObj("../models/obj_files/Low_Poly_Sportcar.obj");

    // if(voxelEngine->ModelLoader.loadVoxModel("../models/vox_files/weird.vox")) {
    //     voxelEngine->chunks[0]->dataChunk.print(0);
    //     renderer.render(numThreads);
    // }
    renderer.render(numThreads);
}