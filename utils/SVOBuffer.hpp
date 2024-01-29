#pragma once
#include "../utils/SVO.hpp"
#include "../utils/Vec3.hpp"
#include <queue>

struct NodeData {
    bool isFilled;
    bool isLeaf;
    bool isEmissive;
    int childIndices[8];
    Vec3 center;
    Vec3 color;
};

class SVOBuffer {
public:
    SVO* svo;
    int currIndex;
    NodeData** nodeArray;
    int nodeArraySize = 0;
    SVOBuffer(SVO* svo) {
        this->svo = svo;
        currIndex = 0;
    }

    void setNodeInArray(SVONode* currRoot, int thisIndex) {
        nodeArray[thisIndex] = new NodeData();
        if (currRoot->getIsLeaf()) {
            nodeArray[thisIndex]->isLeaf = currRoot->getIsLeaf();
            nodeArray[thisIndex]->isFilled = currRoot->getIsFilled();
            nodeArray[thisIndex]->center = currRoot->getCenter();
            nodeArray[thisIndex]->color = currRoot->getColor();
            nodeArray[thisIndex]->isEmissive = currRoot->getIsEmissive();
        }
        else {
            for (int i = 0; i < 8; i++) {
                if (currRoot->getChildren()[i]) {
                    nodeArray[thisIndex]->childIndices[i] = currIndex + i;
                }
                else {
                    nodeArray[thisIndex]->childIndices[i] = -1;
                }

            }
        }
    }

    void buildArray() {
        svo->calcNumNodes();
        nodeArraySize = svo->nodeCount;
        nodeArray = new NodeData*[nodeArraySize];
        std::queue<SVONode*> nodeQueue;
        std::queue<int> nodeIndices;
        SVONode* currRoot = svo->root;
        while (currRoot) {
            // If this is a branch
            if (!currRoot->getIsLeaf()) {
                for (int i = 0; i < 8; i++) {
                    if (currRoot->getChildren()[i]){
                        nodeQueue.push(currRoot->getChildren()[i]);
                        nodeIndices.push(currIndex);
                    }
                    currIndex += 1;
                }
            }
            if (nodeQueue.size() > 0) {
                currRoot = nodeQueue.front();
                setNodeInArray(currRoot, nodeIndices.front());
                nodeIndices.pop();
                nodeQueue.pop();
            }
            else {
                currRoot = nullptr;
            }
        }
    }
};