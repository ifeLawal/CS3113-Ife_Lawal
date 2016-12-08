#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "ShaderProgram.h"
#include "Matrix.h"
#include "Text.h"
#include "Entity.h"
#include "CompositeEntity.h"
#include "ReadTileMap.h"
#include "Shape.h"


#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

#define FIXED_TIMESTEP 0.01666667f
#define MAX_TIMESTEPS 6

SDL_Window* displayWindow;

const std::string levelFile = "NYUCodebase.app/Contents/Resources/myMap.txt";
const float heightRatio = 1.0*3;
const float wideRatio = heightRatio*(16.0/9.0);
const float aspectRatio = 16.0/9.0;
const float PI = 3.14159265;
float randXVelocity = 0.0f;
float randYVelocity = 0.0f;
float angle;
//const std::string levelFile = "myMap.txt";

Matrix modelMatrix;
Matrix viewMatrix;
Matrix projectionMatrix;

enum GameState {STATE_MAIN_MENU, STATE_GAME_LEVEL};

int state;

GLuint LoadTexture(const char *image_path) {
    SDL_Surface *surface = IMG_Load(image_path);
    
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_BGRA, GL_UNSIGNED_BYTE, surface->pixels);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    SDL_FreeSurface(surface);
    
    return textureID;
}

void DrawText(ShaderProgram *program, GLuint fontTexture, std::string text, float size, float spacing) {

    float texture_size = 1.0/16.0f;
    std::vector<float> vertexData;
    std::vector<float> texCoordData;
    
    for(int i=0; i < text.size(); i++) {
        float texture_x = (float)(((int)text[i]) % 16) / 16.0f;
        float texture_y = (float)(((int)text[i]) / 16) / 16.0f;
        vertexData.insert(vertexData.end(), {
            ((size+spacing) * i) + (-0.5f * size), 0.5f * size * 2,
            ((size+spacing) * i) + (-0.5f * size), -0.5f * size *2,
            ((size+spacing) * i) + (0.5f * size), 0.5f * size *2,
            ((size+spacing) * i) + (0.5f * size), -0.5f * size*2,
            ((size+spacing) * i) + (0.5f * size), 0.5f * size*2,
            ((size+spacing) * i) + (-0.5f * size), -0.5f * size*2,
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
    
    glUseProgram(program->programID);
    
    //modelMatrix.Translate(2, 0, 1);
    
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertexData.data());
    glEnableVertexAttribArray(program->positionAttribute);
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoordData.data());
    glEnableVertexAttribArray(program->texCoordAttribute);
    
    glBindTexture(GL_TEXTURE_2D, fontTexture);
    glDrawArrays(GL_TRIANGLES, 0, text.size() * 6);
    
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
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


bool testSATSeparationForEdge(float edgeX, float edgeY, const std::vector<Vector> &points1, const std::vector<Vector> &points2) {
    float normalX = -edgeY;
    float normalY = edgeX;
    float len = sqrtf(normalX*normalX + normalY*normalY);
    normalX /= len;
    normalY /= len;
    
    std::vector<float> e1Projected;
    std::vector<float> e2Projected;
    
    for(int i=0; i < points1.size(); i++) {
        e1Projected.push_back(points1[i].x * normalX + points1[i].y * normalY);
    }
    for(int i=0; i < points2.size(); i++) {
        e2Projected.push_back(points2[i].x * normalX + points2[i].y * normalY);
    }
    
    std::sort(e1Projected.begin(), e1Projected.end());
    std::sort(e2Projected.begin(), e2Projected.end());
    
    float e1Min = e1Projected[0];
    float e1Max = e1Projected[e1Projected.size()-1];
    float e2Min = e2Projected[0];
    float e2Max = e2Projected[e2Projected.size()-1];
    float e1Width = fabs(e1Max-e1Min);
    float e2Width = fabs(e2Max-e2Min);
    float e1Center = e1Min + (e1Width/2.0);
    float e2Center = e2Min + (e2Width/2.0);
    float dist = fabs(e1Center-e2Center);
    float p = dist - ((e1Width+e2Width)/2.0);
    
    if(p < 0) {
        return true;
    }
    return false;
}

bool checkSATCollision(const std::vector<Vector> &e1Points, const std::vector<Vector> &e2Points) {
    /*
    std::vector<Vector> e1Points (e1.vertices);
    std::vector<Vector> e2Points (e2.vertices);
     */
    for(int i=0; i < e1Points.size(); i++) {
        float edgeX, edgeY;
        
        if(i == e1Points.size()-1) {
            edgeX = e1Points[0].x - e1Points[i].x;
            edgeY = e1Points[0].y - e1Points[i].y;
        } else {
            edgeX = e1Points[i+1].x - e1Points[i].x;
            edgeY = e1Points[i+1].y - e1Points[i].y;
        }
        
        bool result = testSATSeparationForEdge(edgeX, edgeY, e1Points, e2Points);
        
        if(!result) {
            return false;
        }
    }
    for(int i=0; i < e2Points.size(); i++) {
        float edgeX, edgeY;
        
        if(i == e2Points.size()-1) {
            edgeX = e2Points[0].x - e2Points[i].x;
            edgeY = e2Points[0].y - e2Points[i].y;
        } else {
            edgeX = e2Points[i+1].x - e2Points[i].x;
            edgeY = e2Points[i+1].y - e2Points[i].y;
        }
        bool result = testSATSeparationForEdge(edgeX, edgeY, e1Points, e2Points);
        if(!result) {
            return false;
        }
    }
    return true;
}

void colRespone(Shape& e1, Shape& e2) {
    int maxChecks = 10;
    while(checkSATCollision(e1.vertPos, e2.vertPos) && maxChecks > 0) {
        /*
        if(e1.isStatic == true) {
            e1.xVelocity = 0;
            e1.xAccle = 0;
        }
        if(e2.isStatic == true) {
            e2.xVelocity = 0;
            e2.xAccle = 0;
        }*/
        Vector responseVector = Vector(e1.xTrans - e2.xTrans, e1.yTrans - e2.yTrans);
        
        responseVector.normalize();
        
        e1.xTrans += responseVector.x * 0.002;
        e1.yTrans += responseVector.y * 0.002;
        
        e2.xTrans -= responseVector.x * 0.002;
        e2.yTrans -= responseVector.y * 0.002;
        //printf("collision\n");
        maxChecks--;
    }
    
}

int main(int argc, char *argv[])
{
    setup();
    
    ShaderProgram program (RESOURCE_FOLDER"vertex.glsl", RESOURCE_FOLDER"fragment.glsl");
    
    projectionMatrix.setOrthoProjection(-wideRatio, wideRatio, -heightRatio, heightRatio, -1.0, 1.0);
    
    GLuint fontTexture = LoadTexture("font1.png");
    GLuint spriteTexture = LoadTexture("arne_sprites.png");
    GLuint charTexture = LoadTexture("characters_3_02.png");

    int jmpAmt;
    
    state = 0;
    
    float lastFrameTicks = 0.0f;
    
    srand(time(NULL));
    //Shape sTest(.5, .5, .5, false, 8);
    //Shape sTest2(.2, .35, 1, false, 3);
    std::vector<Vector> verts;
    /*
    for(int i = 0; i < 4; i++) {
        float x;
        float y;
        if(i < 4/2) {
            x=i+1;
            y=i+2;
        }else {
            x=i-1;
            y=i-2;
        }
        Vector point (x, y);
        verts.push_back(point);
    }
    */
    
    
    std::vector<Entity*> collidable;
    
    
    float vertices[] = {0.1, 0.1, 0.5, 0.1, 0.5, 0.5, 0.1, 0.5};
    Vector points1(0.1, 0.1);
    Vector points2(.7, -0.1);
    Vector points3(1, 1);
    Vector points4(0.5, 2);
    Vector points5(0.15, 0.5);
    Vector points6(0.0, 0.3);
    verts.push_back(points1);
    verts.push_back(points2);
    verts.push_back(points3);
    
    Shape player(1.0, false, verts);
    
    verts.push_back(points4);
    verts.push_back(points5);
    verts.push_back(points6);
    
    Shape sTest(1.0, true, verts);
    Shape sTest2(1.0, true, verts);
    
    sTest.setPosition(1, 1, 1);
    sTest2.setPosition(-1, 1, 1);
    
    program.setModelMatrix(modelMatrix);
    program.setProjectionMatrix(projectionMatrix);
    program.setViewMatrix(viewMatrix);
    
    SDL_Event event;
    bool done = false;
    while (!done) {
        const Uint8 *keys = SDL_GetKeyboardState(NULL);
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
                done = true;
            }
            /*
            if(event.type == SDL_KEYDOWN) {
                if(event.key.keysym.scancode == SDL_SCANCODE_SPACE && player.collidedBottom == true) {
                    jmpAmt = 0;
                    player.yVelocity = 6;
                    jmpAmt++;
                    printf("jumped: %i", jmpAmt);
                }//event.key.keysym.scancode == SDL_SCANCODE_SPACE
                else if(event.key.keysym.scancode == SDL_SCANCODE_SPACE && jmpAmt == 1) {
                    player.yVelocity = 9;
                    jmpAmt = 0;
                    printf("jumping: %i", jmpAmt);
                }
            }
             */
        }
        
        if(keys[SDL_SCANCODE_RIGHT]) {
            player.xAccle = 1;
        } else if (keys[SDL_SCANCODE_LEFT]) {
            player.xAccle = -1;
        } else {
            player.xAccle = 0;
        }
        if (keys[SDL_SCANCODE_UP]){
            player.yAccle = 1;
        } else if (keys[SDL_SCANCODE_DOWN]) {
            player.yAccle = -1;
        } else {
            player.yAccle = 0;
        }
        
        
        float ticks = (float)SDL_GetTicks()/1000.0f;
        float elapsed = ticks - lastFrameTicks;
        lastFrameTicks = ticks;
        
        glClearColor(.1, .1, .1, .7);
        glClear(GL_COLOR_BUFFER_BIT);
        
        
        colRespone(player, sTest);
        colRespone(player, sTest2);
        colRespone(sTest2, sTest);
        
        float fixedElapsed = elapsed;
        if(fixedElapsed > FIXED_TIMESTEP * MAX_TIMESTEPS) {
            fixedElapsed = FIXED_TIMESTEP * MAX_TIMESTEPS;
        }
        while (fixedElapsed >= FIXED_TIMESTEP) {
            fixedElapsed -= FIXED_TIMESTEP;
            sTest.movement(&program, FIXED_TIMESTEP);
            sTest2.movement(&program, FIXED_TIMESTEP);
            player.movement(&program, FIXED_TIMESTEP);
        }
        sTest.movement(&program, fixedElapsed);
        sTest2.movement(&program, fixedElapsed);
        player.movement(&program, fixedElapsed);
        
        

        
        player.buildShape(&program);
        sTest.buildShape(&program);
        sTest2.buildShape(&program);
        SDL_GL_SwapWindow(displayWindow);
        
    }
    
    SDL_Quit();
    return 0;
}
