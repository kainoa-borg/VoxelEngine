#pragma once
#include "Ray.hpp"
#include <vector>
#include <random>

namespace Lighting {


float iRayAxesDebug(Ray &r, float &tMin, float &tMax, iVec3 &step, int &thisDepth, iVec3 &thisCenter) {
    bool signX = r.invDir.x >= 0;
    bool signY = r.invDir.y >= 0;
    bool signZ = r.invDir.z >= 0;

    Vec3 minPos;
    Vec3 maxPos;

    minPos = thisCenter;
    maxPos = thisCenter;

    float t1x = signX ? (maxPos.x - r.origin.x) * r.invDir.x : (minPos.x - r.origin.x) * r.invDir.x;
    float t1y = signY ? (maxPos.y - r.origin.y) * r.invDir.y : (minPos.y - r.origin.y) * r.invDir.y;
    float t1z = signZ ? (maxPos.z - r.origin.z) * r.invDir.z : (minPos.z - r.origin.z) * r.invDir.z;

    if (t1x != t1x || t1x <= -INFINITY) t1x = INFINITY;
    if (t1y != t1y || t1y <= -INFINITY) t1y = INFINITY;
    if (t1z != t1z || t1z <= -INFINITY) t1z = INFINITY;

    float thisTMax = 0;

    // Amanatides and Woo
    if (t1x < t1y) {
        if (t1x < t1z) {
            // We traverse x
            thisTMax = t1x;
        } else {
            // We traverse z
            thisTMax = t1z;
        }
        
    } else {
        if (t1y < t1z) {
            // We traverse y
            thisTMax = t1y;
        } else {
            // We traverse z
            thisTMax = t1z;
        }
    }

    return thisTMax;
}

Vec3 uniformSampleHemisphere(const float &r1, const float &r2)
{
    // cos(theta) = r1 = y
    // cos^2(theta) + sin^2(theta) = 1 -> sin(theta) = srtf(1 - cos^2(theta))
    float sinTheta = sqrtf(1 - r1 * r1);
    float phi = 2 * M_PI * r2;
    float x = sinTheta * cosf(phi);
    float z = sinTheta * sinf(phi);
    return Vec3(x, r1, z);
}

static Vec3 generateRandomDirection(std::mt19937 &generator, std::uniform_real_distribution<float> &distribution, const Vec3 &hitNormal, const Vec3 &Nt, const Vec3 &Nb, float &cosTheta) {
    float r1 = distribution(generator); // cos(theta) = N.Light Direction
    cosTheta = r1;
    float r2 = distribution(generator);
    Vec3 sample = Lighting::uniformSampleHemisphere(r1, r2);
    Vec3 worldSample = Vec3( 
        sample.x * Nb.x + sample.y * hitNormal.x + sample.z * Nt.x,
        sample.x * Nb.y + sample.y * hitNormal.y + sample.z * Nt.y,
        sample.x * Nb.z + sample.y * hitNormal.z + sample.z * Nt.z);
    worldSample.normalize();
    return worldSample;
}


static bool iRayBox(Ray &r, Vec3 minPos, Vec3 maxPos, float &tMin, float &tMax) {
    bool signX = r.invDir.x >= 0;
    bool signY = r.invDir.y >= 0;
    bool signZ = r.invDir.z >= 0;

    float t0x = signX ? (minPos.x - r.origin.x) * r.invDir.x : (maxPos.x - r.origin.x) * r.invDir.x; 
    float t1x = signX ? (maxPos.x - r.origin.x) * r.invDir.x : (minPos.x - r.origin.x) * r.invDir.x;

    tMin = t0x;
    tMax = t1x;

    float t0y = signY ? (minPos.y - r.origin.y) * r.invDir.y : (maxPos.y - r.origin.y) * r.invDir.y;
    float t1y = signY ? (maxPos.y - r.origin.y) * r.invDir.y : (minPos.y - r.origin.y) * r.invDir.y;

    if ((tMin > t1y) || (t0y > tMax)) 
        return false;

    if (t0y > tMin) 
        tMin = t0y;
    if (t1y < tMax) 
        tMax = t1y;

    float t0z = signZ ? (minPos.z - r.origin.z) * r.invDir.z : (maxPos.z - r.origin.z) * r.invDir.z;
    float t1z = signZ ? (maxPos.z - r.origin.z) * r.invDir.z : (minPos.z - r.origin.z) * r.invDir.z;

    if (tMin > t1z || t0z > tMax) 
        return false;

    if (t0z > tMin) tMin = t0z;
    if (t1z < tMax) tMax = t1z;

    // if (r.origin.x >= minPos.x && r.origin.y >= minPos.y && r.origin.z >= minPos.z && r.origin.x <= maxPos.x && r.origin.y <= maxPos.y && r.origin.z <= maxPos.z) 
        // return INFINITY;

    // if (tMin > tMax) return tMax;

    return true;
}

void iRayChildDebug(Ray &r, float &tMin, float &tMax, iVec3 &step, int &voxelSize, iVec3 &thisCenter) {

    // int voxelSize = pow(2, depth - thisDepth);

    Vec3 fHitPoint = r.origin + r.direction * (tMin + .5f);
    // Vec3 minHitPoint = r.origin + r.direction * tMin;
    // Vec3 maxHitPoint = r.origin + r.direction * tMax;
    // Vec3 minCenter = minHitPoint - voxelSize;
    // thisCenter = minHitPoint + voxelSize;

            // ** Better 3D Case: **
    // ** Coordinates in morton order **
    // ** We know the quadrant that the ray originates in **
    // ** Assuming quad is centered on origin **
    // if the origin.x is positive -> origin in (1,?,?)
    if (fHitPoint.x - thisCenter.x > 0) {
        // if the origin.y is positive -> origin in (1,1,?)
        if (fHitPoint.y - thisCenter.y > 0) {
            // if the origin.z is positive -> origin in (1,1,1)
            if (fHitPoint.z - thisCenter.z > 0) {
                step = iVec3(1,1,1);
            }
            // if the origin.z is negative -> origin in (1,1,0)
            else {
                step = iVec3(1, 1, 0);
            }
        // if the origin.y is negative -> origin in (1,0,?)
        }
        else {
            // if the origin.z is positive -> origin in (1,0,1)
            if (fHitPoint.z - thisCenter.z > 0) {
                step = iVec3(1,0,1);
            }
            // if the origin.z is negative -> origin in (1,0,0)
            else {
                step = iVec3(1,0,0);
            }
        }
    }
    // else origin.x is negative -> origin in (0,?,?)
    else {
        // if the origin.y is positive -> origin in (0,1,?)
        if (fHitPoint.y - thisCenter.y > 0) {
            // if the origin.z is positive ->  origin in (0,1,1)
            if (fHitPoint.z - thisCenter.z > 0) {
                step = iVec3(0,1,1);
            }
            // if the origin.z is negative -> origin in (0,1,0)
            else {
                step = iVec3(0,1,0);
            }
        }
        // if the origin.y is negative -> origin in (0,0,?)
        else {
            // if the origin.z is positive -> origin in (0,0,1)
            if (fHitPoint.z - thisCenter.z > 0) {
                step = iVec3(0,0,1);
            }
            // if the origin.z is positive -> origin in (0,0,0)
            else {
                step = iVec3(0,0,0);
            }
        }
    }

    Vec3 translation = Vec3((step.x + .5f), (step.y + .5f), (step.z + .5f));
    thisCenter = thisCenter + translation * voxelSize;
}

}