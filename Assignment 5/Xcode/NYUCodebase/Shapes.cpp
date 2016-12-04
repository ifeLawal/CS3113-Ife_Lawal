//
//  NYUCB.cpp
//  NYUCodebase
//
//  Created by Ifeoluwa Lawal on 12/3/16.
//  Copyright © 2016 Ivan Safrin. All rights reserved.
//

#include "Shapes.h
"
void Shapes::buildTriangle(ShaderProgram *program) {
    setMatrices(program);
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(program->positionAttribute);
}


void Shapes::render(ShaderProgram *program, float elapsed) {
    xVelocity = lerp(xVelocity, 0.0f, elapsed * xFric);
    yVelocity = lerp(yVelocity, 0.0f, elapsed * yFric);
    
    xVelocity += xAccle * elapsed;
    yVelocity += yAccle * elapsed;
    
    xTrans += xVelocity * elapsed;
    yTrans += yVelocity * elapsed;
    
    updatePosition(xTrans, yTrans, zTrans);
}
