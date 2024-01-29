#pragma once
#include "../utils/Bmp.hpp"
#include "Lighting.hpp"
#include "Camera.hpp"
#include <stdlib.h>

namespace Debug {
static void setColorTest(char &r, char &g, char &b) {

}

float remap(float v, float inMin, float inMax, float outMin, float outMax) {
  return (v - inMin) / (inMax - inMin) * (outMax - outMin) + outMin;
}

static void debugRenderBox(Vec3 center, Vec3 goalPoint, Vec3 rayHitPoint, float size, int loopNum, iVec3 pixelIndex) {

    std::vector<std::vector<RGB>> pixels;

    BmpInfoHeader bmpInfoHeader = BmpInfoHeader(GlobalCamera::gResolution.x, -GlobalCamera::gResolution.y); 
    BmpHeader bmpHeader = BmpHeader(iVec2(GlobalCamera::gResolution.x, GlobalCamera::gResolution.y));

    pixels.resize(GlobalCamera::gResolution.y);
    for (int i = 0; i < GlobalCamera::gResolution.y; i++) {
        pixels[i].resize(GlobalCamera::gResolution.x);
    }

    size = size;

    if (size == 0) {
        size = 0.5f;
    }

    Vec3 minPos = Vec3(center.x - size, center.y - size, center.z - size);
    Vec3 maxPos = Vec3(center.x + size, center.y + size, center.z + size);

    for (int heightIndex = 0; heightIndex < GlobalCamera::gResolution.y; heightIndex++) {
        for (int widthIndex = 0; widthIndex < GlobalCamera::gResolution.x; widthIndex++) {
            // unsigned char val = 255 * (float)widthIndex/GlobalCamera::gResolution.x;
            unsigned char val = 255;
            Ray r;
            GlobalCamera::gCamera.constructRay(r, iVec2(widthIndex, heightIndex));

            float tMin;
            float tMax;
            bool hit = Lighting::iRayBox(r, minPos, maxPos, tMin, tMax);

            unsigned char bgR = 0;
            unsigned char bgG = 0;
            unsigned char bgB = 0;

            unsigned char hR = 193;
            unsigned char hG = 18;
            unsigned char hB = 31;

            unsigned char aR = 102;
            unsigned char aG = 155;
            unsigned char aB = 188;

            unsigned char rColor = bgR;
            unsigned char gColor = bgG;
            unsigned char bColor = bgB;

            float wB = (tMax - tMin) / tMax;
            float wH = 1 - wB;

            Vec3 hitPoint = r.origin + r.direction * tMin;
            float cMin = hitPoint.x;
            if (hitPoint.y < cMin) 
                cMin = fabs(hitPoint.y);
            else if (abs(hitPoint.z) < cMin)
                cMin = abs(hitPoint.z);

            // float temp = hitToCenter.mag() / 32;
            Vec3 weightsB = Vec3(1, 1, 1);
            Vec3 weightsH = Vec3(1,1,1) - weightsB;

            if (hit) {
                // Vec3 hitPoint = r.origin + r.direction * tMin;
                // r.origin = r.origin + r.direction * tMin;
                float tNext;

                // Vec3 hitToCamera = pos - hitPoint;
                // Vec3 cameraToOrigin = Vec3(0,0,0) - pos;
                // float distWeight = 1;
                // float dist = fabs(hitToCamera.mag());
                // float maxDist = 600;
                // float minDist = 200;
                // distWeight = remap(dist, 200, 500, .2, 1.0);
                // float otherWeight = 1 - distWeight;

                // int rColori = max((int)hR * distWeight + bgR * otherWeight, 0.0f);
                // int gColori = max((int)hG * distWeight + bgG * otherWeight, 0.0f);
                // int bColori = max((int)hB * distWeight + bgB * otherWeight, 0.0f);

                rColor = hR;
                gColor = hG;
                bColor = hB;

                

                int countAxis = 0;

                // iRayBox(r, minPos, maxPos, tMin, tNext);
                // iVec3 step = iVec3(0, 0, 0);
                // int depth = 0;
                // tNext = iRayAxesDebug(r, tMin, tMax, step, depth, center);
                // if (tNext < tMin) {
                //     // std::cout << "uhhhhhh\n";
                //     float temp = tMin;
                //     tMin = tNext;
                //     tNext = temp;
                // }
                weightsH = Vec3(1, 1, 1);
                weightsB = Vec3(1,1,1) - weightsH;


                // Vec3 newMinPos = Vec3(center.x - size/2, center.y - size/2, center.z - size/2);
                // Vec3 newMaxPos = Vec3(center.x + size/2, center.y + size/2, center.z + size/2);

                Vec3 goalMinPos = Vec3(goalPoint.x, goalPoint.y, goalPoint.z);
                Vec3 goalMaxPos = Vec3(goalPoint.x + 1, goalPoint.y + 1, goalPoint.z + 1);

                bool hitGoal = Lighting::iRayBox(r, goalMinPos, goalMaxPos, tMin, tNext);

                if (hitGoal) {
                    rColor = aR;
                    gColor = aG;
                    bColor = aB;
                }


                if ((abs(hitPoint.x)) < .5 && (abs(hitPoint.x)) > 0) {
                    countAxis += 1;
                }
                if ((abs(hitPoint.y) ) < .5 && (abs(hitPoint.y)) > 0) {
                    countAxis += 1;
                }
                if ((abs(hitPoint.z)  < .5 && (abs(hitPoint.z)) > 0)) {
                    countAxis += 1;
                }

                if (countAxis>0) {
                    rColor = 0;
                    gColor = 255;
                    bColor = 0;
                }

                int countUnit = 0;

                if (hitPoint.x > floor(hitPoint.x) - 0.1 && hitPoint.x < floor(hitPoint.x) + 0.1) {
                    countUnit += 1;
                }
                if (hitPoint.y > floor(hitPoint.y) - 0.1 && hitPoint.y < floor(hitPoint.y) + 0.1) {
                    countUnit += 1;
                }
                if ((hitPoint.z > floor(hitPoint.z) - 0.1 && hitPoint.z < floor(hitPoint.z) + 0.1)) {
                    countUnit += 1;
                }

                if (countUnit>1) {
                    rColor = 0;
                    gColor = 0;
                    bColor = 255;
                }
                
            }

            int pixelSize = GlobalCamera::gResolution.x / pixelIndex.z;
            int heightMin = (pixelIndex.y*pixelSize);
            int heightMax = (pixelIndex.y*pixelSize) + pixelSize;
            int widthMin = (pixelIndex.x*pixelSize);
            int widthMax = (pixelIndex.x*pixelSize) + pixelSize;

            if (heightIndex >= heightMin && heightIndex <= heightMax && widthIndex >= widthMin && widthIndex <= widthMax) {
                rColor = floor(rColor*.5 + 255*.5);
                gColor = floor(gColor*.5 + 255*.5);
                bColor = floor(bColor*.5 + 255*.5);
            }

            
            Vec3 rayHitMinPoint = Vec3(rayHitPoint.x - .05f, rayHitPoint.y - .05f, rayHitPoint.z - .05f);
            Vec3 rayHitMaxPoint = Vec3(rayHitPoint.x + .05f, rayHitPoint.y + .05f, rayHitPoint.z + .05f);
            
            float tMinTemp;
            float tMaxTemp;

            if (Lighting::iRayBox(r, rayHitMinPoint, rayHitMaxPoint, tMinTemp, tMaxTemp)) {
                rColor = 255;
                gColor = 255;
                bColor = 0;
            }


            pixels[heightIndex][widthIndex] = RGB(rColor, gColor, bColor);
        } 
    }
    // std::cout << sizeof(BmpHeader) << std::endl;
    // std::cout << sizeof(BmpInfoHeader) << std::endl;
    // std::cout << sizeof(RGB) << std::endl;
    // std::cout << bmpHeader.sizeOfBitmapFile << std::endl;

    // std::string loopNumString = "loop";
    // loopNumString.append(std::to_string(loopNum));
    // loopNumString.append(".bmp");

    exportBmp("output.bmp", bmpHeader, bmpInfoHeader, GlobalCamera::gResolution, pixels);
    return;
}

static void debugRenderBox(Vec3 center, Vec3 goal, Vec3 hitPoint, int size, int loopNum) {
    debugRenderBox(center, goal, hitPoint, size, loopNum, iVec3(0, 0, 10));
}

static void debugRenderBox(Vec3 center, Vec3 goal, int size, int loopNum) {
    debugRenderBox(center, goal, center, size, loopNum, iVec3(0, 0, 10));
}

static void debugRenderBox(Vec3 center, int size, int loopNum) {
    debugRenderBox(center, center, center, size, loopNum, iVec3(0, 0, 10));
}

}

