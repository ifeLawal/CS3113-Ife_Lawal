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

const float heightRatio = 1.0;
const float wideRatio = heightRatio*(16.0/9.0);
const float aspectRatio = 16.0/9.0;
const float PI = 3.14159265;
float randXVelocity = 0.0f;
float randYVelocity = 0.0f;
float angle;

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
    ShaderProgram program (RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
    
    state = 0;
    
    GLuint fontTexture = LoadTexture("arne_sprites.png");
    projectionMatrix.setOrthoProjection(-wideRatio, wideRatio, -heightRatio, heightRatio, -1.0, 1.0);
    

    float lastFrameTicks = 0.0f;
    
    program.setModelMatrix(modelMatrix);
    program.setViewMatrix(viewMatrix);
    program.setProjectionMatrix(projectionMatrix);
    
    SpriteSheet sheetTest(fontTexture, 16, 8, 6);
    SpriteSheet sheetTest2(fontTexture, 16, 8, 0);
    SpriteSheet sheetTest3(fontTexture, 16, 8, 1);
    SpriteSheet playerSheet(fontTexture, 16, 8, 98);
    SpriteSheet legSheet(fontTexture, 16, 8, 71);
    //SpriteSheet(<#GLuint spriteSheet#>, int spriteCountX, <#int spriteCountY#>)
    float h = 1;
    Entity tester(h, h, .2, true, ENTITY_WALL, &sheetTest);
    Entity tester2(h, h, .2, true, ENTITY_WALL, &sheetTest2);
    Entity tester3(h, h, .2, true, ENTITY_WALL, &sheetTest3);
    Entity player(h, h, .2, false,ENTITY_PLAYER, &playerSheet);
    Entity leg(h, h, .2, false, ENTITY_PLAYER, &legSheet);
    //Entity(<#float wVal#>, <#float hVal#>, <#SpriteSheet sheet#>)
    tester.setPosition(0, -.4, 0);
    tester2.setPosition(0.4, -.4, 0);
    tester3.setPosition(0.6, -.4, 0);
    player.setPosition(0, 0, 0);
    leg.setPosition(0, 0, 0);
    
    
    CompositeEntity fullPlayer(player,leg);
    
    //fullPlayer.updatePosition(.2, 0, 0);
    Text test("Hello", fontTexture, .2, -.04);
    
    SDL_Event event;
    bool done = false;
    while (!done) {
        const Uint8 *keys = SDL_GetKeyboardState(NULL);
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
                done = true;
            }
            if(keys[SDL_SCANCODE_RIGHT]) {
                fullPlayer.xVelocity = 1;
            } else if (keys[SDL_SCANCODE_LEFT]) {
                fullPlayer.xVelocity = -1;
            }
        }
        
        float ticks = (float)SDL_GetTicks()/1000.0f;
        float elapsed = ticks - lastFrameTicks;
        lastFrameTicks = ticks;
        

        //loop
        
        
        
        glClear(GL_COLOR_BUFFER_BIT);
        
        /*
        modelMatrix.identity();
        modelMatrix.Translate(-.5f, .5f, 0);
        program.setModelMatrix(modelMatrix);
        */

        //fullPlayer.drawSprite(&program);

        //leg.drawSprite(&program);
        //fullPlayer.movement(&program, &tester, elapsed);
        //DrawText(&program, fontTexture, "Hello", .2, -.04);
        float fixedElapsed = elapsed;
        if(fixedElapsed > FIXED_TIMESTEP * MAX_TIMESTEPS) {
            fixedElapsed = FIXED_TIMESTEP * MAX_TIMESTEPS;
        }
        while (fixedElapsed >= FIXED_TIMESTEP) {
            fixedElapsed -= FIXED_TIMESTEP;
            //player.drawSprite(&program);
            player.movement(&program, &tester, FIXED_TIMESTEP);
            //leg.movement(&program, &tester, FIXED_TIMESTEP);
        }
        player.movement(&program, &tester, fixedElapsed);
        //leg.movement(&program, &tester, fixedElapsed);
        
        //fullPlayer.isColliding(&tester);
        
        tester.drawSprite(&program);
        tester2.drawSprite(&program);
        tester3.drawSprite(&program);
        player.drawSprite(&program);
        leg.drawSprite(&program);
        
        program.setViewMatrix(player.modelMatrix.inverse());
        
        modelMatrix.identity();
        test.DrawText(&program);
        
        SDL_GL_SwapWindow(displayWindow);
    }
    
    SDL_Quit();
    return 0;
}
