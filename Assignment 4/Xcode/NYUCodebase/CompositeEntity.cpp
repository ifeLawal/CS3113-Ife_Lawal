//
//  Composite Entity.cpp
//  NYUCodebase
//
//  Created by Ifeoluwa Lawal on 11/2/16.
//  Copyright © 2016 Ivan Safrin. All rights reserved.
//

#include "CompositeEntity.h"

CompositeEntity::CompositeEntity(Entity base, Entity sub) : baseEntity(base), subEntity(sub) {
    baseEntity.setPosition(baseEntity.xTrans, baseEntity.yTrans, baseEntity.zTrans);
    subEntity.setPosition(subEntity.xTrans, subEntity.yTrans, subEntity.zTrans);
    yGravity = -0.81;
}

void CompositeEntity::updateYPosition(Entity e, float elapsed) {
    e.yVelocity = lerp(yVelocity, 0.0f, elapsed * yFric);
    e.yVelocity += (yAccle + yGravity) * elapsed;
    e.yTrans += yVelocity * elapsed;
    //baseEntity.updatePosition(x, y, z);
    //subEntity.updatePosition(x, y, z);
}

void CompositeEntity::updateXPosition(Entity e, float elapsed) {
    e.xVelocity = lerp(xVelocity, 0.0f, elapsed * yFric);
    e.xVelocity += xAccle * elapsed;
    e.xTrans += xVelocity * elapsed;
    //baseEntity.updatePosition(x, y, z);
    //subEntity.updatePosition(x, y, z);
}

void CompositeEntity::movement(ShaderProgram *program, Entity *other, float elapsed) {
    if(!isStatic) {
        updateYPosition(baseEntity, elapsed);
        if(baseEntity.yCollisionHandling(other) > 0) {
            baseEntity.yTrans += baseEntity.yCollisionHandling(other);
            subEntity.yTrans += baseEntity.yCollisionHandling(other);
        }
        updateYPosition(subEntity, elapsed);
        if(subEntity.yCollisionHandling(other) > 0) {
            baseEntity.yTrans += baseEntity.yCollisionHandling(other);
            subEntity.yTrans += baseEntity.yCollisionHandling(other);
        }
        updateXPosition(baseEntity, elapsed);
        if(baseEntity.xCollisionHandling(other) > 0) {
            baseEntity.xTrans += baseEntity.xCollisionHandling(other);
            subEntity.xTrans += baseEntity.xCollisionHandling(other);
        }
        updateXPosition(subEntity, elapsed);
        if(subEntity.xCollisionHandling(other) > 0) {
            baseEntity.xTrans += baseEntity.xCollisionHandling(other);
            subEntity.xTrans += baseEntity.xCollisionHandling(other);
        }
        
        baseEntity.updatePosition(baseEntity.xTrans, baseEntity.yTrans, baseEntity.zTrans);
        subEntity.updatePosition(subEntity.xTrans, subEntity.yTrans, subEntity.zTrans);
    }
}

bool CompositeEntity::isColliding(Entity *other) {
    return baseEntity.isColliding(other);
    return subEntity.isColliding(other);
    
    return false;
}

void CompositeEntity::setMatrices(ShaderProgram* program) {
    baseEntity.setMatrices(program);
    subEntity.setMatrices(program);
}

void CompositeEntity::drawSprite(ShaderProgram* program){
    baseEntity.drawSprite(program);
    subEntity.drawSprite(program);
    
}
