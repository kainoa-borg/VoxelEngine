#include "Vec3.hpp"
#include "../VoxelEngine/Face.hpp"
#include "math.h"
#include "../VoxelEngine/Ray.hpp"
#include "../VoxelEngine/Lighting.hpp"

struct BVHNode
{
    Vec3 aabbMin, aabbMax;     // 24 bytes
    uint leftChild, rightChild;  // 8 bytes
    uint leftNode, firstFaceIdx, faceCount;
    bool isLeaf() { return faceCount > 0; }
}; 

class BVH
{
public:
    std::vector<Face> faces;
    std::vector<BVHNode> bvhNodes;
    uint rootNodeIdx, nodesUsed;
    std::vector<uint> faceIdx;

    BVH() {
        rootNodeIdx = 0;
        nodesUsed = 0;
    }

    BVH(std::vector<Face>& _faces) {
        faces = _faces;
        faceIdx.resize(faces.size());
        bvhNodes.resize(faces.size() * 2 - 1);
        rootNodeIdx = 0;
        nodesUsed = 0;
    }

    void UpdateNodeBounds(uint nodeIdx) {
        BVHNode& node = bvhNodes[nodeIdx];
        node.aabbMin = Vec3(1e30f, 1e30f, 1e30f);
        node.aabbMax = Vec3(-1e30f, -1e30f, -1e30f);
        for (uint first = node.firstFaceIdx, i = 0; i < node.faceCount; i++)
        {
            uint leafFaceIdx = faceIdx[first + i];
            Face& leafFace = faces[leafFaceIdx];
            node.aabbMin = Vec3::fminf( node.aabbMin, leafFace.vertices[0] );
            node.aabbMin = Vec3::fminf( node.aabbMin, leafFace.vertices[1] );
            node.aabbMin = Vec3::fminf( node.aabbMin, leafFace.vertices[2] );
            node.aabbMax = Vec3::fmaxf( node.aabbMax, leafFace.vertices[0] );
            node.aabbMax = Vec3::fmaxf( node.aabbMax, leafFace.vertices[1] );
            node.aabbMax = Vec3::fmaxf( node.aabbMax, leafFace.vertices[2] );
        }
    }

    void Subdivide( uint nodeIdx )
    {
        // terminate recursion
        BVHNode& node = bvhNodes[nodeIdx];
        if (node.faceCount <= 2) return;
        // determine split axis and position
        Vec3 extent = node.aabbMax - node.aabbMin;
        int axis = 0;
        if (extent.y > extent.x) axis = 1;
        if (extent.z > extent[axis]) axis = 2;
        float splitPos = node.aabbMin[axis] + extent[axis] * 0.5f;
        // in-place partition
        int i = node.firstFaceIdx;
        int j = i + node.faceCount - 1;
        while (i <= j)
        {
            if (faces[faceIdx[i]].center[axis] < splitPos)
                i++;
            else
                std::swap( faceIdx[i], faceIdx[j--] );
        }
        // abort split if one of the sides is empty
        int leftCount = i - node.firstFaceIdx;
        if (leftCount == 0 || leftCount == node.faceCount) return;
        // create child nodes
        int leftChildIdx = nodesUsed++;
        int rightChildIdx = nodesUsed++;
        bvhNodes[leftChildIdx].firstFaceIdx = node.firstFaceIdx;
        bvhNodes[leftChildIdx].faceCount = leftCount;
        bvhNodes[rightChildIdx].firstFaceIdx = i;
        bvhNodes[rightChildIdx].faceCount = node.faceCount - leftCount;
        node.leftNode = leftChildIdx;
        node.faceCount = 0;
        UpdateNodeBounds( leftChildIdx );
        UpdateNodeBounds( rightChildIdx );
        // recurse
        Subdivide( leftChildIdx );
        Subdivide( rightChildIdx );
    }
 
    void BuildBVH(std::vector<Face>& _faces)
    {
        // populate triangle index array
        for (int i = 0; i < faces.size(); i++) faceIdx[i] = i;
        int N = faces.size();
        // assign all triangles to root node
        BVHNode& root = bvhNodes[rootNodeIdx];
        root.leftChild = root.rightChild = 0;
        root.faceCount = N;
        nodesUsed++;
        UpdateNodeBounds( rootNodeIdx );
        // subdivide recursively
        Subdivide( rootNodeIdx );
    }

    bool IntersectAABB( const Ray& ray, const Vec3 bmin, const Vec3 bmax )
    {
        float tx1 = (bmin.x - ray.origin.x) / ray.direction.x, tx2 = (bmax.x - ray.origin.x) / ray.direction.x;
        float tmin = std::fmin( tx1, tx2 ), tmax = std::fmax( tx1, tx2 );
        float ty1 = (bmin.y - ray.origin.y) / ray.direction.y, ty2 = (bmax.y - ray.origin.y) / ray.direction.y;
        tmin = std::fmax( tmin, std::fmin( ty1, ty2 ) ), tmax = std::fmin( tmax, std::fmax( ty1, ty2 ) );
        float tz1 = (bmin.z - ray.origin.z) / ray.direction.z, tz2 = (bmax.z - ray.origin.z) / ray.direction.z;
        tmin = std::fmax( tmin, std::fmin( tz1, tz2 ) ), tmax = std::fmin( tmax, std::fmax( tz1, tz2 ) );
        return tmax >= tmin && tmin < ray.t && tmax > 0;
    }

    bool rayTriangleIntersect(
    const Vec3 &orig, const Vec3 &dir,
    const Vec3 &v0, const Vec3 &v1, const Vec3 &v2,
    float &t)
    {
        // Compute the plane's normal
        Vec3 v0v1 = v1 - v0;
        Vec3 v0v2 = v2 - v0;
        // No need to normalize
        Vec3 N = v0v1.cross(v0v2); // N
        float area2 = N.mag();
    
        // Step 1: Finding P
        
        // Check if the ray and plane are parallel
        float NdotRayDirection = N.dot(dir);
        if (fabs(NdotRayDirection) < 0.001f) // Almost 0
            return false; // They are parallel, so they don't intersect!

        if (NdotRayDirection > 0) {
            N = -N;
        }

        // Compute d parameter using equation 2
        float d = -N.dot(v0);
        
        // Compute t (equation 3)
        t = -(N.dot(orig) + d) / NdotRayDirection;
        
        // Check if the triangle is behind the ray
        if (t < 0) return false; // The triangle is behind
    
        // Compute the intersection point using equation 1
        Vec3 P = orig + dir * t;
    
        // Step 2: Inside-Outside Test
        Vec3 C; // Vector perpendicular to triangle's plane
    
        // Edge 0
        Vec3 edge0 = v1 - v0; 
        Vec3 vp0 = P - v0;
        C = edge0.cross(vp0);
        if (N.dot(C) < 0) return false; // P is on the right side
    
        // Edge 1
        Vec3 edge1 = v2 - v1; 
        Vec3 vp1 = P - v1;
        C = edge1.cross(vp1);
        if (N.dot(C) < 0) return false; // P is on the right side
    
        // Edge 2
        Vec3 edge2 = v0 - v2; 
        Vec3 vp2 = P - v2;
        C = edge2.cross(vp2);
        if (N.dot(C) < 0) return false; // P is on the right side

        return true; // This ray hits the triangle
    }

    bool IntersectFace( Ray& ray, const Face& face, Vec3& albedo, bool& isEmissive, float& tMin, float& tMax, Vec3& hitNormal)
    {
        Vec3 edge1 = face.vertices[1] - face.vertices[0];
        Vec3 edge2 = face.vertices[2] - face.vertices[0];
        Vec3 h = ray.direction.cross(edge2);
        float a = edge1.dot(h);
        if (a > -0.0001f && a < 0.0001f) return false; // ray parallel to triangle
        float f = 1 / a;
        Vec3 s = ray.origin - face.vertices[0];
        float u = f * s.dot(h);
        if (u < 0 || u > 1) return false;
        Vec3 q = s.cross(edge1);
        float v = f * ray.direction.dot(q);
        if (v < 0 || u + v > 1) return false;
        const float t = f * edge2.dot(q);
        if (t > 0.0001f) {
            ray.t = std::fmin(t, ray.t);
            return true;
        }
        else {
            return false;
        }
    }

    bool dumbTrace( Ray& ray, Vec3& albedo, bool& isEmissive, float& tMin, float& tMax, Vec3& hitNormal) {
        int face_index = -1;
        bool didHit = false;
        int hitFaces = 0;
        int minFaces = 0;
        float t = INFINITY;
        for (int i = 0; i < faces.size(); i++) {
            // if (IntersectFace( ray, faces[i], albedo, isEmissive, tMin, tMax, hitNormal) && tMin < t) {
            if (rayTriangleIntersect(ray.origin, ray.direction, faces[i].vertices[0], faces[i].vertices[1], faces[i].vertices[2], tMin) && tMin < t) {
                t = tMin;
                face_index = i;
                didHit = true;
            }
        }
        if (didHit) {
            // // Compute the plane's normal
            // Vec3 v0v1 = faces[face_index].vertices[1] - faces[face_index].vertices[0];
            // Vec3 v0v2 = faces[face_index].vertices[2] - faces[face_index].vertices[0];
            // // No need to normalize
            // Vec3 N = v0v1.cross(v0v2); // N
            hitNormal = faces[face_index].normal;
            // hitNormal = ray.direction.dot(hitNormal) > 0 ? -hitNormal : hitNormal;
            isEmissive = false;
            albedo = faces[face_index].color;
            return true;
        }
        return false;
    }

    void IntersectBVH(bool& didHit, Ray& ray, const uint nodeIdx, Vec3& albedo, bool& isEmissive, Vec3& hitNormal, float& tMin, float& tMax)
    {
        int f_index = -1;
        BVHNode& node = bvhNodes[nodeIdx];
        float aabbtMin, aabbtMax = 0;
        if (!Lighting::iRayBox(ray, node.aabbMin, node.aabbMax, aabbtMin, aabbtMax)) {
            return;
        }
        if (node.isLeaf())
        {
            float currtMin = INFINITY;
            for (uint i = 0; i < node.faceCount; i++ ) {
                auto& thisFace = faces[faceIdx[node.firstFaceIdx + i]];
                if (rayTriangleIntersect(ray.origin, ray.direction, thisFace.vertices[0], thisFace.vertices[1], thisFace.vertices[2], currtMin) && currtMin < tMin) {
                // if (IntersectFace( ray, thisFace, albedo, isEmissive, tMin, tMax, hitNormal) && tMin < t) {
                    didHit = true;
                    tMin = currtMin;
                    ray.t = tMin;
                    f_index = faceIdx[node.firstFaceIdx + i];
                    hitNormal = thisFace.normal;
                    // hitNormal = ray.direction.dot(thisFace.normal) > 0 ? -thisFace.normal : thisFace.normal;
                    albedo = thisFace.color;
                    isEmissive = false;
                }
            }
        }
        else
        {
            IntersectBVH( didHit, ray, node.leftNode, albedo, isEmissive, hitNormal, tMin, tMax);
            IntersectBVH( didHit, ray, node.leftNode + 1, albedo, isEmissive, hitNormal, tMin, tMax);
        }
    }
};