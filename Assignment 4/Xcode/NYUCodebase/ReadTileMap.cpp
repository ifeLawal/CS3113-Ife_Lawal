//
//  ReadTileMap.cpp
//  NYUCodebase
//
//  Created by Ifeoluwa Lawal on 11/14/16.
//  Copyright © 2016 Ivan Safrin. All rights reserved.
//

#include "ReadTileMap.h"


#define TILE_SIZE 16
#define SPRITE_COUNT_X 16
#define SPRITE_COUNT_Y 8
using namespace std;

ReadTileMap::ReadTileMap() {
    GLuint mapWidth = -1;
    GLuint mapHeight = -1;
}

bool ReadTileMap::readHeader(std::ifstream &stream) {
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
        return true;
    }
}

bool ReadTileMap::readLayerData(std::ifstream &stream) {
    string line;
    while(getline(stream, line)) {
        if(line == "") { break; }
        istringstream sStream(line);
        string key, value;
        getline(sStream, key, '=');
        getline(sStream, value);
        if(key == "data") {
            for (int y = 0; y < mapHeight; y++) {
                getline(stream, line);
                istringstream lineStream(line);
                string tile;
                
                for(int x=0; x < mapWidth;x++) {
                    getline(lineStream, tile, ',');
                    printf("%s", tile.c_str());
                    unsigned char val = (unsigned char)atoi(tile.c_str());
                    if(val > 0) {
                        levelData[y][x] = val - 1;
                    } else {
                        levelData [y][x] = 0;
                    }
                }
            }
        }
    }
    return true;
}

bool ReadTileMap::readEntityData(std::ifstream &stream) {
    string line;
    string type;
    
    while(getline(stream, line)) {
        if(line == "") { break; }
        
        istringstream sStream(line);
        string key, value;
        getline(sStream, key, '=');
        getline(sStream, value);
        
        if(key == "type") {
            type = value;
        } else if(key == "location") {
            istringstream lineStream(value);
            string xPosition, yPosition;
            getline(lineStream, xPosition, ',');
            getline(lineStream, yPosition, ',');
            
            float placeX = atoi(xPosition.c_str())*TILE_SIZE;
            float placeY = atoi(yPosition.c_str())*TILE_SIZE;
            
            types.push_back(type);
            xPosList.push_back(placeX);
            yPosList.push_back(placeY);
        }
    }
    return true;
}

void ReadTileMap::renderMap(ShaderProgram *program, GLuint spriteSheet) {
    vector<float> vertexData;
    vector<float> texCoordData;
    int numTex = 0;
    float scaleSize = .02;
    
    for(int y=0; y < mapHeight;y++) {
        for(int x=0;x< mapWidth;x++) {
            if(levelData[y][x] != 0) {
                numTex += 1;
                printf("numTexIn: %i\n", numTex);
                float u = (float)(((int)levelData[x][y]) % SPRITE_COUNT_X) / (float) SPRITE_COUNT_X;
                float v = (float)(((int)levelData[x][y]) / SPRITE_COUNT_Y) / (float) SPRITE_COUNT_Y;
            
                float spriteWidth = 1.0f/(float)SPRITE_COUNT_X;
                float spriteHeight = 1.0f/(float)SPRITE_COUNT_Y;
                if(numTex == 1) {
                    vertexData.insert(vertexData.end(), {
                        0.0,-0.2,
                        0.2, -0.2,
                        0.2, 0.0,
                        
                        0.0, -0.2,
                        0.2, 0.0,
                        0.0, 0.0
                    });
                } else {
                vertexData.insert(vertexData.end(), {
                     TILE_SIZE * (float)x, -TILE_SIZE * (float)y,
                     TILE_SIZE * (float)x,  (-TILE_SIZE * (float)y)-TILE_SIZE,
                     (TILE_SIZE * (float)x)+TILE_SIZE,  (-TILE_SIZE * (float)y)-TILE_SIZE,
                
                     TILE_SIZE * (float)x,  -TILE_SIZE * (float)y,
                     (TILE_SIZE * (float)x)+TILE_SIZE,  (-TILE_SIZE * (float)y)-TILE_SIZE,
                     (TILE_SIZE * (float)x)+TILE_SIZE,  -TILE_SIZE * (float)y
                });
                }
                texCoordData.insert(texCoordData.end(), {
                    u, v,
                    u, v+spriteHeight,
                    u+spriteWidth, v+spriteHeight,
                
                    u, v,
                    u+spriteWidth, v+spriteHeight,
                    u+spriteWidth, v
                });
            }
        }
    }
    //printf("numTex: %i\n", numTex);
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertexData.data());
    glEnableVertexAttribArray(program->positionAttribute);
    
    
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoordData.data());
    glEnableVertexAttribArray(program->texCoordAttribute);
    
    glBindTexture(GL_TEXTURE_2D, spriteSheet);
    glDrawArrays(GL_TRIANGLES, 0, numTex*6);
    
    glDisableVertexAttribArray(program->positionAttribute);
    
    glDisableVertexAttribArray(program->texCoordAttribute);
}


void ReadTileMap::worldToTileCoordinates(float worldX, float worldY, int *gridX, int *gridY) {
    *gridX = (int)(worldX / TILE_SIZE);
    *gridY = (int)(-worldY / TILE_SIZE);
}
