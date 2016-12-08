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
#include "ReadTileMap.h"
#include "Matrix.h"
#include "ShaderProgram.h"
#include <math.h>

struct Vector {
    float x;
    float y;
    Vector(float xU, float yU) {
        x = xU;
        y = yU;
    }
    
    Vector(const Vector& vc) {
        x = vc.x;
        y = vc.y;
    }
    
    void normalize() {
        float len = sqrtf(x*x + y*y);
        x = x/len;
        y = y/len;
    }
};

class Shape {
    
public:
    float xTrans, yTrans, zTrans;
    float width;
    float height;
    float size;
    float animationRate;
    int sides;
    //int level, xPos;
    Matrix modelMatrix;
    std::vector<Vector> vertices;
    std::vector<Vector> vertPos;
    //std::vector<<#class _Tp#>>
    
    int collisionAmt;
    
    bool isStatic;
    bool hollow;
    //EntityType entityType;
    
    float xFric, yFric;
    float xVelocity, yVelocity, zVelocity;
    float xAccle, yAccle, zAccle, yGravity;
    
    bool collidedTop;
    bool collidedBottom;
    bool collidedLeft;
    bool collidedRight;
    bool landed;
    
    Shape();
    
    float lerp (float v0, float v1, float t);
    
    Shape(float wVal, float hVal, float size, bool st, int sides);
    
    Shape(float size, bool st, std::vector<Vector> vertices);
    
    //Shape::Shape(float wVal, float hVal, float size, bool st, int numSides);
    
    Shape(const Shape &other);
    
    void changeHollow(bool h);
    
    void buildVerts();
    
    void updateVertPos();
    
    void setPosition(float x, float y, float z);
    
    void updatePosition(float x, float y, float z);
    
    void movement(ShaderProgram* program, float elapsed);
    
    bool isColliding(Shape *other);
    
    void setMatrices(ShaderProgram *program);
    
    void buildShape(ShaderProgram *program);
    
    void render(ShaderProgram *program, float elapsed);
    
};


#endif /* NYUCB_hpp */
