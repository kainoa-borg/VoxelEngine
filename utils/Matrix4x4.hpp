#pragma once
#include "../utils/Vec3.hpp"
#include <iostream>

class Matrix4x4 {
public:
    int dim = 4;
    float matrix[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    Matrix4x4() {};
    Matrix4x4(float temp[16]) {
        for(int i = 0; i < 16; i++) {
            matrix[i] = temp[i];
        }
    }

    inline void transpose() {
        float m[16];
        for (int x = 0; x < dim; x++) {
            for (int y = 0; y < dim; y++) {
                m[x*dim+y] = matrix[y*dim+x];
            }
        }
        memcpy(&matrix, &m, sizeof(m));
    }

    inline Matrix4x4 operator*(Matrix4x4 &other) {
        Matrix4x4 c = Matrix4x4();
        for (int i = 0; i < dim; i++)
            for (int k = 0; k < dim; k++)
                for (int j = 0; j < dim; j++) { // swapped order
                    float a = this->matrix[i*dim+k];
                    float b = other.matrix[k+dim+j];
                    float s = c.matrix[i*dim+j];
                    c.matrix[i*dim+j] += this->matrix[i*dim+k] * other.matrix[k*dim+j];
                }
        return c;
    }

    inline Vec3 operator*(Vec3 &other) {
        float mVec[4] = {other.x, other.y, other.z, 1};
        float resMVec[4] = {0, 0, 0, 0};
        for (int i = 0; i < dim; i++) {
            for (int k = 0; k < dim; k++) {
                float a = matrix[i*dim+k];
                float b = mVec[k];
                resMVec[i] += a * b;
            }
        }
        return Vec3(resMVec[0], resMVec[1], resMVec[2]);
    }
};