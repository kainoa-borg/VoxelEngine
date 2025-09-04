#pragma once
#include <math.h>
#include <iostream>

class Vec3;

class iVec3 {
public:
    int x;
    int y;
    int z;

    iVec3() : x(0), y(0), z(0) {};
    iVec3(int _x, int _y, int _z) : x(_x), y(_y), z(_z) {};

    void operator=(Vec3 other);

    bool operator==(iVec3 other) {
        if (x == other.x && y == other.y && z == other.z) return true;
        return false;
    }
    bool operator<(iVec3  other) {
        if (x < other.x && y < other.y && z < other.z) return true;
        return false;
    }
    bool operator>(iVec3 other) {
        if (x > other.x && y > other.y && z > other.z) return true;
        return false;
    }
    bool operator<(Vec3 other);
    bool operator>(Vec3 other);

    iVec3 operator-(iVec3 other) {
        return iVec3(x - other.x, y - other.y, z - other.z);
    }
    iVec3 operator+(Vec3 other);

    iVec3 operator*(int i) {
        return iVec3(x * i, y * i, z * i);
    }
    iVec3 operator*(Vec3 other);

    static iVec3 extractMin(iVec3 v1, iVec3 v2) {
        int minX = v1.x < v2.x ? v1.x : v2.x;
        int minY = v1.y < v2.y ? v1.y : v2.y;
        int minZ = v1.z < v2.z ? v1.z : v2.z;
        return iVec3(minX, minY, minZ);
    }

    friend std::ostream& operator<<(std::ostream& out, iVec3 v) {
        out << "(" << v.x << ", " << v.y << ", " << v.z << ")";
        return out;
    }

};

class Vec3 {
public:
    float x;
    float y;
    float z;

    // Base constructor
    Vec3() {
        // Initialize with origin
        x = 0.0f;
        y = 0.0f;
        z = 0.0f;
    }
    // Full constructor
    Vec3(float _x, float _y, float _z) {
        // Initialize with these coordinates
        x = _x;
        y = _y;
        z = _z;
    }

    void createCoordinateSystem(Vec3 &Nt, Vec3 &Nb) {
        if (std::fabs(x) > std::fabs(y))
            Nt = Vec3(z, 0, -x) / sqrtf(x * x + z * z);
        else
            Nt = Vec3(0, -z, y) / sqrtf(y * y + z * z);
        Nb = this->cross(Nt); 
        return;
    }

    static Vec3 Zero() { return Vec3(0.0f, 0.0f, 0.0f); }

    void operator=(iVec3 other);
    bool operator>(iVec3 other);
    bool operator<(iVec3 other);

    Vec3 operator-() {
        return Vec3(-x, -y, -z);
    }

    // Dot Product
    float dot(Vec3 other) {
        return (this->x * other.x + this->y * other.y + this->z * other.z);
    }

    bool operator==(Vec3 other) {
        return x == other.x && y == other.y && z == other.z;
    }

    // Addition Assignment
    void operator+=(Vec3 other) {
        this->x += other.x;
        this->y += other.y;
        this->z += other.z;
    }
    // Addition
    friend Vec3 &operator+(Vec3 lhs, Vec3 rhs) {
        lhs += rhs;
        return lhs;
    }
    Vec3 operator+(float i) {
        return Vec3(x + i, y + i, z + i);
    }
    // friend Vec3 operator+(Vec3 a, Vec3 b) {
    //     return Vec3(a.x + b.x, a.y + b.y, a.z + b.z);
    // }


    // Subtraction Assingment
    void operator-=(Vec3 other) {
        this->x -= other.x;
        this->y -= other.y;
        this->z -= other.z;
    }
    // Subtraction
    friend Vec3 &operator-(Vec3 lhs, Vec3 rhs) {
        lhs -= rhs;
        return lhs;
    }

    // friend Vec3 operator-(Vec3 a, Vec3 b) {
    //     return Vec3(a.x - b.x, a.y - b.y, a.z - b.z);
    // }
    Vec3 operator-(float i) {
        return Vec3(x - i, y - i, z - i);
    }

    // Element-Wise Mult. Assigment
    void operator*=(Vec3 other) {
        this->x *= other.x;
        this->y *= other.y;
        this->z *= other.z;
    }
    // Element-Wise Mult.
    friend Vec3 &operator*(Vec3 lhs, Vec3 rhs) {
        lhs *= rhs;
        return lhs;
    }
    void operator*=(float f) {
        x *= f;
        y *= f;
        z *= f;
    }
    friend Vec3 &operator*(Vec3 lhs, float f) {
        lhs *= f;
        return lhs;
    }
    // // float Mult.
    // friend Vec3 operator*(float f, const Vec3 v) {
    //     return Vec3(v.x * f, v.y * f, v.z * f);
    // }
    // friend Vec3 operator*(const Vec3 v, float f) {
    //     return f * v;
    // }

    // Element-Wise Div. Assigment
    void operator/=(Vec3 other) {
        this->x /= other.x;
        this->y /= other.y;
        this->z /= other.z;
    }
    // Element-Wise Div.
    Vec3 operator/(Vec3 other) {
        return Vec3(this->x / other.x, this->y / other.y, this->z / other.z);
    }
    Vec3 operator/(float i) {
        return Vec3(x / i, y / i, z / i);
    }

    bool operator>(Vec3 other) {
        return (x > other.x || y > other.y || z > other.z);
    }
    bool operator<(Vec3 other) {
        float total = x + y + z;
        float totalOther = other.x + other.y + other.z;
        return (x < other.x && y < other.y && z < other.z);
        return (total < totalOther);
    }

    float mag() {
        return sqrt(x*x + y*y + z*z);
    }

    void normalize() {
        float thisMag = mag();
        if (thisMag == 0) {
            x = 0;
            y = 0;
            z = 0;
        }
        x = x / thisMag;
        y = y / thisMag;
        z = z / thisMag;
    }

    Vec3 normalized() {
        float thisMag = mag();
        if (thisMag == 0) {
            return Vec3(0, 0, 0);
        }
        Vec3 n = Vec3(x, y, z) / thisMag;
        return n;
    }

    Vec3 cross(Vec3 &other) {
        float cx = y*other.z - z*other.y;
        float cy = z*other.x - x*other.z;
        float cz = x*other.y - y*other.x;
        return Vec3(cx, cy, cz);
    }

    friend std::ostream& operator<<(std::ostream& out, Vec3 v) {
        out << "(" << v.x << ", " << v.y << ", " << v.z << ")";
        return out;
    }

    static Vec3 fminf(Vec3 a, Vec3 b);
    static Vec3 fmaxf(Vec3 a, Vec3 b);

    float operator[](int index) {
        if (index > 2 || index < 0) return -1;
        switch(index) {
            case 0: return x;
            case 1: return y;
            case 2: return z;
        }
    }
};

void iVec3::operator=(Vec3 other) {
    x = other.x;
    y = other.y;
    z = other.z;
}
bool iVec3::operator>(Vec3 other) {
    if (x > other.x && y > other.y && z > other.z);
}
bool iVec3::operator<(Vec3 other) {
    return (x < other.x && y < other.y && z < other.z);
}

void Vec3::operator=(iVec3 other) {
    x = other.x;
    y = other.y;
    z = other.z;
}
bool Vec3::operator>(iVec3 other) {
    return (x > other.x && y > other.y && z > other.z);
}
bool Vec3::operator<(iVec3 other) {
    return (x < other.x && y < other.y && z < other.z);
}

Vec3 Vec3::fminf(Vec3 a, Vec3 b) {
    return Vec3(fmin(a.x, b.x), fmin(a.y, b.y), fmin(a.z, b.z));
}

Vec3 Vec3::fmaxf(Vec3 a, Vec3 b) {
    return Vec3(fmax(a.x, b.x), fmax(a.y, b.y), fmax(a.z, b.z));
}

iVec3 iVec3::operator*(Vec3 other) {
    return iVec3(x * other.x, y * other.y, z * other.z);
}
iVec3 iVec3::operator+(Vec3 other) {
    return iVec3(x + other.x, y + other.y, z + other.z);
}
