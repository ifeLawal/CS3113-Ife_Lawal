//
//  UniSheetSprite.hpp
//  NYUCodebase
//
//  Created by Ifeoluwa Lawal on 10/24/16.
//  Copyright © 2016 Ivan Safrin. All rights reserved.
//



#ifndef spriteSheet_h
#define spriteSheet_h

#include <stdio.h>
#include "ShaderProgram.h"
#include <vector>
#include <stdarg.h>

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
    
    int walkCyclePos;
    int idleCyclePos;
    std::vector<int> idleSprites;
    std::vector<int> walkSprites;
    
    SpriteSheet();
    SpriteSheet(GLuint spriteSheet, int spriteCountX, int spriteCountY, int index);
    SpriteSheet(GLuint spriteSheet, float u, float v, float height, float width, float size);
    SpriteSheet(SpriteSheet const &sprite);
    //SpriteSheet(
    
    GLfloat* genCoords();
    
    void setSpriteCount(int x, int y);
    void setHNW(int h, int w);
    void setIndex(int i);
    
    void animateIdle();
    
    void animateWalk();
    
    void fillIdle(int n_args, ...);
    
    void fillWalk(int n_args, ...);
    
};

#endif /* sheetSprite_h */
