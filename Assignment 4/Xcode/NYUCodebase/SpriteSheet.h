//
//  UniSheetSprite.hpp
//  NYUCodebase
//
//  Created by Ifeoluwa Lawal on 10/24/16.
//  Copyright © 2016 Ivan Safrin. All rights reserved.
//

#include "ShaderProgram.h"

#ifndef SpriteSheet_h
#define SpriteSheet_h

#include <stdio.h>

class SpriteSheet {

public:
    int spriteCountX;
    int spriteCountY;
    int index = -1;
    GLuint spriteSheet;
    float u;
    float v;
    float height;
    float width;
    float size;
    
    SpriteSheet();
    SpriteSheet(GLuint spriteSheet, int spriteCountX, int spriteCountY, int index);
    SpriteSheet(GLuint spriteSheet, float u, float v, float height, float width, float size);
    SpriteSheet(SpriteSheet const &sprite);
    
    GLfloat* genCoords();
    
    void setSpriteCount(int x, int y);
    void setHNW(int h, int w);
    void setIndex(int i);
    
};

#endif /* UniSheetSprite_hpp */
