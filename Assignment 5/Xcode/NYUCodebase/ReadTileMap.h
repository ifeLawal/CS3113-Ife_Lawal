//
//  ReadTileMap.hpp
//  NYUCodebase
//
//  Created by Ifeoluwa Lawal on 11/14/16.
//  Copyright © 2016 Ivan Safrin. All rights reserved.
//

#ifndef ReadTileMap_h
#define ReadTileMap_h

#define TILE_SIZE 16
#define SPRITE_COUNT_X 16
#define SPRITE_COUNT_Y 8

#include "ShaderProgram.h"
#include <vector>

class ReadTileMap {
public:
    GLuint mapWidth;
    GLuint mapHeight;
    std::vector<std::string> types;
    std::vector<float> xPosList;
    std::vector<float> yPosList;
    unsigned char** levelData;
    std::vector<int> solidTiles;
    std::vector<int> deathTiles;
    
    ReadTileMap();

    bool readHeader(std::ifstream &stream);
    
    bool readLayerData(std::ifstream &stream);
    
    bool readEntityData(std::ifstream &stream);
    
    void renderMap(ShaderProgram *program, GLuint spriteSheet);
    
    void placeEntity(std::string type, float x, float y);
    
    void worldToTileCoordinates(float worldX, float worldY, int *gridX, int *gridY);
    
    bool isSolid(int x, int y);
    
    bool isDeathTile(int x, int y);

};


#endif /* ReadTileMap_hpp */
