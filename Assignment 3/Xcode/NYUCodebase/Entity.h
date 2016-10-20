//
//  Entities.hpp
//  NYUCodebase
//
//  Created by Ifeoluwa Lawal on 10/18/16.
//  Copyright © 2016 Ivan Safrin. All rights reserved.
//

#ifndef Entities_hpp
#define Entities_hpp

#include <stdio.h>
#include "Matrix.h"
#include "ShaderProgram.h"

class Entity {
public:
    float xTrans, yTrans, zTrans;
    float width;
    float height;
    int index, spriteCountX, spriteCountY;
    //int level, xPos;
    GLuint spriteSheet;
    Matrix modelMatrix;
    float vertices[12];
    int level;
    bool wallTouching, active;
    
    Entity(float wVal, float hVal, GLuint sheet, int lvl);
    
    Entity(const Entity &other);
    
    void setUpSpriteSheet(int in, int xCount, int yCount);
    
    
    void setTrans(float x, float y, float z);
    
    void updateIndex(int in);
    
    void updateY(float yC);
    
    bool isColliding(Entity other);
    
    void updatePosition(float x, float y, float z);
    
    void setMatrices(ShaderProgram *program);
    
    void drawSprite(ShaderProgram* program);
    
    void DrawSpriteSheetSprite(ShaderProgram *program, int index, int spriteCountX, int spriteCountY);
    
    void buildTriangle(ShaderProgram *program);
    
    void render(ShaderProgram *program);
};

#endif /* Entities_hpp */
