//
//  ReadTileMap.hpp
//  NYUCodebase
//
//  Created by Ifeoluwa Lawal on 11/14/16.
//  Copyright © 2016 Ivan Safrin. All rights reserved.
//

#ifndef ReadTileMap_hpp
#define ReadTileMap_hpp

#include <string>
#include <stdio.h>

bool readHeader(std::ifstream &stream);

bool readLayerData(std::ifstream &stream);

bool readEntityData(std::ifstream &stream);

void placeEntity();

#endif /* ReadTileMap_hpp */
