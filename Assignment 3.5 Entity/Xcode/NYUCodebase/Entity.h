//
//  Entities.hpp
//  NYUCodebase
//
//  Created by Ifeoluwa Lawal on 10/18/16.
//  Copyright © 2016 Ivan Safrin. All rights reserved.
//

#ifndef Entity_h
#define Entity_h

#include <stdio.h>
#include "ReadTileMap.h"
#include "Matrix.h"
#include "ShaderProgram.h"
#include "SpriteSheet.h"

enum EntityType {ENTITY_BACK, ENTITY_PLAYER, ENTITY_ENEMY, ENTITY_COIN, ENTITY_WALL};

class Entity {
    
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
    
    Entity();
    
    float lerp (float v0, float v1, float t);
    
    Entity(float wVal, float hVal, float size, bool st, EntityType eType, SpriteSheet *sheet);
    
    Entity(const Entity &other);
    
    void setPosition(float x, float y, float z);
    
    void updatePosition(float x, float y, float z);
    
    void movement(ShaderProgram* program, Entity *other, float elapsed);
    
    void updateIndex(int in);
    
    void updateY(float yC);
    
    void resetCollisionFlags();
    
    bool isColliding(Entity *other);
    
    float xCollisionHandling(Entity *other);
    
    float yCollisionHandling(Entity *other);
    
    void tileCollision(ReadTileMap rTM);
    
    void setMatrices(ShaderProgram *program);
    
    void drawSprite(ShaderProgram* program);
    
    void buildTriangle(ShaderProgram *program);
    
    void render(ShaderProgram *program, float elapsed);
    
    void worldToTileCoordinates(float worldX, float worldY, int *gridX, int gridY);
};

#endif /* Entities_hpp */
