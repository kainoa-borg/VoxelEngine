#pragma once

#include <cstdint>
#include <stddef.h>
#include <iostream>
#include <stack>

#include "../VoxelEngine/Ray.hpp"
#include "../VoxelEngine/Debug.hpp"
#include "../utils/Bmp.hpp"
#include "../VoxelEngine/Face.hpp"

using namespace std;
struct SVONode {
public:
    SVONode* parent;
    SVONode* children;
    bool isLeaf;
    bool isFilled;
    bool isEmissive;
    Vec3 color;
    Vec3 center;
    std::vector<Vec3> vertices;
    virtual SVONode* getParent() {};
    virtual void setParent(SVONode* parent) {};
    virtual SVONode** getChildren() {};
    virtual void setChild(int index, SVONode* child) {};
    virtual bool getIsLeaf() {};
    virtual bool getIsFilled() {};
    virtual void setIsEmissive(bool _isEmissive) {};
    virtual bool getIsEmissive() {};
    virtual Vec3 getColor() {};
    virtual Vec3 getCenter() {};
    virtual std::vector<Vec3> setVertices() {};
    virtual void setCenter(Vec3 _center) {};
    virtual void setColor(Vec3 _color) {};
    virtual void setIsLeaf(bool _isLeaf) {};
    virtual void setIsFilled(bool _isFilled) {};
    virtual void setVertices(std::vector<Vec3> _vertices) {};
};
struct SVOLeaf : public SVONode {
public:
    bool isFilled; // For now just filled/not
    bool isLeaf;
    bool isEmissive;
    Vec3 color;
    std::vector<Vec3> vertices;

    Vec3 getColor() {
        return color;
    }
    void setColor(Vec3 _color) {
        color = _color;
    }
    
    SVOLeaf(SVONode* _parent) {
        parent = _parent;
        isFilled = false;
        isLeaf = true;
    }
    SVOLeaf(bool _isFilled) {
        isFilled = _isFilled;
        isLeaf = true;
    }

    SVONode* getParent() {
        return parent;
    }
    void setParent(SVONode* _parent) {
        parent = _parent;
    }
    SVONode** getChildren() {
        return nullptr;
    }
    void setChild(int index, SVONode* _child) {
        return;
    }
    
    bool getIsLeaf() {
        return isLeaf;
    }
    void setIsLeaf(bool _isLeaf){
        isLeaf = _isLeaf;
    }
    bool getIsFilled() {
        return isFilled;
    }
    void setIsFilled(bool _isFilled) {
        isFilled = _isFilled;
    }
    void setIsEmissive(bool _isEmissive) {isEmissive = _isEmissive;}
    bool getIsEmissive() { return isEmissive; };

    virtual Vec3 getCenter() {return center;}
    virtual void setCenter(Vec3 _center) {center = _center;}

    virtual std::vector<Vec3> getVertices() {return vertices;}
    virtual void setVertices(std::vector<Vec3> _vertices) {
        vertices = _vertices;
    ;}
};
struct SVOBranch : public SVONode {
public:
    SVONode* children[8];
    bool isLeaf;
    bool isFilled;
    Vec3 bounds[2];
    Vec3 center;

    SVOBranch() {
        for (int i = 0; i < 8; i++) {
            children[i] = nullptr;
        };
        isLeaf = false;
    }

    SVOBranch(SVONode* _parent, SVONode* _children[]) {
        parent = _parent;
        for (int i = 0; i < 8; i++) {
            children[i] = _children[i];
        }
    }

    SVONode** getChildren() {
        return children;
    }
    void setChild(int index, SVONode* _child) {
        children[index] = _child;
    }
    bool getIsLeaf() {
        return isLeaf;
    }
    void setIsLeaf(bool _isLeaf) {
        return;
    }
    bool getIsFilled() {
        return false;
    }
    void setIsFilled(bool _isFilled) {
        return;
    }
};

class _Renderer;
class SVO {
public:
    SVONode* root;
    int svoDim;
    int depth;
    int nodeCount = 0;
    Vec3 minPosition;
    Vec3 maxPosition;
    Vec3 center;

    unordered_map<uint64_t, Vec3> indexMap;

    void calcNumNodes() {
        //cout << "Dimensions: " << svoDim << "\n";
        //cout << "Min Position: " << "(" << minPosition.x << ", " << minPosition.y << ", " << minPosition.z << ")" << "\n";
        SVONode* currRoot;
        int count = 0;
        std::stack<SVONode*> nodesToVisit;
        std::stack<int> nodeNums;
        nodesToVisit.push(root);
        nodeNums.push(nodeCount);
        while(nodesToVisit.size() > 0) {
            currRoot = nodesToVisit.top();
            //cout << "Curr Node: " << nodeNums.top() << endl;
            nodeNums.pop();
            nodesToVisit.pop();
            if (currRoot && !currRoot->getIsLeaf()) {
                for (int i = 0; i < 8; i++) {
                    if (currRoot->getChildren()[i] != nullptr) {
                        nodesToVisit.push(currRoot->getChildren()[i]);
                        nodeNums.push(nodeCount);
                    }
                    nodeCount += 1;
                }
            } 
            else {
                if (currRoot->getIsFilled()) {
                    count += 1;
                }
            }
        }
        cout << "Leaf Count: " << count << "\n";
    }

    SVO() {
        root = new SVOBranch();
        for(int i = 0; i < 8; i++) {
            root->getChildren()[i] = nullptr;
        }
        depth = 1;
        svoDim = pow(2, depth);
        center = Vec3(0,0,0);
        minPosition = Vec3(INFINITY, INFINITY, INFINITY);
        maxPosition = Vec3(-INFINITY, -INFINITY, -INFINITY);
    }

    ~SVO() {
        recursiveDelete(root);
    }

    void clear() {
        for (int i = 0; i < 8; i++) {
            SVONode* child = root->getChildren()[i];
            recursiveDelete(child);
            root->getChildren()[i] = nullptr;
        }
    }

    void recursiveDelete(SVONode* node) {
        if (node == nullptr) return;
        if (!node->getIsLeaf()) {
            for (int i = 0; i < 8; i++) {
                recursiveDelete(node->getChildren()[i]);
            }
            delete node;
        }
    }

    // method to seperate bits from a given integer 3 positions apart
    static inline uint64_t splitBy3(unsigned long a){
        uint64_t x = a & 0x1fffff; // we only look at the first 21 bits
        x = (x | x << 32) & 0x1f00000000ffff; // shift left 32 bits, OR with self, and 00011111000000000000000000000000000000001111111111111111
        x = (x | x << 16) & 0x1f0000ff0000ff; // shift left 32 bits, OR with self, and 00011111000000000000000011111111000000000000000011111111
        x = (x | x << 8) & 0x100f00f00f00f00f; // shift left 32 bits, OR with self, and 0001000000001111000000001111000000001111000000001111000000000000
        x = (x | x << 4) & 0x10c30c30c30c30c3; // shift left 32 bits, OR with self, and 0001000011000011000011000011000011000011000011000011000100000000
        x = (x | x << 2) & 0x1249249249249249;
        return x;
    }

    static inline uint64_t getThirdBits(uint64_t a){
        uint64_t x = a & 0x1249249249249249; // we only look at the first 21 bits
        x = (x ^ (x >> 2)) & 0x10c30c30c30c30c3;
        x = (x ^ (x >> 4)) & 0x100f00f00f00f00f; // shift left 32 bits, OR with self, and 0001000011000011000011000011000011000011000011000011000100000000
        x = (x ^ (x >> 8)) & 0x1f0000ff0000ff; // shift left 32 bits, OR with self, and 0001000000001111000000001111000000001111000000001111000000000000
        x = (x ^ (x >> 16)) & 0x1f00000000ffff; // shift left 32 bits, OR with self, and 00011111000000000000000011111111000000000000000011111111
        x = (x ^ (x >> 32)) & 0x1fffff; // shift left 32 bits, OR with self, and 00011111000000000000000000000000000000001111111111111111
        return x;
    }

    static inline Vec3 getVector3(uint64_t index){
        int x = getThirdBits(index);
        int y = getThirdBits(index >> 1);
        int z = getThirdBits(index >> 2);

        return Vec3(x, y, z);
    }


    static inline uint64_t getIndex(Vec3 v){
        unsigned int x = v.x;
        unsigned int y = v.y;
        unsigned int z = v.z;

        uint64_t answer = 0;
        answer |= splitBy3(x) | splitBy3(y) << 1 | splitBy3(z) << 2;
        return answer;
    }

    uint32_t abs_svo(int32_t n) {
        return n < 0 ? (-n - 1) : n;
    }
    
    bool compAgainstBounds(Vec3 v, uint32_t d) {
        return (abs_svo(v.x) | abs_svo(v.y) | abs_svo(v.z)) >= d;
    }

    string getBinaryString(uint64_t num, int numBits) {
        string s;
        while(num > 0) {
            s = ((num % 2 == 0) ? '0' : '1') + s;
            num = num / 2;
        }
        int len = s.length();
        for (int i = 0; i < (numBits - len); i++) {
            s = '0' + s;
        }
        return s;
    }

    SVONode* leafToBranch(SVONode* currLeaf, int index) {
        SVONode* temp = currLeaf;
        currLeaf = new SVOBranch();
        currLeaf->setChild(index, temp);
        return currLeaf;
    }

    void setMin() {
        minPosition = getMin();
    }
    void setMax() {
        maxPosition = getMax();
    }

    Vec3 getMin() {
        int minCoord = svoDim;
        return Vec3(-minCoord, -minCoord, -minCoord);
    }

    Vec3 getMax() {
        int maxCoord = svoDim;
        return Vec3(maxCoord, maxCoord, maxCoord);
    }

    void getPath(std::vector<int> &indices, uint64_t index) {
        if (index == 0)
            indices.push_back(0);
        while(index > 0) {
            indices.push_back((index) & 0b111);
            index = index >> 3;
        }
    }

    Vec3 deNormalizePos(Vec3 pos) {
        Vec3 maxSVOPos = getMax();
        return pos - maxSVOPos;
    }
    Vec3 normalizePos(Vec3 pos) {
        Vec3 minSVOPos = getMin();
        return pos - minSVOPos;
    }

    bool inBounds(Vec3 vec) {
        int tempMax = getMax().x * 2;
        return (vec.x >= 0 && vec.y >= 0 && vec.z >= 0 && vec.x <= tempMax && vec.y <= tempMax && vec.z <= tempMax);
    }

    void calcNormal(Ray &r, Vec3 &newOrigin, Vec3 thisCenter, int thisDepth, Vec3 &normal) {

        Vec3 retNormal;

        float voxelSize = pow(2, depth - thisDepth);

        bool sideX = thisCenter.x - newOrigin.x < 0;
        bool sideY = thisCenter.y - newOrigin.y < 0;
        bool sideZ = thisCenter.z - newOrigin.z < 0;

        if (thisCenter.x - newOrigin.x == 0.0f) {
            sideX = r.direction.x > 0;
        }
        if (thisCenter.y - newOrigin.y == 0.0f) {
            sideY = r.direction.y > 0;
        }
        if (thisCenter.z - newOrigin.z == 0.0f) {
            sideZ = r.direction.z > 0;
        }

        Vec3 translate(sideX ? 1 : -1, sideY ? 1 : -1, sideZ ? 1 : -1);

        // Vec3 translation = Vec3((step.x)-1, (step.y)-1, (step.z-1));
        // thisCenter = thisCenter + (translate * (voxelSize));

        float transX = (sideX ? 1 : -1) * voxelSize;
        float transY = (sideY ? 1 : -1) * voxelSize;
        float transZ = (sideZ ? 1 : -1) * voxelSize;
        
        // Vec3 thisCorner = thisCenter + translate * voxelSize;
        float cornerX = thisCenter.x + transX;
        float cornerY = thisCenter.y + transY;
        float cornerZ = thisCenter.z + transZ;
        // newOrigin = newOrigin + (translate * voxelSize);

        Vec3 v(newOrigin.x - cornerX, 0, 0);
        Vec3 u(0, newOrigin.y - cornerY, 0);
        Vec3 w(0, 0, newOrigin.z - cornerZ);

        if (v.mag() > u.mag()) {
            if (v.mag() > w.mag()) {
                normal = v.normalized();
            }
            else {
                normal = w.normalized();
            }
        }
        else {
            if (u.mag() > w.mag()) {
                normal = u.normalized();
            }
            else {
                normal = w.normalized();
            }
        }
    }

    void iRayChild(Ray &r, Vec3 &newOrigin, int &index, int &thisDepth, Vec3 &thisCenter, Vec3 &normal) {

        int voxelSize = pow(2, depth - thisDepth);

        bool sideX = thisCenter.x - newOrigin.x < 0;
        bool sideY = thisCenter.y - newOrigin.y < 0;
        bool sideZ = thisCenter.z - newOrigin.z < 0;

        if (thisCenter.x - newOrigin.x == 0.0f) {
            sideX = r.direction.x > 0;
        }
        if (thisCenter.y - newOrigin.y == 0.0f) {
            sideY = r.direction.y > 0;
        }
        if (thisCenter.z - newOrigin.z == 0.0f) {
            sideZ = r.direction.z > 0;
        }

        index = (sideX ? 1 : 0) | (sideY ? 2 : 0) | (sideZ ? 4 : 0);

        float transX = (sideX ? 1 : -1) * voxelSize;
        float transY = (sideY ? 1 : -1) * voxelSize;
        float transZ = (sideZ ? 1 : -1) * voxelSize;
        
        // Vec3 thisCenter = thisCenter + translate * voxelSize;
        thisCenter.x += + transX;
        thisCenter.y += + transY;
        thisCenter.z += + transZ;
    }

    float iRayAxes(Ray &r, Vec3 &newOrigin, float &tMin, float &tMax, int &thisDepth, Vec3 &thisCenter) {

        float dX = thisCenter.x - newOrigin.x;
        float dY = thisCenter.y - newOrigin.y;
        float dZ = thisCenter.z - newOrigin.z;

        float t1x = dX * r.invDir.x;
        float t1y = dY * r.invDir.y;
        float t1z = dZ * r.invDir.z;

        if (t1x <= 0.0f) t1x = 10000;
        if (t1y <= 0.0f) t1y = 10000;
        if (t1z <= 0.0f) t1z = 10000;

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

        return thisTMax + tMin;
    }


    // Mcgraw et. al.
    bool betterTraceRay(Ray &r, Vec3 &goalPoint, Vec3 &hitColor, bool &emissive, Vec3 &normal, float&sceneMin, float &sceneMax) {
        // tMin ← sceneMin;
        float tMin = fmax(0.0f, sceneMin);
        // tMax ← sceneMin; 
        float tMax = fmax(0.0f, sceneMin);
        SVONode* currRoot = root;
        int baseDepth = 0;
        int thisDepth = 0;
        Vec3 baseCenter = center;
        Vec3 thisCenter = center;
        float closest = 0;
        // while tMax < sceneMax do
        int loopCount = 0;
        while (tMax < sceneMax) {
            // tMin ← tMax; 
            tMin = tMax;
            // tMax ← sceneMax;
            tMax = sceneMax; 
            // node ← root; 
            SVONode* node = currRoot;
            thisDepth = baseDepth;
            thisCenter = baseCenter;
            // pushdown ← true;
            bool pushdown = true;
            // Debug::debugRenderBox(thisCenter, thisCenter, r.origin + r.direction + tMin, pow(2, depth - thisDepth), loopCount, pixelIndex);
            // while node.type != leaf do
            // std::cout << "Starting from depth: " << thisDepth << "\n";
            Vec3 newOrigin = (r.origin) + (r.direction) * (tMin);
            while (node && !node->getIsLeaf()) {
                // tSplit ← node.splitPlaneIntersection(ray)
                float tSplit = iRayAxes(r, newOrigin, tMin, tMax, thisDepth, thisCenter);
                closest = tSplit;
                // if tSplit > tMin AND tSplit<tMax then
                if ((tSplit > tMin) && tSplit < tMax) { 
                    // tMax ← tSplit;
                    tMax = tSplit; 
                    // pushdown ← false;
                    pushdown = false;
                // end if
                }
                // node ← node.child(ray, tMin, tMax)
                thisDepth += 1;
                // iRayChild(r, tMin, tMax, step, thisDepth, thisCenter);
                int index = 0;
                iRayChild(r, newOrigin, index, thisDepth, thisCenter, normal);
                // std::cout << index << "\n";
                node = node->getChildren()[index];
                // Debug::debugRenderBox(thisCenter, thisCenter, r.origin + r.direction + tMin, pow(2, depth - thisDepth), loopCount, pixelIndex);
                // if pushdown == true then
                if (pushdown) {
                    // root ← node
                    currRoot = node;
                    baseDepth = thisDepth;
                    baseCenter = thisCenter;
                // end if
                }
            // end while
            }
            // if node.isEmpty == false then
            if (node && node->getIsFilled()) {
                // return ray intersection with leaf node contents
                hitColor = hitColor + node->getColor();
                emissive = node->getIsEmissive();
                calcNormal(r, newOrigin, node->getCenter(), thisDepth, normal);
                sceneMin = tMin;
                sceneMax = tMax;
                return true;
            // end if 
            }
        // end while
        }
        // No intersection found
        return false;
    }

    bool find(Vec3 position, int &thisDepth) {
        Vec3 nPosition = normalizePos(position);
        int index = getIndex(nPosition);
        vector<int> indices;
        getPath(indices, index);
        SVONode* currNode = root;
        int leafIndex = indices[0];
        for (int i = indices.size() - 1; i > 0; i--) {
            std::cout << indices[i] << ", ";
            // If the next branch doesn't exist
            if(currNode->getIsLeaf()) {
                thisDepth = indices.size() - 1;
                return false;
            }
            else if (currNode->getChildren()[indices[i]]) {
                currNode = currNode->getChildren()[indices[i]];
            }
        }
        // If this is a leaf
        if(!currNode->getIsLeaf()) {
            currNode = currNode->getChildren()[leafIndex];
        }
        // If it is filled
        if (currNode && currNode->getIsLeaf() && currNode->getIsFilled()) {
            return true;
        }
        thisDepth = indices.size() - 1;
        std::cout << "\n";
        return false;
    }

    void grow(SVONode* currRoot) {
        bool grew = false;
        for (int i = 0; i < 8; i++) {
            if (currRoot->getChildren()[i]) {
                SVONode* newBranchParent = new SVOBranch();
                SVONode* temp = currRoot->getChildren()[i];
                newBranchParent->setChild(~i & 0b111, currRoot->getChildren()[i]);
                temp->setParent(newBranchParent);
                currRoot->setChild(i, newBranchParent);
            }
        }
        depth += 1;
        svoDim = pow(2, depth);
    }

    void fillNode(uint64_t index, Vec3 _position, Vec3 _color, bool _isEmissive, std::vector<Vec3> _vertices) {
        vector<int> indices;
        uint64_t tempIndex = index;
        Vec3 thisPos = getVector3(index);
        getPath(indices, index);
        SVONode* currNode = root;
        int leafIndex = indices[0];
        for (int i = indices.size() - 1; i > 0; i--) {
            // If the next branch doesn't exist
            if (!currNode->getChildren()[indices[i]]) {
                // Create it
                currNode->setChild(indices[i], new SVOBranch());
            }
            currNode = currNode->getChildren()[indices[i]];
        }
        // // If the leaf in this branch does not yet exist 
        if (!currNode->getChildren()[leafIndex]) {
            // Create it
            currNode->setChild(leafIndex, new SVOLeaf(currNode));
        } 
        // The something exists at this point, traverse to it
        currNode = currNode->getChildren()[leafIndex];
        currNode->setIsFilled(true);
        currNode->setColor(_color);
        currNode->setIsEmissive(_isEmissive);
        currNode->setCenter(_position);
        currNode->setVertices(_vertices);
    }

    void insert(Vec3 position, Vec3 _color, bool _isEmissive, std::vector<Vec3> vertices) {
        // Test signed position against SVO dimensions
        SVONode* currRoot = root;
        while(compAgainstBounds(position, svoDim)) {
            // Grow the SVO
            grow(currRoot);
        }
        // Subtract minPosition of SVO from position to normalize
        Vec3 minSVOPos = getMin();
        Vec3 nPosition = position - minSVOPos;
        // Get SVO index from nPosition
        uint64_t index = getIndex(nPosition);

        // Traverse SVO to index and fix the voxel
        fillNode(index, position, _color, _isEmissive, vertices);
    }
};
