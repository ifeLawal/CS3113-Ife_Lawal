//
//  Entities.cpp
//  NYUCodebase
//
//  Created by Ifeoluwa Lawal on 10/18/16.
//  Copyright © 2016 Ivan Safrin. All rights reserved.
//

#include "Entity.h"
#include <math.h>
#define TILE_SIZE 1

Entity::Entity() {
}

Entity::Entity(float wVal, float hVal, float s, bool st, EntityType eType, SpriteSheet *sheet) : width(wVal), height(hVal), size(s), isStatic(st), entityType(eType), xTrans(0), yTrans(0), zTrans(1) {
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
    //active = false;
    someSound = Mix_LoadWAV("RainDrop.wav");
}

Entity::Entity(const Entity &other) : width(other.width), height(other.height), xTrans(other.xTrans), yTrans(other.yTrans), zTrans(other.zTrans) {
    
    memcpy(vertices, other.vertices, sizeof(other.vertices));
    sprite = *(new SpriteSheet(other.sprite));
    
}

float Entity::lerp (float v0, float v1, float t) {
    return (1.0-t)*v0 + t*v1;
}

void Entity::setPosition(float x, float y, float z) {
    modelMatrix.identity();
    xTrans = x;
    yTrans = y;
    zTrans = z;
    modelMatrix.Translate(xTrans, yTrans, zTrans);
}

void Entity::updatePosition(float x, float y, float z) {
    modelMatrix.identity();
    xTrans += x;
    yTrans = yTrans + y;
    zTrans = zTrans + z;
    modelMatrix.Translate(xTrans, yTrans, zTrans);
    //printf("%f", xTrans);
}

void Entity::resetCollisionFlags() {
    collidedBottom = false;
    collidedTop = false;
    collidedLeft = false;
    collidedRight = false;
}

void Entity::movement(ShaderProgram *program, Entity *other, float elapsed) {
    
    if(!isStatic) {
        if(xVelocity < 0) {
            xScl = -1;
        } else {
            xScl = 1;
        }
        landed = false;
        animationRate = animationRate + 10 * elapsed;
        //printf("\n%f", animationRate);
        if(collidedBottom == true) {
            if(xVelocity < 0.1 && xVelocity > -0.1) {
                if(animationRate / 2.3 > 1.0) {
                    sprite.animateIdle();
                    animationRate = 0;
                }
            } else {
                if(animationRate / 1.8 > 1.0) {
                    sprite.animateWalk();
                    animationRate = 0;
                }
            }
        } else {
            sprite.index = sprite.idleSprites[0];
        }
        
        
        yVelocity = lerp(yVelocity, 0.0f, elapsed * yFric);
        yVelocity += (yAccle + yGravity) * elapsed;
        yTrans += yVelocity * elapsed;
        yTrans += yCollisionHandling(other);
        
        
        

        if(landed) {printf("landed");}
        
        xVelocity = lerp(xVelocity, 0.0f, elapsed * xFric);
        xVelocity += xAccle * elapsed;
        xTrans += xVelocity * elapsed;
        xTrans += xCollisionHandling(other);
        
        
        
        setPosition(xTrans, yTrans, zTrans);
        //modelMatrix.Scale(xScl, <#float y#>, <#float z#>)
//        printf("y: %lf / ", yTrans);
//        printf("x: %lf\n", xTrans);
    }
}

void Entity::movement(ShaderProgram* program, ReadTileMap rTM, Entity *other, float elapsed) {
    if(!isStatic) {
        if(xVelocity < 0) {
            xScl = -1;
        } else {
            xScl = 1;
        }
        landed = false;
        animationRate = animationRate + 10 * elapsed;
        //printf("\n%f", animationRate);
        if(collidedBottom == true) {
            if(xVelocity < 0.1 && xVelocity > -0.1) {
                if(animationRate / 1.4 > 1.0) {
                    sprite.animateIdle();
                    animationRate = 0;
                }
            } else {
                if(animationRate / 1.8 > 1.0) {
                    sprite.animateWalk();
                    animationRate = 0;
                }
            }
        } else {
            sprite.index = sprite.idleSprites[0];
        }
        /*if(animationRate)
        if(animationRate / 1.8 > 1.0) {
            animationRate = 0;
            if(collidedBottom == true) {
                if(xVelocity < 0.1 && xVelocity > -0.1) {
                    sprite.animateIdle();
                } else {
                    sprite.animateWalk();
                }
            }
        }*/
        
        
        yVelocity = lerp(yVelocity, 0.0f, elapsed * yFric);
        yVelocity += (yAccle + yGravity) * elapsed;
        yTrans += yVelocity * elapsed;
        yTrans += yCollisionHandling(other);
        ytileCollision(rTM);
        
        
        
        if(landed) {Mix_PlayChannel(-1, someSound, 0);}
        
        xVelocity = lerp(xVelocity, 0.0f, elapsed * xFric);
        xVelocity += xAccle * elapsed;
        xTrans += xVelocity * elapsed;
        xTrans += xCollisionHandling(other);
        xtileCollision(rTM);
        
        
        setPosition(xTrans, yTrans, zTrans);
    }

}

bool Entity::isColliding(Entity *other) {
    float bottom1 = yTrans - height/2;
    float top1 = yTrans + height/2;
    float right1 = xTrans + width/2;
    float left1 = xTrans - width/2;
    float bottom2 = other->yTrans - other->height/2;
    float top2 = other->yTrans + other->height/2;
    float right2 = other->xTrans + other->width/2;
    float left2 = other->xTrans - other->height/2;
    if(other->entityType != ENTITY_BACK){
        if(bottom1 > top2 || top1 < bottom2 || left1 > right2 || right1 < left2 ) {
            return false;

        }
               // printf("collision\n");
        if(bottom1 < top2) {
            collidedBottom = true;
        } else{
            collidedBottom = false;
        }
        if(top1 > bottom2) {
            collidedTop = true;
        } else{
            collidedTop = false;
        }
        if(right1 > left2) {
            collidedRight = true;
        } else{
            collidedRight = false;
        }
        if(left1 < right2) {
            collidedLeft = true;
        } else{
            collidedLeft = false;
        }
        
    //box 1s top is lower than box 2's bottom
    //box 1s bottom is higher than box 2's top
    //box 1s right is smaller than box 2's left
    //box 1s left is bigger than box 2's left
    //no collision
        return true;
    }
    //resetCollisionFlags();
    return false;
}

float Entity::xCollisionHandling(Entity *other) {
    if(isColliding(other)) {
        float x_distance = fabs(other->xTrans - xTrans);
        float penetration = fabs(x_distance - other->width/2 - width/2);
        float xMove = penetration + .005;
        xVelocity = 0;
        if(collidedRight) {
            return -xMove;
        }
        if(collidedLeft) {
           return xMove;
        }
    }
    return 0;
}

float Entity::yCollisionHandling(Entity *other) {
    bool pastCollidedB = collidedBottom;
    if(isColliding(other)) {
        //printf("collision\n");
        float x_distance = fabs(other->yTrans - yTrans);
        float penetration = fabs(x_distance - other->height/2 - height/2);
        float yMove = penetration + .005;
        yVelocity = 0;
        if(collidedTop) {
            return yMove;
        }
        if(collidedBottom) {
           return -yMove;
        }
    }
    if(pastCollidedB == false && collidedBottom == true) {
        landed = true;
    }
    
    return 0;
}

void Entity::xtileCollision(ReadTileMap rTM) {
    //resetCollisionFlags();
    bool pastCollidedB = collidedBottom;
    float left = xTrans - width/2;
    float right = xTrans + width/2;
    float bottom = yTrans - height/2;
    float top = yTrans + height/2;
    int gridX;
    int gridY;
    rTM.worldToTileCoordinates(left, yTrans, &gridX, &gridY);
    
    if(gridY > rTM.mapHeight || gridY < 0.0 || gridX < 0.0 || rTM.isDeathTile(gridX, gridY)) {
        xVelocity = 0;
        for(int i = 0; i < rTM.types.size(); i++) {
            if(rTM.types[i] == pType) {
                setPosition(rTM.xPosList[i]+width, -rTM.yPosList[i], 0);
            }
            
        }
        return;
    }
    else {
        
        if(rTM.isSolid(gridX, gridY)) {
            collidedLeft = true;
            xVelocity = 0;
            xTrans += ((TILE_SIZE * gridX + TILE_SIZE) - left);
            collisionAmt++;
        } else {collidedLeft =false;}
        
        rTM.worldToTileCoordinates(right, yTrans, &gridX, &gridY);
        if(rTM.isSolid(gridX, gridY)) {
            collidedRight = true;
            xVelocity = 0;
            xTrans -= (right - TILE_SIZE * gridX);
            collisionAmt++;
        } else {collidedRight =false;}

        
    }
    if(pastCollidedB == false && collidedBottom == true) {
        landed = true;
        printf("landed");
    } else {
        landed = false;
    }
    
}

void Entity::ytileCollision(ReadTileMap rTM) {
    //resetCollisionFlags();
    bool pastCollidedB = collidedBottom;
    float left = xTrans - width/2;
    float right = xTrans + width/2;
    float bottom = yTrans - height/2;
    float top = yTrans + height/2;
    int gridX;
    int gridY;
    rTM.worldToTileCoordinates(left, yTrans, &gridX, &gridY);
    
    if(gridY > rTM.mapHeight || gridY < 0.0 || gridX < 0.0 || rTM.isDeathTile(gridX, gridY)) {
        xVelocity = 0;
        for(int i = 0; i < rTM.types.size(); i++) {
            if(rTM.types[i] == pType) {
                setPosition(rTM.xPosList[i]+width, -rTM.yPosList[i], 0);
            }
            
        }
        return;
    }
    else {
        
        rTM.worldToTileCoordinates(xTrans, bottom, &gridX, &gridY);
        if(rTM.isSolid(gridX, gridY)) {
            collidedBottom = true;
            yVelocity = 0;
            yTrans += (-TILE_SIZE * gridY - bottom);
            collisionAmt++;
        } else {collidedBottom =false;}
        
        rTM.worldToTileCoordinates(xTrans, top, &gridX, &gridY);
        if(rTM.isSolid(gridX, gridY)) {
            collidedTop = true;
            yVelocity = 0;
            yTrans -= (top - (-TILE_SIZE * gridY)) + height;
            collisionAmt++;
        } else {collidedTop =false;}
        
        
    }
    if(pastCollidedB == false && collidedBottom == true) {
        landed = true;
        printf("landed");
    } else {
        landed = false;
    }
    
}

void Entity::setMatrices(ShaderProgram* program) {
    program->setModelMatrix(modelMatrix);
}

void Entity::drawSprite(ShaderProgram* program){
    GLuint spriteSheet = sprite.spriteSheet;
    setMatrices(program);
    GLfloat texCoords[12];
    
    
    if(sprite.index > -1) {
        int index = sprite.index;
        int spriteCountX = sprite.spriteCountX;
        int spriteCountY = sprite.spriteCountY;
        glBindTexture(GL_TEXTURE_2D, spriteSheet);
        //printf("we made it");
        float u = (float)(((int)index) % spriteCountX) / (float) spriteCountX;
        float v = (float)(((int)index) / spriteCountX) / (float) spriteCountY;
        float spriteWidth = 1.0/(float)spriteCountX;
        float spriteHeight = 1.0/(float)spriteCountY;
        
        GLfloat texCoordsCp [] = {
            u, v+spriteHeight,
            u+spriteWidth*xScl, v+spriteHeight,
            u+spriteWidth*xScl, v,
            u, v+spriteHeight,
            u+spriteWidth*xScl, v,
            u, v
        };
        
        
        /*
         u, v+spriteHeight,
         u+spriteWidth, v,
         u, v,
         u+spriteWidth, v,
         u, v+spriteHeight,
         u+spriteWidth, v+spriteHeight
         };*/
        
        
        //float vertices[] = {1.5f, -1.5f, 1.0f, 1.5f, -.7f, .7f};
        
        //float texCoords[] = sprite.genCoords();
        
        memcpy(texCoords, texCoordsCp, sizeof(texCoordsCp));

    }
    
    if(sprite.u > 0) {
        float u = sprite.u;
        float v = sprite.v;
        float height = sprite.height;
        float width = sprite.width;
        
        glBindTexture(GL_TEXTURE_2D, spriteSheet);
        
        GLfloat texCoordsCp [] = {
            u, v+height,
            u+width, v+height,
            u+width, v,
            u, v+height,
            u+width, v,
            u, v
        };
        
        memcpy(texCoords, texCoordsCp, sizeof(texCoordsCp));
    }
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);
    
    
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);
    
    glBindTexture(GL_TEXTURE_2D, sprite.spriteSheet);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(program->positionAttribute);
    
    glDisableVertexAttribArray(program->texCoordAttribute);
    
    //printf("%f", xScl);
    //modelMatrix.setScale(xScl, yScl, zScl);

    
}

void Entity::buildTriangle(ShaderProgram *program) {
    setMatrices(program);
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(program->positionAttribute);
}


void Entity::render(ShaderProgram *program, float elapsed) {
    xVelocity = lerp(xVelocity, 0.0f, elapsed * xFric);
    yVelocity = lerp(yVelocity, 0.0f, elapsed * yFric);
    
    xVelocity += xAccle * elapsed;
    yVelocity += yAccle * elapsed;
    
    xTrans += xVelocity * elapsed;
    yTrans += yVelocity * elapsed;
    
    updatePosition(xTrans, yTrans, zTrans);
    //modelMatrix.Scale(xScl, yScl, zScl);
}
