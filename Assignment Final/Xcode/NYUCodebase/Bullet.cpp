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
}

void Bullet::movement(ShaderProgram *program, Entity *other, float elapsed) {
    
    //xVelocity = lerp(xVelocity, 0.0f, elapsed * xFric);
    //xVelocity += xAccle * elapsed;
    xTrans += xVelocity * elapsed;
    xTrans += xCollisionHandling(other);
    
    setPosition(xTrans, yTrans, zTrans);
}

void Bullet::xtileCollision(ReadTileMap rTM) {
        
}
