//
//  Text.cpp
//  NYUCodebase
//
//  Created by Ifeoluwa Lawal on 10/18/16.
//  Copyright © 2016 Ivan Safrin. All rights reserved.
//
#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "ShaderProgram.h"
#include <stdio.h>
#include <vector>
#include "Matrix.h"
#include "Text.h"


    
Text::Text(const std::string& tex, GLuint fontSheet, int size, int spacing) : text(tex), fontTexture(fontSheet) {
    this->size = size;
    this->spacing = spacing;
    setPosition(xTrans, yTrans, zTrans);
    float width = text.size() * (size + spacing);
    xCenter = width/2;
    xTrans = 0;
    yTrans = 0;
    zTrans = 0;
    yCenter = 0.5;
}

void Text::setMatrices(ShaderProgram *program) {
    program->setModelMatrix(modelMatrix);
}

void Text::setPosition(float x, float y, float z) {
    xTrans = x;
    yTrans = y;
    zTrans = z;
    //modelMatrix.Translate(xTrans, yTrans, zTrans);
}

void Text::updatePosition(float x, float y, float z) {
    xTrans += x;
    yTrans += y;
    zTrans += z;
    //modelMatrix.Translate(xTrans, yTrans, zTrans);
}

void Text::changeText(std::string nStr) {
    text = nStr;
}

void Text::DrawText(ShaderProgram *program, Matrix* modelMat) {
    //program->setModelMatrix(*modelMat);

    //modelMat->identity();
    modelMat->Translate(-xCenter+xTrans, yTrans, zTrans);
    program->setModelMatrix(*modelMat);

    
    float texture_size = 1.0/16.0f;
    std::vector<float> vertexData;
    std::vector<float> texCoordData;
        
    for(int i=0; i < text.size(); i++) {
        float texture_x = (float)(((int)text[i]) % 16) / 16.0f;
        float texture_y = (float)(((int)text[i]) / 16) / 16.0f;
        vertexData.insert(vertexData.end(), {
            ((size+spacing) * i) + (-0.5f * size), 0.5f * size,
            ((size+spacing) * i) + (-0.5f * size), -0.5f * size,
            ((size+spacing) * i) + (0.5f * size), 0.5f * size,
            ((size+spacing) * i) + (0.5f * size), -0.5f * size,
            ((size+spacing) * i) + (0.5f * size), 0.5f * size,
            ((size+spacing) * i) + (-0.5f * size), -0.5f * size,
        });
        texCoordData.insert(texCoordData.end(), {
            texture_x, texture_y,
            texture_x, texture_y + texture_size,
            texture_x + texture_size, texture_y,
            texture_x + texture_size, texture_y + texture_size,
            texture_x + texture_size, texture_y,
            texture_x, texture_y + texture_size,
        });
    }
    //printf("working");
    glUseProgram(program->programID);
    
    //modelMatrix.Translate(xTrans, yTrans, zTrans);
    
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertexData.data());
    glEnableVertexAttribArray(program->positionAttribute);
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoordData.data());
    glEnableVertexAttribArray(program->texCoordAttribute);
    
    //printf("%i", text.size());
    glBindTexture(GL_TEXTURE_2D, fontTexture);
    glDrawArrays(GL_TRIANGLES, 0, text.size() * 6);
        
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
    
    //program->setModelMatrix(*modelMat);
    modelMat->identity();
    
}
