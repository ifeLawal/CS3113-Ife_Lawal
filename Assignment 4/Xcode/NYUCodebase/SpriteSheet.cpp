//
//  UniSheetSprite.cpp
//  NYUCodebase
//
//  Created by Ifeoluwa Lawal on 10/24/16.
//  Copyright © 2016 Ivan Safrin. All rights reserved.
//
#include "ShaderProgram.h"
#include "SpriteSheet.h"

SpriteSheet::SpriteSheet() {
    
}

SpriteSheet::SpriteSheet(GLuint sheet, int countX, int countY, int ind) : spriteSheet(sheet), spriteCountX(countX), spriteCountY(countY), index(ind){
    
}

SpriteSheet::SpriteSheet(GLuint sheet, float uVal, float vVal, float h, float w, float s) : spriteSheet(sheet),  u(uVal), v(vVal), height(h), width(w), size(s) {
    
}

SpriteSheet::SpriteSheet(SpriteSheet const &sheet) {
    spriteCountX = sheet.spriteCountX;
    spriteCountY = sheet.spriteCountY;
    index = sheet.index;
    spriteSheet = sheet.spriteSheet;
    u = sheet.u;
    v = sheet.v;
    height = sheet.height;
    width = sheet.width;
    size = sheet.size;
}

GLfloat* SpriteSheet::genCoords() {
    if(index > -1) {
        glBindTexture(GL_TEXTURE_2D, spriteSheet);
        //printf("we made it");
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
        return texCoords;
    }
    
    if(height != 0) {
        
        glBindTexture(GL_TEXTURE_2D, spriteSheet);
    
        GLfloat texCoords [] = {
            u, v+height,
            u+width, v+height,
            u+width, v,
            u, v+height,
            u+width, v,
            u, v
        };
    
        return texCoords;
    }
    printf("No sprite setup");
    return 0;/*
     u, v+spriteHeight,
     u+spriteWidth, v,
     u, v,
     u+spriteWidth, v,
     u, v+spriteHeight,
     u+spriteWidth, v+spriteHeight
     };*/
    
    
    //float vertices[] = {1.5f, -1.5f, 1.0f, 1.5f, -.7f, .7f};
    
}
