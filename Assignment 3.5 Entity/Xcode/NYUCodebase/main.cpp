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


#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

#define FIXED_TIMESTEP 0.01666667f
#define MAX_TIMESTEPS 6

SDL_Window* displayWindow;

const std::string levelFile = "NYUCodebase.app/Contents/Resources/myMap.txt";
const float heightRatio = 1.0*5;
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
    int jmpAmt;
    
    
    ShaderProgram program (RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
    
    state = 0;
    
    GLuint spriteTexture = LoadTexture("arne_sprites.png");
    GLuint charTexture = LoadTexture("characters_3_02.png");
    projectionMatrix.setOrthoProjection(-wideRatio, wideRatio, -heightRatio, heightRatio, -1, 1);
    

    float lastFrameTicks = 0.0f;
    
    program.setModelMatrix(modelMatrix);
    program.setViewMatrix(viewMatrix);
    program.setProjectionMatrix(projectionMatrix);
    
    SpriteSheet sheetTest(spriteTexture, 16, 8, 80);
    SpriteSheet sheetTest2(spriteTexture, 16, 8, 1);
    SpriteSheet sheetTest3(spriteTexture, 16, 8, 1);
    
    SpriteSheet playerSheet(charTexture, 8, 3, 8);
    
    playerSheet.fillIdle(3, 13, 14, 15);
    playerSheet.fillWalk(4, 8, 9, 10, 11);
    
    SpriteSheet legSheet(spriteTexture, 16, 8, 71);

    float h = 1;
    Entity enemy(h, h, 1, false, ENTITY_ENEMY, &sheetTest);
    Entity block(h, h, 1, true, ENTITY_WALL, &sheetTest2);
    Entity block2(h, h, 1, true, ENTITY_WALL, &sheetTest2);
    Entity player(h, h, 1, false,ENTITY_PLAYER, &playerSheet);
    Entity leg(h, h, .2, false, ENTITY_PLAYER, &legSheet);

    player.setPosition(0, 0, 0);
    leg.setPosition(0, 0, 0);
    
    block.setPosition(.4, -2.4, 1);
    block2.setPosition(0, -2.4, 1);
    
    std::vector<Entity*> collidable;
    //collidable.push_back(&enemy);
    collidable.push_back(&block);
    collidable.push_back(&block2);
    
    //CompositeEntity fullPlayer(player,leg);
    
    //fullPlayer.updatePosition(.2, 0, 0);
    Text test("Hello", spriteTexture, .2, -.04);
    
    SDL_Event event;
    bool done = false;
    while (!done) {
        const Uint8 *keys = SDL_GetKeyboardState(NULL);
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
                done = true;
            } if(event.type == SDL_KEYDOWN) {
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
        }
        
        if(keys[SDL_SCANCODE_RIGHT]) {
            player.xAccle = 5;
        } else if (keys[SDL_SCANCODE_LEFT]) {
            player.xAccle = -5;
        }  else {
            player.xAccle = 0;
        }
        
        if(enemy.collisionAmt == 1 && enemy.xVelocity == 0 && enemy.collidedBottom == true && enemy.collidedLeft == false && enemy.collidedRight == false ) {
            enemy.xVelocity =  10;
        }
        if(enemy.collidedRight == true) {
            enemy.xVelocity = -20;
        } else if(enemy.collidedLeft == true) {
            enemy.xVelocity = 20;
        }
        
        float ticks = (float)SDL_GetTicks()/1000.0f;
        float elapsed = ticks - lastFrameTicks;
        lastFrameTicks = ticks;
        

        //loop
        
        
        glClearColor(.9, .9, .9, .7);
        glClear(GL_COLOR_BUFFER_BIT);
        
        
        program.setViewMatrix(player.modelMatrix.inverse());

        modelMatrix.identity();
        program.setModelMatrix(modelMatrix);
        

        float fixedElapsed = elapsed;
        if(fixedElapsed > FIXED_TIMESTEP * MAX_TIMESTEPS) {
            fixedElapsed = FIXED_TIMESTEP * MAX_TIMESTEPS;
        }
        while (fixedElapsed >= FIXED_TIMESTEP) {
            fixedElapsed -= FIXED_TIMESTEP;
            //player.drawSprite(&program);
            for(int i = 0; i < collidable.size();i++) {
                player.movement(&program, collidable[i], FIXED_TIMESTEP);
                if(!collidable[i]->isStatic) {
                    collidable[i]->movement(&program, &player,FIXED_TIMESTEP);
                }
            }
            
            enemy.movement(&program, &player, FIXED_TIMESTEP);
            //leg.movement(&program, &tester, FIXED_TIMESTEP);
        }
        //enemy.movement(&program, &player, fixedElapsed);
        player.movement(&program, &enemy, fixedElapsed);

        
        //enemy.drawSprite(&program);
        
        player.drawSprite(&program);
        
        block.drawSprite(&program);
        block2.drawSprite(&program);
        
        modelMatrix.identity();
        test.DrawText(&program);
        
        SDL_GL_SwapWindow(displayWindow);
        
    }
    
    SDL_Quit();
    return 0;
}
