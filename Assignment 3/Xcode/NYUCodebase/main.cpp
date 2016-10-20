#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "ShaderProgram.h"
#include "Matrix.h"
#include <vector>
#include "Text.h"
#include "Entity.h"

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

Matrix modelMatrix;
Matrix viewMatrix;
Matrix projectionMatrix;

enum GameState {STATE_MAIN_MENU, STATE_GAME_LEVEL };

int state;

GLuint LoadTexture(const char *image_path) {
    SDL_Surface *surface = IMG_Load(image_path);
    
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    
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

int main(int argc, char *argv[])
{
    setup();
    ShaderProgram program (RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
    

    state = 0;
    
    projectionMatrix.setOrthoProjection(-wideRatio, wideRatio, -heightRatio, heightRatio, -1.0, 1.0);
    
    float lastFrameTicks = 0.0f;
    float xSpeed = 0;
    float xVelocity = 0;
    float spriteCountX = 2;
    float spriteCountY = 5;
    int maxBullets = 5;

    GLuint spriteSheet1 = LoadTexture("characters_3.png");
    GLuint fontSheet = LoadTexture("font1.png");
    GLuint spriteSheet3 = LoadTexture("SI-Sheet.png");
    
    float wHRatio = .4;
    float pieceSize = .25;
    float screenHeight = 1;
    float screenWidth = screenHeight * wideRatio;
    int alienAmt = 11;
    float levels = 5;
    //float pieceHeight = .75;
    //float pieceHeight = .75;
    float pieceWidth = pieceSize*wHRatio;
    float bulletSpeed = 0;
    
    Text tester("Game Start", fontSheet, .15, 0.01);
    
    std::vector<Entity> aliens;
    Entity player(pieceWidth, pieceSize/1.7, spriteSheet3, 0);
    player.setUpSpriteSheet(6, spriteCountX, spriteCountY);
    player.setTrans(0, -screenHeight+player.height/2, 1);
    Entity bullet(pieceWidth/12, pieceSize/3, spriteSheet3, 0);
    bullet.setUpSpriteSheet(7, spriteCountX, spriteCountY);
    bullet.setTrans(player.xTrans, player.yTrans+.3, player.zTrans);
    bullet.active = false;
    
    std::vector<Entity> bullets;
    for(int i = 0; i < maxBullets; i++) {
        Entity eBullet(pieceWidth/12, pieceSize/3, spriteSheet3, 0);
        eBullet.setUpSpriteSheet(7, spriteCountX, spriteCountY);
        eBullet.setTrans(player.xTrans, player.yTrans+.3, player.zTrans);
        eBullet.active = false;
        bullets.push_back(eBullet);
    }
    
    for(int i = 0; i < levels; i++) {
        float yPos = screenHeight - i*(screenHeight/levels) - pieceSize;
        int index = (i/2)*2;
        for(int j = 0; j < alienAmt; j++) {
            Entity alien(pieceWidth+.05, pieceSize, spriteSheet3, i);
            float xPos =  -screenWidth + j*(screenWidth/alienAmt+.05) + alien.width;
            alien.setTrans(xPos, yPos, 1);
            alien.setUpSpriteSheet(index, spriteCountX, spriteCountY);
            alien.active = true;
            aliens.push_back(alien);
        }
    }
    
    program.setModelMatrix(modelMatrix);
    program.setProjectionMatrix(projectionMatrix);
    program.setViewMatrix(viewMatrix);
    
    float amountElpsed = 0;
    float moveFactor = .1;
    float moveDown = .1;
    float moveFactorlv1 = .1;
    float moveFactorlv2 = .1;
    float moveFactorlv3 = .1;
    float moveFactorlv4 = .1;
    float moveFactorlv5 = .1;
    float counter = 0;
    float counterDown = 0;
    int group = 1;
    bool reverse = false;
    bool reset = false;
    //tester.updateX(-1);
    //tester.updateY(-1);
    
    SDL_Event event;
    bool done = false;
    while (!done) {
        const Uint8 *keys = SDL_GetKeyboardState(NULL);
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
                done = true;
            } if(state == STATE_MAIN_MENU && event.type == SDL_KEYDOWN) {
                if(event.key.keysym.scancode == SDL_SCANCODE_SPACE) {
                    state = 1;
                }
            }else if(state == STATE_GAME_LEVEL) {
               
                if(keys[SDL_SCANCODE_RIGHT]) {
                    xSpeed = 1;
                } else if (keys[SDL_SCANCODE_LEFT]) {
                    xSpeed = -1;
                } else {
                    xSpeed = 0;
                }
                if(event.type == SDL_KEYDOWN) {
                    if(event.key.keysym.scancode == SDL_SCANCODE_SPACE && !bullet.active) {
                        bullet.setTrans(player.xTrans, player.yTrans, player.zTrans);
                        bullet.active = true;
                    }
                }
            }
        }
        
        //loop
        
        glClear(GL_COLOR_BUFFER_BIT);
        
        //glUseProgram(program.programID);
        
        float ticks = (float)SDL_GetTicks()/1000.0;
        float elapsed = ticks - lastFrameTicks;
        lastFrameTicks = ticks;
        
        if(state==STATE_MAIN_MENU) {
            
            program.setModelMatrix(modelMatrix);
            modelMatrix.identity();
            //tester.drawSprite(program);
            //modelMatrix.identity();
            modelMatrix.Translate(-.5, -0.3, 1);
            
            DrawText(&program, fontSheet, "Game Start", .15, -.025);
            
            program.setModelMatrix(modelMatrix);
            modelMatrix.identity();
            modelMatrix.Translate(-.55, 0, 1);
            DrawText(&program, fontSheet, "Tap Spacebar to Begin", .05, -.0025);
            //tester.DrawText(&program);

        }
        else if(state == STATE_GAME_LEVEL) {

            
            if(amountElpsed/elapsed > 3) {
                amountElpsed = 0;
            }
            float group = amountElpsed/elapsed;

            if(bullet.active) {
                bullet.updatePosition(0, 1*elapsed, 0);
                //bullet.drawSprite(&program);
            } else {
                bullet.setTrans(player.xTrans, player.yTrans, player.zTrans);
            }
            if(bullet.yTrans > screenHeight) {
                bullet.active = false;
            }
            for(int j = 0; j < aliens.size();j++) {
                if(reset){
                    if(aliens[j].xTrans-aliens[j].width < -screenWidth) {
                        reverse = true;
                        reset = false;
                    }
                }
                else if(!reset) {
                    if(aliens[j].xTrans + aliens[j].width/2 > screenWidth) {
                        reverse = true;
                        reset = true;
                    }
                    
                }
                
                if(aliens[j].level*20 == counter && counter < 100) {
                    aliens[j].updatePosition(moveFactor, 0, 0);
                    //printf("Not active\n");
                }
                if(counter > 100 && !bullets[1].active) {
                    if(j > 46) {
                        bullets[1].active= true;
                        bullets[1].setTrans(aliens[j].xTrans, aliens[j].yTrans, 1);
                    }
                }
                
                if(counter == 100) {
                    if(reverse) {
                        aliens[j].updatePosition(0, -moveDown, 0);
                    }
                    printf("Not active\n");
                }
                if((aliens[j].isColliding(bullet))) {
                    aliens[j].active = false;
                    printf("%i\n", j);
                    printf("working");
                    bullet.active = false;
                    bullet.updatePosition(player.xTrans, player.yTrans, player.zTrans);
                }
                if(aliens[j].active) {
                    aliens[j].render(&program);
                    //printf"%f\n", j)
                }else {
                    aliens[j].updatePosition(3, 3, 0);
                    aliens[j].render(&program);
                    printf("%i\n", j);
                }
            }
            
            if(bullets[1].active) {
                bullets[1].updatePosition(0, -1*elapsed, 0);
                if(player.isColliding(bullets[1])){
                    player.setTrans(0, player.yTrans, 1);
                    bullets[1].active = false;
                    printf("collision");
                }else if(bullets[1].yTrans -bullets[1].height/2 < screenHeight){
                    bullets[1].active = false;
                }
                printf("shooting");
            }

            amountElpsed += elapsed;
            xVelocity = xSpeed * elapsed;
            if(xVelocity + player.xTrans + player.width < screenWidth && xVelocity + player.xTrans -player.width > -screenWidth) {
                player.updatePosition(xVelocity, 0, 0);
            }
            bullets[1].render(&program);
            bullet.render(&program);
            player.render(&program);
            counter++;
            if(counter > 100) {
                
                if(reverse) {
                    counterDown++;
                    if(counterDown > 20){
                        moveFactor = -moveFactor;
                        reverse = false;
                    }
                } else{
                    counter = 0;
                    counterDown = 0;
                }
            }
        }
        
        SDL_GL_SwapWindow(displayWindow);
    }
    
    SDL_Quit();
    return 0;
}
