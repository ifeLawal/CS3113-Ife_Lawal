//
//  NYUCB.cpp
//  NYUCodebase
//
//  Created by Ifeoluwa Lawal on 12/3/16.
//  Copyright © 2016 Ivan Safrin. All rights reserved.
//

#include "Shape.h"


Shape::Shape(){

}

Shape::Shape(float wVal, float hVal, float size, bool st, int numSides) : width(wVal), height(hVal)
    , isStatic(st), sides(numSides){
        buildVerts();
        
}

Shape::Shape(float sz, bool st, std::vector<Vector> verts) : size(sz), isStatic(st){
    
    for(int i=0; i<verts.size();i++) {
        verts[i].x *= size;
        verts[i].y *= size;
        vertices.push_back(verts[i]);
    }
    if(isStatic) {
        xVelocity = 2*((float)rand()/ (float)RAND_MAX - 1);
        yVelocity = 2*((float)rand()/ (float)RAND_MAX - 1);
        printf("%f\n", xVelocity);
        printf("%f\n", yVelocity);
    }
    xFric = 1;
    yFric = 1;
}


void Shape::changeHollow(bool h) {
    hollow = h;
}

float Shape::lerp (float v0, float v1, float t) {
    return (1.0-t)*v0 + t*v1;
}

void Shape::setPosition(float x, float y, float z) {
    modelMatrix.identity();
    xTrans = x;
    yTrans = y;
    zTrans = z;
    modelMatrix.Translate(xTrans, yTrans, zTrans);
}

void Shape::updatePosition(float x, float y, float z) {
    modelMatrix.identity();
    xTrans += x;
    yTrans = yTrans + y;
    zTrans = zTrans + z;
    modelMatrix.Translate(xTrans, yTrans, zTrans);
    //printf("%f", xTrans);
}

void Shape::setMatrices(ShaderProgram* program) {
    program->setModelMatrix(modelMatrix);
}

void Shape::buildVerts() {
    float pastX = 0;
    float pastY = 0;
    float minX = 0;
    for(int i = 0; i < sides;i++) {
        float x = width * ((float)rand()/ (float)RAND_MAX);
        float y = height *((float)rand()/ (float)RAND_MAX);
        if(i < sides/2) {
            if(x < pastX) {
                x += pastX;
            }
        } else if (i == sides/2) {
            x += width;
        }else {
            if(x > pastX && minX > x-pastX) {
                x -= pastX;
            } else {
                x = pastX + .01;
            }
        }
        if(y < pastY) {
            y += pastY;
        }
        Vector point(x, y);
        vertices.push_back(point);
        pastX = x;
        pastY = y;
    }
}
void Shape::updateVertPos() {
    for(int i = 0; i < vertices.size(); i++) {
        Vector point(vertices[i].x+xTrans, vertices[i].y+yTrans);
        if(i >= vertPos.size()) {
            vertPos.push_back(point);
        } else{
            vertPos[i] = point;
        }
    }
}


void Shape::buildShape(ShaderProgram *program) {
    std::vector<float> verts;
    for(int i = 0; i<vertices.size();i++) {
        verts.push_back(vertices[i].x);
        verts.push_back(vertices[i].y);
    }
    setMatrices(program);
    
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, verts.data());
    glEnableVertexAttribArray(program->positionAttribute);
    
    
    glDrawArrays(GL_LINE_LOOP, 0, (int)vertices.size());
    
    glDisableVertexAttribArray(program->positionAttribute);
}

void Shape::movement(ShaderProgram* program, float elapsed) {
    if(isStatic) {
        if(xTrans > 4.0 || xTrans < -4.0) {xVelocity = -xVelocity;}
        if(yTrans > 2.0 || yTrans < -3.0) {yVelocity = -yVelocity;}
        updatePosition(xVelocity * elapsed, yVelocity * elapsed, 0);
    }
    else {
        yVelocity = lerp(yVelocity, 0.0f, elapsed * yFric);
        yVelocity += (yAccle + yGravity) * elapsed;
        yTrans += yVelocity * elapsed;
        
        xVelocity = lerp(xVelocity, 0.0f, elapsed * xFric);
        xVelocity += xAccle * elapsed;
        xTrans += xVelocity * elapsed;
        printf("%f\n", xVelocity);
        
        
        if(yAccle == 0 && yVelocity < 0.1 && yVelocity > -0.1) {yVelocity = 0;}
        if(xAccle == 0 && xVelocity < 0.1 && xVelocity > -0.1) {xVelocity = 0;}
        
        setPosition(xTrans, yTrans, 0);
    }
    updateVertPos();
}

void Shape::render(ShaderProgram *program, float elapsed) {
    xVelocity = lerp(xVelocity, 0.0f, elapsed * xFric);
    yVelocity = lerp(yVelocity, 0.0f, elapsed * yFric);
    
    xVelocity += xAccle * elapsed;
    yVelocity += yAccle * elapsed;
    
    xTrans += xVelocity * elapsed;
    yTrans += yVelocity * elapsed;
    
    updatePosition(xTrans, yTrans, zTrans);
}
