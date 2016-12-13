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

const std::string levelFile1 = "NYUCodebase.app/Contents/Resources/myMap.txt";
const std::string levelFile = "NYUCodebase.app/Contents/Resources/Stage1.txt";
const std::string levelFile2 = "NYUCodebase.app/Contents/Resources/Stage2.txt";
const std::string levelFile3 = "NYUCodebase.app/Contents/Resources/Stage3.txt";
const float heightRatio = 1.0*5;
const float wideRatio = heightRatio*(16.0/9.0);
const float aspectRatio = 16.0/9.0;
const float PI = 3.14159265;
float randXVelocity = 0.0f;
float randYVelocity = 0.0f;
float angle;
//const std::string levelFile = "myMap.txt";

SDL_Window* displayWindow;

Matrix modelMatrix;
Matrix viewMatrix;
Matrix projectionMatrix;

enum GameState {STATE_MAIN_MENU, STATE_GAME_LEVEL_1, STATE_GAME_LEVEL_2, STATE_GAME_LEVEL_3};
enum PauseState {UNPAUSED, QUIT_STATE};

int state;
int pauseState;
int pastState;

GLuint LoadTexture(const char *image_path) {
    SDL_Surface *surface = IMG_Load(image_path);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
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
    ReadTileMap rTM;
    ReadTileMap rTM2;
    ReadTileMap rTM3;
    int jmpAmt;
    
    rTM.readFile(levelFile);
    rTM2.readFile(levelFile2);
    rTM3.readFile(levelFile3);
    /*
    std::ifstream infile(levelFile);
    std::string line;
    /*
    if(std::getline(infile, line)) {
        printf("yes!");
        return 0;
    } else {
        printf("No!");
        return 0;
    }
    
    while (std::getline(infile, line)) {
        if(line == "[header]") {
            if(!rTM.readHeader(infile)) {
                return 0;
            }
        }else if(line == "[layer]") {
            rTM.readLayerData(infile);
        } else if(line == "[ObjectsLayer]") {
            rTM.readEntityData(infile);
        }
    }
    */
    
    ShaderProgram program (RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
    
    state = 0;
    pastState = state;
    pauseState = 0;
    
    GLuint spriteTexture = LoadTexture("arne_sprites.png");
    GLuint charTexture = LoadTexture("characters_3_02.png");
    GLuint fontTexture = LoadTexture("font1.png");
    projectionMatrix.setOrthoProjection(-wideRatio, wideRatio, -heightRatio, heightRatio, -1, 1);
    

    float lastFrameTicks = 0.0f;
    
    program.setModelMatrix(modelMatrix);
    program.setViewMatrix(viewMatrix);
    program.setProjectionMatrix(projectionMatrix);
    
    SpriteSheet sheetTest(spriteTexture, 16, 8, 80);
    SpriteSheet sheetTest2(spriteTexture, 16, 8, 81);
    SpriteSheet sheetTest3(spriteTexture, 16, 8, 1);
    
    SpriteSheet playerSheet(charTexture, 8, 3, 8);
    SpriteSheet playerSheet2(charTexture, 8, 3, 0);
    
    playerSheet.fillIdle(3, 13, 14, 15);
    playerSheet.fillWalk(4, 8, 9, 10, 11);
    
    SpriteSheet legSheet(spriteTexture, 16, 8, 71);

    float h = 1;
    Entity enemy(h, h, 1, false, ENTITY_ENEMY, &sheetTest);
    Entity enemy2(h, h, 1, true, ENTITY_ENEMY, &sheetTest2);
    
    Entity player(h, h, 1, false, ENTITY_PLAYER, &playerSheet);
    Entity player2(h, h, 1, false, ENTITY_PLAYER, &playerSheet2);
    Entity leg(h, h, .2, false, ENTITY_PLAYER, &legSheet);

    player.setPosition(0, 0, 0);
    player.pType = "Player1";
    player2.setPosition(0, 0, 0);
    player2.pType = "Player2";
    leg.setPosition(0, 0, 0);
    
    
    
    int xGrid = 0;
    int yGrid = 0;
    //CompositeEntity fullPlayer(player,leg);
    
    //fullPlayer.updatePosition(.2, 0, 0);
    Text test("Hello", fontTexture, 1, -.01);
    Text test2("Game Start", fontTexture, 1, -.01);
    Text stageCh("+", fontTexture, 1, -.01);
    //Text test4("-", fontTexture, 1, -.01);
    
    test.updatePosition(-2, 2, 0);
    //test2.updatePosition(-wideRatio/2, 1, 0);
    //stageCh.updatePosition(0, 0, 0);
    
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
                    player.yVelocity = 12;
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
        
        if(state == 0) {
            if(keys[SDL_SCANCODE_1]) {state = STATE_GAME_LEVEL_1;}
            if(keys[SDL_SCANCODE_2]) {state = STATE_GAME_LEVEL_2;}
            if(keys[SDL_SCANCODE_3]) {state = STATE_GAME_LEVEL_3;}
        }
        if(pauseState==QUIT_STATE) {
            if(keys[SDL_SCANCODE_Q]) {
                state = STATE_MAIN_MENU;
                pauseState = UNPAUSED;
            }if(keys[SDL_SCANCODE_ESCAPE]) {
                pauseState = UNPAUSED;
            }
        }
        else if(state > 0 && keys[SDL_SCANCODE_ESCAPE]) {
            pauseState = QUIT_STATE;
        }
        
        float ticks = (float)SDL_GetTicks()/1000.0f;
        float elapsed = ticks - lastFrameTicks;
        lastFrameTicks = ticks;
        

        //loop
        

        
        glClearColor(.9, .9, .9, .7);
        glClear(GL_COLOR_BUFFER_BIT);
      
        if(state==STATE_MAIN_MENU) {
            test.DrawText(&program, &modelMatrix);
            test2.DrawText(&program, &modelMatrix);
            stageCh.DrawText(&program, &modelMatrix);
        }
        
        if(state==STATE_GAME_LEVEL_1) {
            
            //check for change in states which symbolizes the very beginning of the stage entry
            if(pastState != state && pauseState != QUIT_STATE) {
                
                //rTM.readFile(levelFile);
                for(int i = 0; i < rTM.types.size(); i++) {
                    if(rTM.types[i] == "Player1") {
                        player.setPosition(rTM.xPosList[i], -rTM.yPosList[i], 0);
                        //printf("hey\n");
                    }
                    else if(rTM.types[i] == "Player2") {
                        player2.setPosition(rTM.xPosList[i], -rTM.yPosList[i], 0);
                    }
                }
            }
            
            //follow player with view matrix
            program.setViewMatrix(player.modelMatrix.inverse());

            //reset matrix and set it in prep for players and screen
            modelMatrix.identity();
            program.setModelMatrix(modelMatrix);
            
            //render stage 2 map by sending map texture and program data
            rTM.renderMap(&program, spriteTexture);
            
            //check if we are in paused state
            if(pauseState==QUIT_STATE) {
                //if paused show quit overlay text
                DrawText(&program, fontTexture, "Press Q again to quit", 1, .01);
            }
            //if not paused do movements and collisions
            else if(pauseState==UNPAUSED) {
                float fixedElapsed = elapsed;
                if(fixedElapsed > FIXED_TIMESTEP * MAX_TIMESTEPS) {
                    fixedElapsed = FIXED_TIMESTEP * MAX_TIMESTEPS;
                }
                while (fixedElapsed >= FIXED_TIMESTEP) {
                    fixedElapsed -= FIXED_TIMESTEP;
                    //player.drawSprite(&program);
                    player.movement(&program, &enemy, FIXED_TIMESTEP);
                    player2.movement(&program, &enemy, FIXED_TIMESTEP);
                    //enemy.movement(&program, &player, FIXED_TIMESTEP);
                    //leg.movement(&program, &tester, FIXED_TIMESTEP);
                }
                player.movement(&program, &enemy, fixedElapsed);
                player2.movement(&program, &enemy, fixedElapsed);
                
                player.tileCollision(rTM);
                player2.tileCollision(rTM);
            }
            
            //draw players
            player.drawSprite(&program);
            player2.drawSprite(&program);
        }
        
        if(state==STATE_GAME_LEVEL_2) {
            
            if(pastState != state && pauseState != QUIT_STATE) {
                
                for(int i = 0; i < rTM2.types.size(); i++) {
                    if(rTM2.types[i] == "Player1") {
                        player.setPosition(rTM2.xPosList[i], -rTM2.yPosList[i], 0);
                    }
                    else if(rTM2.types[i] == "Player2") {
                        player2.setPosition(rTM2.xPosList[i], -rTM2.yPosList[i], 0);
                    }
                }
            }
            
            //follow player with view matrix
            program.setViewMatrix(player.modelMatrix.inverse());
            
            //reset matrix and set it in prep for players and screen
            modelMatrix.identity();
            program.setModelMatrix(modelMatrix);
            
            //render stage 2 map by sending map texture and program data
            rTM2.renderMap(&program, spriteTexture);

            //check if we are in paused state
            if(pauseState==QUIT_STATE) {
                //if paused show quit overlay text
                DrawText(&program, fontTexture, "Press Q again to quit", 1, .01);
            }
            //if not paused do movements and collisions
            else if(pauseState==UNPAUSED) {
                float fixedElapsed = elapsed;
                if(fixedElapsed > FIXED_TIMESTEP * MAX_TIMESTEPS) {
                    fixedElapsed = FIXED_TIMESTEP * MAX_TIMESTEPS;
                }
                while (fixedElapsed >= FIXED_TIMESTEP) {
                    fixedElapsed -= FIXED_TIMESTEP;

                    player.movement(&program, &enemy, FIXED_TIMESTEP);
                    player2.movement(&program, &enemy, FIXED_TIMESTEP);
                }
                player.movement(&program, &enemy, fixedElapsed);
                player2.movement(&program, &enemy, fixedElapsed);
                
                player.tileCollision(rTM2);
                player2.tileCollision(rTM2);
            }
            
            
            //draw players
            player.drawSprite(&program);
            player2.drawSprite(&program);
        }
        
        if(state==STATE_GAME_LEVEL_3) {
            
            if(pastState != state && pauseState != QUIT_STATE) {
                
                
                for(int i = 0; i < rTM3.types.size(); i++) {
                    if(rTM3.types[i] == "Player1") {
                        player.setPosition(rTM3.xPosList[i], -rTM3.yPosList[i], 0);
                        
                    }
                    else if(rTM3.types[i] == "Player2") {
                        player2.setPosition(rTM3.xPosList[i], -rTM3.yPosList[i], 0);
                    }
                }
            }
            
            program.setViewMatrix(player.modelMatrix.inverse());
            
            modelMatrix.identity();
            program.setModelMatrix(modelMatrix);
            
            rTM3.renderMap(&program, spriteTexture);
            
            if(pauseState==QUIT_STATE) {
                //if paused show quit overlay text
                DrawText(&program, fontTexture, "Press Q again to quit", 1, .01);
            }
            //if not paused do movements and collisions
            else if(pauseState==UNPAUSED) {
                float fixedElapsed = elapsed;
                if(fixedElapsed > FIXED_TIMESTEP * MAX_TIMESTEPS) {
                    fixedElapsed = FIXED_TIMESTEP * MAX_TIMESTEPS;
                }
                while (fixedElapsed >= FIXED_TIMESTEP) {
                    fixedElapsed -= FIXED_TIMESTEP;
                    
                    player.movement(&program, &enemy, FIXED_TIMESTEP);
                    player2.movement(&program, &enemy, FIXED_TIMESTEP);
                    
                }
                player.movement(&program, &enemy, fixedElapsed);
                player2.movement(&program, &enemy, fixedElapsed);
                player.tileCollision(rTM3);
                player2.tileCollision(rTM3);
            }
            
            //draw sprites
            player.drawSprite(&program);
            player2.drawSprite(&program);
        }
        pastState = state;
        
        SDL_GL_SwapWindow(displayWindow);
        
    }
    
    SDL_Quit();
    return 0;
}
