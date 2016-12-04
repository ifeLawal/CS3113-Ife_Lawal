//
//  NYUCB.hpp
//  NYUCodebase
//
//  Created by Ifeoluwa Lawal on 12/3/16.
//  Copyright © 2016 Ivan Safrin. All rights reserved.
//

#ifndef NYUCB_hpp
#define NYUCB_hpp

#include <stdio.h>

class Shapes {
    
public:
    float xTrans, yTrans, zTrans;
    float width;
    float height;
    float size;
    float animationRate;
    //int level, xPos;
    Matrix modelMatrix;
    float vertices[12];
    
    int collisionAmt;
    
    bool isStatic;
    EntityType entityType;
    
    SpriteSheet sprite;
    float xFric, yFric;
    float xVelocity, yVelocity, zVelocity;
    float xAccle, yAccle, zAccle, yGravity;
    
    bool collidedTop;
    bool collidedBottom;
    bool collidedLeft;
    bool collidedRight;
    bool landed;
    
    Shapes();
    
    float lerp (float v0, float v1, float t);
    
    Entity(float wVal, float hVal, float size, bool st, EntityType eType, SpriteSheet *sheet);
    
    Entity(const Entity &other);
    
    void setPosition(float x, float y, float z);
    
    void updatePosition(float x, float y, float z);
    
    void movement(ShaderProgram* program, Entity *other, float elapsed);
    
    bool isColliding(Entity *other);
    
    float xCollisionHandling(Entity *other);
    
    float yCollisionHandling(Entity *other);
    
    void setMatrices(ShaderProgram *program);
    
    void buildTriangle(ShaderProgram *program);
    
    void render(ShaderProgram *program, float elapsed);
    
};


#endif /* NYUCB_hpp */
