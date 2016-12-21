//
//  Bullet.cpp
//  NYUCodebase
//
//  Created by Ifeoluwa Lawal on 12/20/16.
//  Copyright © 2016 Ivan Safrin. All rights reserved.
//

#include "Bullet.h"


Bullet::Bullet(float wVal, float hVal, float s, bool st, EntityType eType, SpriteSheet *sheet) {
    //height(hVal), size(s), isStatic(st), entityType(eType), xTrans(0), yTrans(0), zTrans(1)
    height = hVal;
    size = s;
    isStatic = st;
    entityType = eType;
    xTrans = 0;
    yTrans = 0;
    zTrans = 0;
    width = wVal;
    float hw = size*width/2;
    float hh = size*height/2;
    //float verts[] = {-hw, -hh, hw, -hh, hw, hh, -hw, -hh, -hw, hh, hw, hh};
    float verts[] = {-hw, -hh, hw, -hh, hw, hh,  -hw, -hh, hw, hh, -hw, hh};
    //float verts[] = {-hw, -hh, hw, -hh, hw, hh};
    memcpy(vertices, verts, sizeof(vertices));
    sprite = *sheet;
    //printf("%lf", vertices[0]);
    yGravity = -9.81;
    xFric = 3;
    yFric = .5;
    collisionAmt = 0;
    animationRate = 0;
    xScl = 1;
    yScl = 1;
    zScl = 1;
    reset = true;
}

void Bullet::movement(ShaderProgram *program, Entity *other, float elapsed) {

    xTrans += xVelocity * elapsed;
    xTrans += xCollisionHandling(other);
    
    setPosition(xTrans, yTrans, zTrans);
    //xVelocity = lerp(xVelocity, 0.0f, elapsed * xFric);
    //xVelocity += xAccle * elapsed;
    
}
void Bullet::checkReset() {
    if(xVelocity > 0.05 || xVelocity < -.05) {
        reset = false;
    } else {
        reset = true;
    } if (xTrans == 0) {
        reset = true;
    }
}

void Bullet::movement(ShaderProgram *program, ReadTileMap rTM, Entity *other, int &score, float elapsed) {
    checkReset();
    if(xtileCollision(rTM)) {
        setPosition(0, 0, 0);
        xVelocity = 0;
    } if(isColliding(other)) {
        score++;
        setPosition(0, 0, 0);
        xVelocity = 0;
        for(int i = 0; i < rTM.types.size(); i++) {
            if(rTM.types[i] == other->pType) {
                other->setPosition(rTM.xPosList[i]+width, -rTM.yPosList[i], 0);
            }
        }
    } else {
        xTrans += xVelocity * elapsed;
        
        setPosition(xTrans, yTrans, zTrans);
    }
    //xVelocity = lerp(xVelocity, 0.0f, elapsed * xFric);
    //xVelocity += xAccle * elapsed;
    
}

bool Bullet::xtileCollision(ReadTileMap rTM) {
    float left = xTrans - width/2;
    float right = xTrans + width/2;
    int gridX;
    int gridY;
    rTM.worldToTileCoordinates(right, yTrans, &gridX, &gridY);
    if(gridY > rTM.mapWidth/2 || rTM.isSolid(gridX, gridY)) {
        return true;
    }
    
    rTM.worldToTileCoordinates(left, yTrans, &gridX, &gridY);
    if(rTM.isSolid(gridX, gridY)) {
        return true;
    }
    return false;
}


