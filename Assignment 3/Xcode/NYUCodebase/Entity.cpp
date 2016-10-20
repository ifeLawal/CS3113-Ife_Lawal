//
//  Entities.cpp
//  NYUCodebase
//
//  Created by Ifeoluwa Lawal on 10/18/16.
//  Copyright © 2016 Ivan Safrin. All rights reserved.
//

#include "Entity.h"

Entity::Entity(float wVal, float hVal, GLuint sheet, const int lvl = 0) : width(wVal), height(hVal), xTrans(0), yTrans(0), zTrans(0), spriteSheet(sheet), level(lvl) {
    float hw = width/2;
    float hh = height/2;
    //float verts[] = {-hw, -hh, hw, -hh, hw, hh, -hw, -hh, -hw, hh, hw, hh};
    float verts[] = {-hw, -hh, hw, -hh, hw, hh,  -hw, -hh, hw, hh, -hw, hh};
    //float verts[] = {-hw, -hh, hw, -hh, hw, hh};
    memcpy(vertices, verts, sizeof(vertices));
    //printf("%lf", vertices[0]);
    xTrans = 0;
    yTrans = 0;
    zTrans = 1;
    active = false;
}

Entity::Entity(const Entity &other) : width(other.width), height(other.height), xTrans(other.xTrans), yTrans(other.yTrans), zTrans(other.zTrans), spriteSheet(other.spriteSheet), level(other.level), active(other.active),spriteCountX(other.spriteCountX), spriteCountY(other.spriteCountY), index(other.index){
    memcpy(vertices, other.vertices, sizeof(other.vertices));
}

void Entity::setUpSpriteSheet(int in, int xCount, int yCount) {
    index = in;
    spriteCountX = xCount;
    spriteCountY = yCount;
}

void Entity::setTrans(float x, float y, float z) {
    modelMatrix.identity();
    xTrans = x;
    yTrans = y;
    zTrans = z;
    modelMatrix.Translate(xTrans, yTrans, zTrans);
}

void Entity::updateIndex(int in) {
    index = in;
}

void Entity::updatePosition(float x, float y, float z) {
    modelMatrix.identity();
    xTrans += x;
    yTrans = yTrans + y;
    zTrans = zTrans + z;
    modelMatrix.Translate(xTrans, yTrans, zTrans);
    //printf("%f", xTrans);
}

bool Entity::isColliding(Entity other) {
    if(yTrans - height/2 < other.yTrans + other.height/2) {
        if(xTrans + width/2 > other.xTrans && xTrans - width/2 < other.xTrans) {
            return true;
        }
    }
    return false;
}

void Entity::setMatrices(ShaderProgram* program) {
    program->setModelMatrix(modelMatrix);
}

void Entity::drawSprite(ShaderProgram* program){
    setMatrices(program);
    
    float texCoords[] = {0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0};
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);
    
    
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);
    
    glBindTexture(GL_TEXTURE_2D, spriteSheet);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(program->positionAttribute);
    
    glDisableVertexAttribArray(program->texCoordAttribute);
    
}

void Entity::DrawSpriteSheetSprite(ShaderProgram *program, int index, int spriteCountX, int spriteCountY) {
    setMatrices(program);
    
    float u = (float)(((int)index) % spriteCountX) / (float) spriteCountX;
    float v = (float)(((int)index) / spriteCountX) / (float) spriteCountY;
    float spriteWidth = 1.0/(float)spriteCountX;
    float spriteHeight = 1.0/(float)spriteCountY;
    
    GLfloat texCoords [] = {
        u, v+spriteHeight,
        u+spriteWidth, v+spriteHeight,
        u+spriteWidth, v,
        u, v+spriteHeight,
        u+spriteWidth, v,
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
    
    
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);
    
    
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);
    
    glBindTexture(GL_TEXTURE_2D, spriteSheet);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(program->positionAttribute);
    
    glDisableVertexAttribArray(program->texCoordAttribute);
}

void Entity::buildTriangle(ShaderProgram *program) {
    setMatrices(program);
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(program->positionAttribute);
}

void Entity::render(ShaderProgram *program) {
    DrawSpriteSheetSprite(program, index, spriteCountX, spriteCountY);
}



