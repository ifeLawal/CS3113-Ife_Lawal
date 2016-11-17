//
//  ReadTileMap.cpp
//  NYUCodebase
//
//  Created by Ifeoluwa Lawal on 11/14/16.
//  Copyright © 2016 Ivan Safrin. All rights reserved.
//

#include "ReadTileMap.h"

#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#define TILE_SIZE
using namespace std;

float mapWidth = -1;
float mapHeight = -1;
unsigned char** levelData;

bool readHeader(std::ifstream &stream) {
    string line;
    while(getline(stream, line)) {
        if(line == "") {break;}
        
        istringstream sStream(line);
        string key, value;
        getline(sStream, key, '=');
        getline(sStream, value);
        
        if(key == "width") {
            mapWidth = atoi(value.c_str());
        } else if (key == "height") {
            mapHeight = atoi(value.c_str());
        }
    }
    
    if(mapWidth == -1 || mapHeight == -1) {
        return false;
    } else { // allocate our map data
        levelData = new unsigned char*[mapHeight];
        for(int i = 0; i < mapHeight; ++i) {
            levelData[i] = new unsigned char[mapWidth];
        }
}

bool readLayerData(std::ifstream &stream);

bool readEntityData(std::ifstream &stream);
