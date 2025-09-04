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
    bool useBVH = true;
    std::string model_file = "../models/obj_files/Low_Poly_Sportcar.obj";

    if (argc > 1) res = atoi(argv[1]);
    if (argc > 2) resY = atoi(argv[2]);
    if (argc > 3) maxBounce = atoi(argv[3]);
    if (argc > 4) numThreads = atoi(argv[4]);
    if (argc > 5) useBVH = atoi(argv[5]);
    if (argc > 6) model_file = atoi(argv[6]);

    if (resY <= 0 || resY > 10000) resY = res;

    VoxelEngine* voxelEngine = new VoxelEngine();
    SVO* svo = new SVO();

    std::vector<std::vector<Face>> shapes = loadObj(model_file);
    std::vector<Face> faces;
    
    // Calculate center of model
    for (int shape_i = 0; shape_i < shapes.size(); shape_i++) {
        for (int face_i = 0; face_i < shapes[shape_i].size(); face_i++) {
            faces.push_back(shapes[shape_i][face_i]);
        }
    }
    Vec3 sum_pos;
    sum_pos = sum_pos / faces.size();
    std::cout << sum_pos << std::endl;
    
    // Build the BVH
    BVH* bvh = new BVH(faces);
    bvh->BuildBVH(faces);

    std::cout << "Tri count: " << faces.size() << std::endl;

    // Initialize renderer (performs all rendering tasks)
    _Renderer renderer = _Renderer(voxelEngine, res, resY, svo, bvh, maxBounce, true, useBVH);

    auto start = std::chrono::system_clock::now();

    // Main render function
    renderer.render(numThreads);
    
    // Display pertinent information
    auto end = std::chrono::system_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(end - start);
    std::cout << "res: " << res << "x" << resY << std::endl;
    std::cout << "maxBounce: " << maxBounce << std::endl;
    std::cout << "numThreads: " << numThreads << std::endl;
    std::cout << "useBVH: " << (useBVH ? "True" : "False") << std::endl;
    std::cout << "Finished in " << elapsed.count() << " seconds!" << '\n';
}