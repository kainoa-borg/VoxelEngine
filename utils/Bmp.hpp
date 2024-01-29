#pragma once
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string>
#include <vector>

#include "../utils/iVec2.hpp"

using namespace std;
struct BmpHeader {
    char bitmapSignatureBytes[2] = {'B', 'M'};
    uint32_t sizeOfBitmapFile = 54 + 786432;
    uint32_t reservedBytes = 0;
    uint32_t pixelDataOffset = 54;

    void writeToFile() {
        ofstream fout("bmpHeader", ios::binary);
        fout.write(this->bitmapSignatureBytes, 2);
        fout.write((char*)&this->sizeOfBitmapFile, sizeof(uint32_t));
        fout.write((char*)&this->reservedBytes, sizeof(uint32_t));
        fout.write((char*)&this->pixelDataOffset, sizeof(uint32_t));
        fout.close();
    }

    BmpHeader() {};
    BmpHeader(iVec2 resolution) {
        sizeOfBitmapFile = 54 + (resolution.x * resolution.y * 3);
    };
};

struct BmpInfoHeader {
    uint32_t sizeOfThisHeader = 40;
    int32_t width = 512; // in pixels
    int32_t height = 512; // in pixels
    uint16_t numberOfColorPlanes = 1; // must be 1
    uint16_t colorDepth = 24;
    uint32_t compressionMethod = 0;
    uint32_t rawBitmapDataSize = 0; // generally ignored
    int32_t horizontalResolution = 3780; // in pixel per meter
    int32_t verticalResolution = 3780; // in pixel per meter
    uint32_t colorTableEntries = 0;
    uint32_t importantColors = 0;

    void writeToFile() {
        ofstream fout("bmpInfoHeader", ios::binary);
        fout.write((char*)&this->sizeOfThisHeader, sizeof(uint32_t));
        fout.write((char*)&this->width, sizeof(int32_t));
        fout.write((char*)&this->height, sizeof(int32_t));
        fout.write((char*)&this->numberOfColorPlanes, sizeof(uint16_t));
        fout.write((char*)&this->colorDepth, sizeof(uint16_t));
        fout.write((char*)&this->compressionMethod, sizeof(uint32_t));
        fout.write((char*)&this->rawBitmapDataSize, sizeof(uint32_t));
        fout.write((char*)&this->horizontalResolution, sizeof(int32_t));
        fout.write((char*)&this->verticalResolution, sizeof(int32_t));
        fout.write((char*)&this->colorTableEntries, sizeof(uint32_t));
        fout.write((char*)&this->importantColors, sizeof(uint32_t));
        fout.close();
    }

    BmpInfoHeader() {};
    BmpInfoHeader(int _width, int _height) : width(_width), height(_height) {};
};

struct RGB {
    unsigned char b;
    unsigned char g;
    unsigned char r;

    RGB() {};
    RGB(unsigned char _r, unsigned char _g, unsigned char _b) : r(_r), g(_g), b(_b) {};

    RGB operator+(RGB other) {
        return RGB(r + other.r, g + other.g, b + other.b);
    }
    RGB operator/(int i) {
        return RGB(r / i, g / i, b / i);
    }

    RGB operator*(float f) {
        return RGB(r * f, g * f, b * f);
    }
};

void exportBmp(std::string fileName, BmpHeader &_bmpHeader, BmpInfoHeader &_bmpInfoHeader, iVec2 _resolution, std::vector< std::vector<RGB> > &_pixels) {
    _bmpHeader.writeToFile();
    _bmpInfoHeader.writeToFile();

    int numPixels = _resolution.x * _resolution.y;
    // std::vector<unsigned char> pixelsBuff(numPixels*3);
    std::vector<unsigned char> pixelsBuff(numPixels*3);

    // pixelsBuff.resize(numPixels*3);

    // ofstream outPixelsFile("pixels", ios::binary);
    // ifstream inPixelsFile("pixels", ios::binary);

    ifstream inBmpHeader("bmpHeader", ios::binary);

    ifstream inBmpInfoHeader("bmpInfoHeader", ios::binary);

    ofstream outFile(fileName, ios::binary);


    for (int i = 0; i < _resolution.y; i++) {
        for (int j = 0; j < _resolution.x; j++) {
            pixelsBuff[j*3 + i*_resolution.x*3] = _pixels[i][j].b;
            pixelsBuff[j*3 + i*_resolution.x*3 + 1] = _pixels[i][j].g;
            pixelsBuff[j*3 + i*_resolution.x*3 + 2] = _pixels[i][j].r;
        }
    }

    if (!outFile) {
        std::cout << "Failed to open bmp file!\n";
    }

    // std::cout << "Concatenating headers to outFile\n";
    outFile << inBmpHeader.rdbuf() << inBmpInfoHeader.rdbuf();
    
    // if (!inPixelsFile.read((char*) pixelsBuff, numPixels)) {
    //     std::cout << "Failed to read pixels!\n";
    // }

    // std::cout << "Writing pixel data\n";
    outFile.write((char*)&pixelsBuff[0], pixelsBuff.size());
        
    // std::cout << "Wrote pixel data\n";
    
    outFile.close();
}