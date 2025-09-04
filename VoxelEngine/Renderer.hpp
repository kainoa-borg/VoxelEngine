#ifndef RENDERER_HPP
#define RENDERER_HPP

#include "../utils/BVH.hpp"
#include "../utils/SVO.hpp"
#include "../utils/SVOBuffer.hpp"
#include "VoxelEngine.hpp"
#include "Lighting.hpp"
#include "Shapes.hpp"
#include "../Threading/Threads.hpp"

#include "../utils/loadingtest.hpp"

#include "tuple"
#include "iostream"
#include "time.h"
#include "thread"

class _Renderer {
private:

    BmpHeader bmpHeader;
    BmpInfoHeader bmpInfoHeader;
    std::vector< std::vector<RGB> > pixels;
    iVec2 resolution;
    Vec3 cameraPos;
    Vec3 cameraUp;
    Vec3 cameraForward;
    VoxelEngine* engine;
    SVO* svo;
    BVH* bvh;
    Camera camera = Camera(GlobalCamera::gPos, GlobalCamera::gUp, GlobalCamera::gForward, iVec2(GlobalCamera::gRes, GlobalCamera::gRes), GlobalCamera::gFov);
    std::vector<Vec3> lights;
    int maxBounce;
    bool isPoly;
    bool useBVH;

public:
    _Renderer() {};
    _Renderer(VoxelEngine* _engine) : engine(_engine) {};
    _Renderer(VoxelEngine* _engine, int resX, int resY, SVO* _svo, BVH* _bvh, int _maxBounce, bool _isPoly, bool _useBVH) : 
        engine(_engine), 
        resolution(iVec2(resX, resY)), 
        bmpInfoHeader(BmpInfoHeader(resX, resY)), 
        bmpHeader(BmpHeader(iVec2(resX, resY))),
        maxBounce(_maxBounce),
        svo(_svo),
        bvh(_bvh),
        isPoly(_isPoly),
        useBVH(_useBVH) {
            if (isPoly == false) isPoly = true;
            pixels.resize(resY);
            for (int i = 0; i < resY; i++) {
                pixels[i].resize(resX);
            }
            camera = Camera(GlobalCamera::gPos, GlobalCamera::gUp, GlobalCamera::gForward, resolution, GlobalCamera::gFov);
    };

    Vec3 traceRay(Ray &r, std::vector<Vec3> &lights, int depth, int maxDepth, std::mt19937& generator) {
        if (depth > maxDepth) {
            return Vec3(0,0,0);
        }
        if (isPoly && !bvh) {
            int foo = 0;;
        }
        // Take this ray from the camera
        Vec3 minPos;
        Vec3 maxPos;
        if (!isPoly) {
            minPos = svo->getMin();
            maxPos = svo->getMax();
        }
        float vistMin;
        float vistMax;
        bool visDidHit;
        if (isPoly) {
            visDidHit = Lighting::iRayBox(r, bvh->bvhNodes[0].aabbMin, bvh->bvhNodes[0].aabbMax, vistMin, vistMax);
        }
        else {
            visDidHit = Lighting::iRayBox(r, minPos, maxPos, vistMin, vistMax);
        }
        // Get the octant it entered in
        float directtMin;
        float directtMax;
        bool voxDidHit = false;
        Vec3 goalPoint = Vec3(0,0,0);
        Vec3 pixColor = Vec3(0,0,0);
        Vec3 directContrib = Vec3(0,0,0);
        Vec3 indirectContrib = Vec3(0,0,0);

        int hitCount = 1;

        Vec3 visSample = Vec3(0,0,0);

        bool isEmissive = false;

        if (visDidHit) {
            // **Compute directContrib**
            Vec3 albedo = Vec3(0,0,0);
            Vec3 hitNormal = Vec3(0,0,0);
            if (isPoly) {
                // r.t = vistMin <= 0 ? vistMin : vistMax;
                r.t = 0;
                vistMin = INFINITY;
                if (useBVH) bvh->IntersectBVH(voxDidHit, r, bvh->rootNodeIdx, albedo, isEmissive, hitNormal, vistMin, vistMax);
                else voxDidHit = bvh->dumbTrace(r, albedo, isEmissive, vistMin, vistMax, hitNormal);
            }
            else {
                voxDidHit = svo->betterTraceRay(r, goalPoint, albedo, isEmissive, hitNormal, vistMin, vistMax);
            }
            if (voxDidHit) {
                if (false) return hitNormal.normalized();
                // Cast shadow ray to determine directSample
                Vec3 hitPosition = r.origin + r.direction * vistMin;
                for (int i = 0; i < lights.size(); i++) {
                    Vec3 shadowSample;
                    Ray sr;
                    sr.origin = hitPosition;
                    sr.direction = lights[i] - sr.origin;
                    // Vec3 lightDir = (sr.origin - lights[i]).normalized();
                    Vec3 lightDir = (sr.origin - lights[i]).normalized();
                    sr.direction.normalize();
                    sr.origin = sr.origin + hitNormal * (0.001f);
                    // sr.invert();
                    Vec3 shadowNormal;
                    float stMin = 0;
                    float stMax = INFINITY;
                    bool shadowDidHit = false;
                    if (isPoly) {
                        r.t = 0;
                        stMin = INFINITY;
                        if (useBVH) bvh->IntersectBVH(shadowDidHit, sr, bvh->rootNodeIdx, shadowSample, isEmissive, shadowNormal, stMin, stMax);
                        else shadowDidHit = bvh->dumbTrace(sr, shadowSample, isEmissive, stMin, stMax, shadowNormal);
                    }
                    else {
                        shadowDidHit = svo->betterTraceRay(sr, lights[i], shadowSample, isEmissive, shadowNormal, stMin, stMax);
                    }
                    if (shadowDidHit) {
                        directContrib += Vec3(0,0,0);
                    }
                    else {
                        directContrib += albedo * fmax(0.0, hitNormal.dot(-lightDir)) * !shadowDidHit;
                    }
                }
                directContrib = directContrib / lights.size();
                // **Computed directContrib**
                // **Compute indirectContrib**
                // Inititalize random generator
                int numSamples = 1;
                Vec3 Nt, Nb;
                hitNormal.createCoordinateSystem(Nt, Nb);

                std::uniform_real_distribution<float> distribution(0,1);
                Vec3 lastDirection;

                if (depth+1 <= maxDepth) {
                    for (int i = 0; i < numSamples; i++) {
                        // Compute indirectSample
                        Vec3 indirectSample;
                        Ray iR;
                        float cosTheta;
                        Vec3 direction;
                        direction = Lighting::generateRandomDirection(generator, distribution, hitNormal, Nb, Nt, cosTheta);
                        iR.origin = hitPosition + hitNormal * 0.001f;
                        iR.direction = direction;
                        iR.invert();
                        // Cast ray for indirectSample
                        indirectSample = traceRay(iR, lights, depth+1, maxDepth, generator);
                        indirectContrib += indirectSample * cosTheta;
                    }
                    // End indirect sample loop
                    indirectContrib = indirectContrib / numSamples;
                    pixColor = (directContrib / M_PI + indirectContrib * 2);
                }
                else {
                    pixColor = directContrib;
                }
                
                
            }
            // End if voxelHit

        }
        // End if visHit

        return pixColor;
    }

    void renderBatch(int startIndex, int batchSize, std::vector<Vec3> &lights, iVec2 &resolution, Camera &camera, int &maxBounce, unsigned long long seed) {
        int endIndex = batchSize + startIndex;
        mt19937 generator(seed);
        mt19937 pixelGenerator(seed);
        std::uniform_real_distribution<float> distribution(0,1);
        for (int heightIndex = startIndex; heightIndex < endIndex; heightIndex++) {
            for (int widthIndex = 0; widthIndex < resolution.x; widthIndex++) {
                // unsigned char val = 255 * (float)widthIndex/resolution.x;
                unsigned char val = 255;
                Ray r;

                float offsetX = distribution(generator) - .5;
                float offsetY = distribution(generator) - .5;
                camera.constructRay(r, widthIndex + offsetX, heightIndex + offsetY);

                Vec3 totalColor = Vec3(0,0,0);

                int numSamples = 16;

                for (int i = 0; i < numSamples; i++) {
                    Vec3 hitColor = traceRay(r, lights, 0, maxBounce, generator);
                    totalColor = totalColor + hitColor;
                }
                
                totalColor = totalColor / numSamples;

                totalColor = Vec3(fmin(totalColor.x,1.0f), fmin(totalColor.y,1.0f), fmin(totalColor.z,1.0f));

                RGB tempPix = RGB(totalColor.x, totalColor.y, totalColor.z);               

                Vec3 tempVec = Vec3(tempPix.r, tempPix.g, tempPix.b);

                pixels[heightIndex][widthIndex] = RGB(totalColor.x * val, totalColor.y * val, totalColor.z * val);
                
                // pixels[heightIndex][widthIndex] = RGB((unsigned char) (val * r.direction.x), (unsigned char) (val * r.direction.y), (unsigned char) (val * r.direction.z));
                
                // std::cout << "For Pixel " << "[" << widthIndex << ", " << heightIndex << "]: " << r.direction << "\n";
                // traceSVORayBool(r);
            } 
            std::cout << "Row " << startIndex / batchSize << " is done!\n";
            // if (heightIndex % percent == 0) {
            //     std::cout << "Complete: " << percentCount << "%" << "\n";
            //     percentCount += 1;
            // } 
        }
    }

    void renderCPU(int numThreads) {
        int percent = resolution.y / 100;
        int percentCount = 0;

        // Break resolution height into batches for each core
        // Assume 7 cores no hyper-threading -> 7 max threads
        // 6 threads handle equally sized batches, last thread handles batch size + remainder
        // 1000 / 7 = 142 --- 1000 - (6*142) = 148
        // 6 threads handle 142 rows and main thread handles last 148 rows
        // Formula: batchSize = resolution.y / 7, remainderBatch = batchSize + resolution.y % 7
        // Formula: batchStartIndex = 142 * threadId -> loop from batchStartIndex until batchStartIndex + batchSize
        
        int threadCount = resolution.y;
        int batchSize = resolution.y / threadCount;
        int remainderBatchStart = batchSize * (threadCount - 1);
        int remainderBatchSize = batchSize + resolution.y % threadCount;
        std::vector<std::thread> threadVec;
        Manager m(numThreads);

        // Create threads
        for (int tId = 0; tId < threadCount-1; tId++) {
            int startIndex = batchSize * tId;
            // std::cout << "Beginning batch " << tId << "/" << threadCount << std::endl;
            unsigned long long thisSeed = std::random_device()();
            m.addTask([this, startIndex, batchSize, thisSeed] { renderBatch(startIndex, batchSize, lights, resolution, camera, maxBounce, thisSeed); });
            // std::thread thisThread([this, startIndex, batchSize, &camera, &generator, &distribution, &maxBounce] { renderBatch(startIndex, batchSize, resolution, camera, generator, distribution, maxBounce); });
            // threadVec.push_back(std::move(thisThread));
        }
        unsigned long long thisSeed = std::random_device()();
        m.addTask([this, remainderBatchStart, remainderBatchSize, thisSeed] { renderBatch(remainderBatchStart, remainderBatchSize, lights, resolution, camera, maxBounce, thisSeed); });


        m.finishWork();

        svo->clear();

        std::cout << sizeof(BmpHeader) << std::endl;
        std::cout << sizeof(BmpInfoHeader) << std::endl;
        std::cout << sizeof(RGB) << std::endl;
        std::cout << bmpHeader.sizeOfBitmapFile << std::endl;

        std::string fileName = "frames/frame";
        fileName.append("0");
        fileName.append(".bmp");

        exportBmp(fileName, bmpHeader, bmpInfoHeader, resolution, pixels);
    }

    void renderPolyInit() {
        lights.push_back(Vec3(0, 600, 0));
    }

    void renderVoxInit() {
        // Container for voxels
        std::vector< std::tuple<Vec3, Vec3, bool> > voxels;

        // **Load Model**
        if (engine->chunks.size() <= 0 || engine->chunks[0] == nullptr) {return;}
        int numModelVoxels = engine->chunks[0]->dataChunk.numVoxels;
        VoxDataChunk* voxDataChunk = &engine->chunks[0]->dataChunk;
        std::cout << "Num Voxels: " << numModelVoxels << "\n";
        for (int i = 0; i < numModelVoxels-1; i++) {
            Vec3 thisVoxPos = Vec3(voxDataChunk->voxelArray[i].x, voxDataChunk->voxelArray[i].y, voxDataChunk->voxelArray[i].z);
            voxels.push_back(std::tuple<Vec3, Vec3, bool>(thisVoxPos, Vec3(0, 1, 0), false));
        }

        Shapes::CreatePlane(Vec3(16,-5,16), 100, Vec3(1,1,1), false, voxels);

        // **Emissive Cube**
        // lights.push_back(Vec3(0, 12, 32));
        // Shapes::CreateCube(lights[0], 3, Vec3(1,1,1)*4, true, voxels);
        lights.push_back(Vec3(16, -1, -10));
        Shapes::CreateCube(lights[0], 3, Vec3(1,1,1)*4, true, voxels);
        lights.push_back(Vec3(64, 7, 64));
        Shapes::CreateCube(lights[1], 3, Vec3(1,1,1)*4, true, voxels);

        // // **Cube**
        // Shapes::CreateCube(Vec3(16,-1,16), 5, Vec3(0,1,0), false, voxels);
        // Shapes::CreateCube(Vec3(8,-4,8), 2, Vec3(1,0,0), false, voxels);
        // Shapes::CreateCube(Vec3(24,-4,24), 2, Vec3(1,0,0), false, voxels);

        // **Sort Voxel Positions
        std::sort(voxels.begin(), voxels.end(), [](auto tupA, auto tupB) {
            return (std::get<0>(tupA) > std::get<0>(tupB));
        });

        // Build SVO
        int numVoxels = voxels.size();
        svo->insert(std::get<0>(voxels[0]), std::get<1>(voxels[0]), std::get<2>(voxels[0]));
        svo->insert(std::get<0>(voxels[numVoxels]), std::get<1>(voxels[numVoxels]), std::get<2>(voxels[numVoxels]));
        for (int i = 1; i < numVoxels-1; i++) {
            svo->insert(std::get<0>(voxels[i]), std::get<1>(voxels[i]), std::get<2>(voxels[i]));
        }

        svo->calcNumNodes();

        SVOBuffer svoBuff(svo);

        svoBuff.buildArray();

        std::vector<NodeData*> arr(svoBuff.nodeArraySize);
        for (int i = 0; i < svoBuff.nodeArraySize; i++) {
            arr[i] = svoBuff.nodeArray[i];
        }

        std::cout << "Foo\n";

        std::cout << "Min point: " << svo->getMin() << "\n";
        std::cout << "Max point: " << svo->getMax() << "\n";

    }

    void render(int numThreads) {
        // renderVoxInit();
        renderPolyInit();
        renderCPU(numThreads);
    }
};
#endif