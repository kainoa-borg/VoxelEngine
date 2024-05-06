#pragma once
// #include "../utils/iVec2.hpp"
// #include "../utils/Vec3.hpp"
#include "../utils/Matrix4x4.hpp"
#include "Ray.hpp"

struct Camera {
    iVec2 resolution;
    float aspectRatio;
    float fov;

    float viewportHeight;
    float viewportWidth;
    Vec3 viewportUpperLeft;
    Vec3 pixelUpperLeft;

    Vec3 up;
    Vec3 forward;

    Vec3 viewportU;
    Vec3 viewportV;
    Vec3 pixelDeltaU;
    Vec3 pixelDeltaV;
    Vec3 u, v, w;
    Vec3 center;

    Matrix4x4 cameraToWorldMatrix;

    Camera(Vec3 _pos, Vec3 _up, Vec3 _forward, iVec2 _resolution, float _fov) : center(_pos), up(_up), forward(_forward), resolution(_resolution), fov(_fov) {
        up.normalize();
        
        aspectRatio = resolution.x/resolution.y;

        Vec3 tempUHHHHH = (center - _forward);

        float focalLength = tempUHHHHH.mag();
        float theta = fov * M_PI / 180;
        float h = tan(theta/2);

        viewportHeight = 2.0f * h * focalLength;
        viewportWidth = viewportHeight * (static_cast<float>(resolution.x) / resolution.y);

        w = (center - forward).normalized();
        u = w.cross(up);
        v = w.cross(u);

        viewportU = u * viewportWidth;
        viewportV = -v * viewportHeight;

        pixelDeltaU = viewportU / resolution.x;
        pixelDeltaV = viewportV / resolution.y;

        // center = Vec3::Zero() + w * pos.z + v * pos.y + u * pos.x;

        viewportUpperLeft = center - (w*focalLength) - viewportU/2 - viewportV/2;
        Vec3 temp = (pixelDeltaU + pixelDeltaV) * 0.5f;
        pixelUpperLeft = viewportUpperLeft + temp;
    }

    void computeCamToWorldMatrix() {
        // cameraToWorldMatrix = Matrix4x4();
        // Vec3 cameraZ(0, 0, -1);



        // float rotX = acos(forward.dot(cameraZ));

        // float c = cos(rotAngle);
        // float s = sin(rotAngle);
        // float t = (1 - c);


        // // float rot[] = {
        // //     c + u.x*u.x*t, u.y*u.x*t + u.z*s, u.z*u.x*t - u.y*s, 0, // col 1
        // //     u.x*u.y*t - u.z*s, c + u.y*u.y*t, u.z*u.y*t + u.x*s, 0,
        // //     u.x*u.z*t + u.y*s, u.y*u.z*t - u.x*s, c + u.z*u.z*t, 0,
        // //     0, 0, 0, 1
        // // };

        // float rotX[] = {
        //     1, 0, 0, 0,
        //     0, cX, -sX, 0,
        //     0, sX, cX, 0,
        //     0, 0, 0, 1
        // };
        // float rotY[] = {
        //     cY, 0, 0, 0,
        //     0, 1, -sY, 0,
        //     -sY, 0, -sY, 0,
        //     0, 0, 0, 1
        // };
        // float rotZ[] = {
        //     cZ, -sZ, 0, 0,
        //     sZ, cZ, 0, 0,
        //     0, 0, 1, 1,
        //     0, 0, 0, 1,
        // };
        // float trans[] = {
        //     1, 0, 0, pos.x,
        //     0, 1, 0, pos.z,
        //     0, 0, 1, pos.y,
        //     0, 0, 0, 1
        // };
        // Matrix4x4 mTrans(trans);
        // mTrans.transpose();
        // Matrix4x4 temp(mRot * mTrans);

        // memcpy(&cameraToWorldMatrix.matrix, &temp.matrix, sizeof(temp.matrix));
    }

    void setRayInvDir(Ray &ray) {
        ray.invDir.x = 1 / ray.direction.x;
        ray.invDir.y = 1 / ray.direction.y;
        ray.invDir.z = 1 / ray.direction.z;
    }

    void constructRay(Ray &ray, iVec2 pixel) {
        auto pixelCenter = pixelUpperLeft + (pixelDeltaU * pixel.x) + (pixelDeltaV * pixel.y);
        ray.origin = center;
        ray.direction = pixelCenter - center;
        ray.direction.normalize();
        ray.invDir.x = 1 / ray.direction.x;
        ray.invDir.y = 1 / ray.direction.y;
        ray.invDir.z = 1 / ray.direction.z;
    }
};


namespace GlobalCamera {
// Global Camera Position
const Vec3 gPos(70, 45, 16);
// Global Camera Look-At
// const Vec3 gForward(7, -3, 16);
const Vec3 gForward(32, 25, 32);

int gFov = 85;

// Global Camera Up
Vec3 gUp(0,1,0);

int gRes = 200;

// Global Camera Resolution
iVec2 gResolution = iVec2(gRes, gRes);

// Global Camera Instance
Camera gCamera = Camera(gPos, gUp, gForward, gResolution, gFov);
}