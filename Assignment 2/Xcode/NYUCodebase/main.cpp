#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "ShaderProgram.h"
#include "Matrix.h"

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

SDL_Window* displayWindow;

const float heightRatio = 1.0;
const float wideRatio = heightRatio*(16/9);
const float PI = 3.14159265;
float randXVelocity = 0.0f;
float randYVelocity = 0.0f;
float angle;

class gamePiece {
public:
    float xTrans, yTrans, zTrans;
    float width;
    float height;
    Matrix modelMatrix;
    Matrix viewMatrix;
    Matrix projectionMatrix;
    float vertices[12];
    
    gamePiece(float wVal, float hVal) : width(wVal), height(hVal), xTrans(0), yTrans(0), zTrans(0) {
        float hw = width/2;
        float hh = height/2;
        float verts[] = {-hw, -hh, hw, -hh, hw, hh, -hw, -hh, -hw, hh, hw, hh};
        //float verts[] = {-hw, -hh, hw, -hh, hw, hh};
        memcpy(vertices, verts, sizeof(vertices));
        //printf("%lf", vertices[0]);
    }
    void setTrans(float x, float y, float z) {
        xTrans = x;
        yTrans = y;
        zTrans = z;
        modelMatrix.identity();
        modelMatrix.Translate(xTrans, yTrans, zTrans);
    }
    
    void updateY(float yC) {
        modelMatrix.identity();
        float yTop = yTrans + height/2;
        float yBottom = yTrans - height/2;
        if(yTop + yC < heightRatio - heightRatio/15 && yBottom + yC > -heightRatio + heightRatio/15) {
            modelMatrix.Translate(xTrans, yC + yTrans, zTrans);
            yTrans = yC+yTrans;
        } else{
            modelMatrix.Translate(xTrans, yTrans, zTrans);
        }
    }
    
    void ballUpdateY(float yC) {
        modelMatrix.identity();
        modelMatrix.Translate(xTrans, yC + yTrans, zTrans);
    }
    
    void updateX(float xC) {
        modelMatrix.identity();
        modelMatrix.Translate(xC + xTrans, yTrans, zTrans);
        xTrans = xC + xTrans;
    }
    void rotate(float r) {
        //modelMatrix.identity();
        modelMatrix.Rotate(r);
    }
    void setMatrices(ShaderProgram program) {
        program.setModelMatrix(modelMatrix);
        program.setProjectionMatrix(projectionMatrix);
        program.setViewMatrix(viewMatrix);
        
    }
    
    
    
    void buildTraingle(ShaderProgram program) {
        setMatrices(program);
        glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
        glEnableVertexAttribArray(program.positionAttribute);
        
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        glDisableVertexAttribArray(program.positionAttribute);
    }
    
};

bool collision (gamePiece obj1, gamePiece obj2, bool collisionLeft) {
    float obj1Top = obj1.yTrans + obj1.height/2;
    float obj1Bottom = obj1.yTrans - obj1.height/2;
    float obj1Right = obj1.xTrans + obj1.width/2;
    float obj1Left = obj1.xTrans - obj1.width/2;
    float obj2Top = obj2.yTrans + obj2.height/2;
    float obj2Bottom = obj2.yTrans - obj2.height/2;
    float obj2Right = obj2.xTrans + obj2.width/2;
    float obj2Left = obj2.xTrans - obj2.width/2;
    /*
     if(obj2Bottom > obj1Top){}
    if(obj2Top > obj1Bottom){}
     */
    if(collisionLeft && obj1Right > obj2Left) {
        printf("working\n");
        //obj1Top > obj2Top && obj1Bottom < obj2Bottom)
        if(!(obj1Top < obj2Bottom || obj1Bottom > obj2Top)){
            printf("%f,", obj1Top);
            printf("%f,", obj2Top);
            printf("%f,", obj1Bottom);
            printf("%f\n", obj2Bottom);
            return true;
        }
    }
    if(!collisionLeft && obj1Left < obj2Right) {
        printf("working\n");
        if(!(obj1Top < obj2Bottom || obj1Bottom > obj2Top)) {
            printf("%f,", obj1Top);
            printf("%f,", obj2Top);
            printf("%f,", obj1Bottom);
            printf("%f\n", obj2Bottom);
            return true;
        }
    }
    return false;
}

void makeGoodRand() {
    while(randXVelocity == 0 || ((abs(randXVelocity) > .3 && abs(randXVelocity) < .1)) && (abs(randYVelocity) > .3 && abs(randYVelocity) <.1)){
        srand(time(NULL));
        randXVelocity = (rand() % 100) - 50;
        randYVelocity = (rand() % 100) - 50;
        randXVelocity = randXVelocity/50;
        randYVelocity = randYVelocity/50;
        printf("%f", randXVelocity);
        printf("%f", randYVelocity);
    }
}

void setup() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 360, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
#ifdef _WINDOWS
    glewInit();
#endif

    
}

int main(int argc, char *argv[])
{
    setup();
    ShaderProgram program (RESOURCE_FOLDER"vertex.glsl", RESOURCE_FOLDER"fragment.glsl");
    
    Matrix modelMatrix;
    Matrix viewMatrix;
    Matrix projectionMatrix;
    
    projectionMatrix.setOrthoProjection(-wideRatio, wideRatio, -heightRatio, heightRatio, -1.0, 1.0);
    
    float lastFrameTicks = 0.0f;
    float y1Speed = 0;
    float y2Speed = 0;
    
    //float vertices5[] = {0.5, -0.5, 0.0, 0.5, -0.5, -0.5};
    gamePiece pad1(.03, .4);
    pad1.setTrans(-.95, 0, 0);
    gamePiece pad2(.03, .4);
    pad2.setTrans(.95, 0, 0);
    
    gamePiece ball(.03, .06);
    ball.setTrans(0, 0, 0);
    
    gamePiece topBlock(2*wideRatio, heightRatio/20);
    topBlock.setTrans(0,heightRatio-(heightRatio/20),0);
    gamePiece bottomBlock(2*wideRatio, heightRatio/20);
    bottomBlock.setTrans(0,-heightRatio+(heightRatio/20),0);
    
    float widthRatio = heightRatio*9/16;
    float width = widthRatio/15;
    float height = heightRatio/15;
    gamePiece middleBlocks1(width, height);
    middleBlocks1.setTrans(0,heightRatio-(heightRatio/7),0);
    gamePiece middleBlocks2(width, height);
    middleBlocks2.setTrans(0,heightRatio-2*(heightRatio/7),0);
    gamePiece middleBlocks3(width, height);
    middleBlocks3.setTrans(0,heightRatio-3*(heightRatio/7),0);
    gamePiece middleBlocks4(width, height);
    middleBlocks4.setTrans(0,heightRatio-4*(heightRatio/7),0);
    gamePiece middleBlocks5(width, height);
    middleBlocks5.setTrans(0,heightRatio-5*(heightRatio/7),0);
    gamePiece middleBlocks6(width, height);
    middleBlocks6.setTrans(0,heightRatio-6*(heightRatio/7),0);
    gamePiece middleBlocks7(width, height);
    middleBlocks7.setTrans(0,heightRatio-7*(heightRatio/7),0);
    gamePiece middleBlocks8(width, height);
    middleBlocks8.setTrans(0,heightRatio-8*(heightRatio/7),0);
    gamePiece middleBlocks9(width, height);
    middleBlocks9.setTrans(0,heightRatio-9*(heightRatio/7),0);
    gamePiece middleBlocks10(width, height);
    middleBlocks10.setTrans(0,heightRatio-10*(heightRatio/7),0);
    gamePiece middleBlocks11(width, height);
    middleBlocks11.setTrans(0,heightRatio-11*(heightRatio/7),0);
    gamePiece middleBlocks12(width, height);
    middleBlocks12.setTrans(0,heightRatio-12*(heightRatio/7),0);
    gamePiece middleBlocks13(width, height);
    middleBlocks13.setTrans(0,heightRatio-13*(heightRatio/7),0);
    gamePiece middleBlocks14(width, height);
    middleBlocks14.setTrans(0,heightRatio-14*(heightRatio/7),0);
    
    makeGoodRand();
    
    SDL_Event event;
    bool done = false;
    while (!done) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
                done = true;
            } if(event.type == SDL_KEYDOWN) {
                if(event.key.keysym.scancode == SDL_SCANCODE_W) {
                    y1Speed = 1;
                }
                else if(event.key.keysym.scancode == SDL_SCANCODE_S) {
                    y1Speed = -1;
                }
                if(event.key.keysym.scancode == SDL_SCANCODE_UP) {
                    y2Speed = 1;
                }
                else if(event.key.keysym.scancode == SDL_SCANCODE_DOWN) {
                    y2Speed = -1;
                }
            }if(event.type == SDL_KEYUP) {
                if(event.key.keysym.scancode == SDL_SCANCODE_W) {
                    y1Speed = 0;
                }
                else if(event.key.keysym.scancode == SDL_SCANCODE_S) {
                    y1Speed = 0;
                }
                if(event.key.keysym.scancode == SDL_SCANCODE_UP) {
                    y2Speed = 0;
                }
                else if(event.key.keysym.scancode == SDL_SCANCODE_DOWN) {
                    y2Speed = 0;
                }
            }
        }
        
        float ticks = (float)SDL_GetTicks()/1000.0f;
        float elapsed = ticks - lastFrameTicks;
        lastFrameTicks = ticks;
        //printf("%f", elapsed);
        pad1.updateY(y1Speed*elapsed);
        pad2.updateY(y2Speed*elapsed);
        ball.ballUpdateY(randXVelocity*elapsed);
        ball.updateX(randYVelocity*elapsed);
        modelMatrix.Rotate(PI*elapsed);
        if(collision(pad1, ball, true) || collision(pad2, ball, false)) {
            randYVelocity = -randYVelocity + y1Speed*elapsed;
            randXVelocity = randXVelocity + y1Speed*elapsed;

        }
        if(ball.xTrans > wideRatio || ball.xTrans < -wideRatio) {
            ball.setTrans(0, 0, 0);
            makeGoodRand();
        }
        if((ball.yTrans + height > heightRatio || ball.yTrans - height < -heightRatio)) {
            randXVelocity = -randXVelocity;
            
            //randXVelocity = -randXVelocity;
        }
        
        glClear(GL_COLOR_BUFFER_BIT);
        
        //loop
        pad1.buildTraingle(program);
        pad2.buildTraingle(program);
        ball.buildTraingle(program);
        topBlock.buildTraingle(program);
        bottomBlock.buildTraingle(program);
        middleBlocks1.buildTraingle(program);
        middleBlocks2.buildTraingle(program);
        middleBlocks3.buildTraingle(program);
        middleBlocks4.buildTraingle(program);
        middleBlocks5.buildTraingle(program);
        middleBlocks6.buildTraingle(program);
        middleBlocks7.buildTraingle(program);
        middleBlocks8.buildTraingle(program);
        middleBlocks9.buildTraingle(program);
        middleBlocks10.buildTraingle(program);
        middleBlocks11.buildTraingle(program);
        middleBlocks12.buildTraingle(program);
        middleBlocks13.buildTraingle(program);
        //middleBlocks14.buildTraingle(program);
        
        
        SDL_GL_SwapWindow(displayWindow);
    }
    
    SDL_Quit();
    return 0;
}
