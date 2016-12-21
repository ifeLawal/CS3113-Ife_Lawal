//
//  Text.cpp
//  NYUCodebase
//
//  Created by Ifeoluwa Lawal on 10/18/16.
//  Copyright © 2016 Ivan Safrin. All rights reserved.
//

#include "ShaderProgram.h"
#include "Matrix.h"

class Text {
public:
    
    Text(const std::string& tex, GLuint fontSheet, int size, int spacing);
    std::string text;
    float size, spacing;
    GLuint fontTexture;
    Matrix modelMatrix;
    float xTrans, yTrans, zTrans;
    float xCenter, yCenter;
    
    void setMatrices(ShaderProgram *program);
    void setPosition(float x, float y, float z);
    void updatePosition(float x, float y, float z);
    void DrawText(ShaderProgram *program, Matrix* modelMatrix);
    void changeText(std::string nStr);
};
