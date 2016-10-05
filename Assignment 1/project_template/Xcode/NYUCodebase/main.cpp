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

/*
class MatrixSetup {


public:
    ShaderProgram program;
    Matrix modelMatrix;
    Matrix viewMatrix;
    Matrix projectionMatrix;
    
};
*/
const float heightRatio = 2.0;
const float wideRatio = heightRatio*(16/9);
const float PI = 3.14159265;
float angle;

/*
typedef struct SDL_Surface
{
    int w;
    int h;
    void *pixels;
} SDL_Surface;
*/

class tester {
    
public:
    Matrix modelMatrix;
    Matrix projectionMatrix;
    Matrix viewMatrix;
    float x, y, z, rot;
    float uX, uY, uZ, uRot;
    float* vertices;
    
    tester() {
        projectionMatrix.setOrthoProjection(-wideRatio, wideRatio, -heightRatio, heightRatio, -1.0, 1.0);
    }
    
    void setMatrices(ShaderProgram program) {
        program.setModelMatrix(modelMatrix);
        program.setProjectionMatrix(projectionMatrix);
        program.setViewMatrix(viewMatrix);
    }
    
    void setVertices(float* verts) {
        vertices = verts;
    }
    void updateX(float xVal){
        x = xVal;
        uX = xVal;
    }
    void updateY(float yVal){
        y = yVal;
        uY = yVal;
    }
    void updateZ(float zVal){
        z = zVal;
        uZ = zVal;
    }
    void updateRotate(float rotVal){
        rot = rotVal;
    }
    void buildTriangle(ShaderProgram program){
        
        if(vertices == NULL) {
            printf("Error. Stop immediately!");
        }
        else{
            
            modelMatrix.Translate(x, y, z);
            modelMatrix.Rotate(rot);
            glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
            glEnableVertexAttribArray(program.positionAttribute);
        
            glDrawArrays(GL_TRIANGLES, 0, 3);
        
            glDisableVertexAttribArray(program.positionAttribute);
            
            
        }
    }
    
    void buildImage(ShaderProgram program, GLuint imageGiven, float* verts, float* texPos) {
        uY += -.005;
        program.setModelMatrix(modelMatrix);
        program.setProjectionMatrix(projectionMatrix);
        program.setViewMatrix(viewMatrix);
        if(uY < -.75 ){
            modelMatrix.identity();
            float randomNumber = (2.0 * float(rand()) / float(RAND_MAX)) - 1.0;
            modelMatrix.Translate(x+randomNumber, y, 0);
            uY = y;
        }
        else {
         modelMatrix.Translate(x, y, z);
        }
        modelMatrix.Rotate(rot);
        
        glBindTexture(GL_TEXTURE_2D, imageGiven);
        
        //float vertices[] = {1.5f, -1.5f, 1.0f, 1.5f, -.7f, .7f};
        
        
        glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, verts);
        glEnableVertexAttribArray(program.positionAttribute);
        
        
        glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texPos);
        glEnableVertexAttribArray(program.texCoordAttribute);
        
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        glDisableVertexAttribArray(program.positionAttribute);
        
        glDisableVertexAttribArray(program.texCoordAttribute);
       
        
    }
    
};

void buildImage(ShaderProgram program, GLuint imageGiven, float* verts, float* texPos) {
    
    
    glBindTexture(GL_TEXTURE_2D, imageGiven);
    
    //float vertices[] = {1.5f, -1.5f, 1.0f, 1.5f, -.7f, .7f};
    
    
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, verts);
    glEnableVertexAttribArray(program.positionAttribute);
    
    
    glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texPos);
    glEnableVertexAttribArray(program.texCoordAttribute);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(program.positionAttribute);
    
    glDisableVertexAttribArray(program.texCoordAttribute);
    
}

GLuint LoadTexture(const char *image_path) {
    SDL_Surface *surface = IMG_Load(image_path);
    
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_BGRA, GL_UNSIGNED_BYTE, surface->pixels);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    SDL_FreeSurface(surface);
    
    return textureID;
}



int main(int argc, char *argv[])
{
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 360, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
#ifdef _WINDOWS
    glewInit();
#endif
    
    //setup
    glViewport(0, 0, 640, 360);
    
    ShaderProgram program(RESOURCE_FOLDER"vertex_textured.glsl",RESOURCE_FOLDER"fragment_textured.glsl");
    
    GLuint emojiTexture = LoadTexture("MyPixelRain.png");
    GLuint rain2Texture = LoadTexture("MyPixelRain2.png");
    
    Matrix modelMatrix;
    Matrix projectionMatrix;
    Matrix viewMatrix;
    
    projectionMatrix.setOrthoProjection(-wideRatio, wideRatio, -heightRatio, heightRatio, -1.0, 1.0);
    
    glUseProgram(program.programID);
    
    float vertices[] = {-1.5, 1, -0.5, 1, -0.5, 3.0, -1.5, 1, -0.5, 3, -1.5, 3};
    float vertices2[] = {-0.5, 2, 0.5, 2, 0.5, 4, -0.5, 2, 0.5, 4, -0.5, 4};
    float vertices3[] = {0.5, 2.2, 1.5, 2.2, 1.5, 4.2, 0.5, 2.2, 1.5, 4.2, 0.5, 4.2};
    float texCoords[] = {0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0};
    float vertices5[] = {0.5, -0.5, 0.0, 0.5, -0.5, -0.5};
    
    tester tester1 = tester();
    tester tester2 = tester();
    tester tester3 = tester();
    tester1.updateY(-.03);
    tester2.updateY(-.04);
    tester3.updateY(-.05);
    
    SDL_Event event;
    bool done = false;
    while (!done) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
                done = true;
            }
        }
        
        //glClearColor(0, 0, 0, 1.0);
        
        glClear(GL_COLOR_BUFFER_BIT);
        
        //loop
        
        //tester1.setMatrices(program);
        //tester2.setMatrices(program);

        //tester1.setMatrices(program);
        
        glEnable(GL_BLEND);
        
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        float lastFrameTicks = 0.0f;
        
        float ticks = (float)SDL_GetTicks()/1000.0f;
        float elapsed = ticks - lastFrameTicks;
        lastFrameTicks = ticks;
        angle += elapsed;
        //tester1.buildTriangle(program);
        tester1.buildImage(program, emojiTexture, vertices, texCoords);
        tester2.buildImage(program, emojiTexture, vertices2, texCoords);
        tester3.buildImage(program, rain2Texture, vertices3, texCoords);
        modelMatrix.Rotate(angle);
        
        
        
        program.setModelMatrix(modelMatrix);
        program.setProjectionMatrix(projectionMatrix);
        program.setViewMatrix(viewMatrix);
        /*
        glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices5);
        glEnableVertexAttribArray(program.positionAttribute);
        
        glDrawArrays(GL_TRIANGLES, 0, 3);
        
        glDisableVertexAttribArray(program.positionAttribute);
        */
        buildImage(program, emojiTexture, vertices, texCoords);
        /*modelMatrix.Translate(.5, 0, 0);
        //buildImage(program, emojiTexture, vertices2, texCoords);
        */
        //
        modelMatrix.Rotate(PI/10);
        
        
        
        
        SDL_GL_SwapWindow(displayWindow);
    }
    
    SDL_Quit();
    return 0;
}
