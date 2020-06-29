//to add a texture remember to add to Build Phase -> copy file
#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h" //needed for adding texture

SDL_Window* displayWindow;
bool gameIsRunning = true;
ShaderProgram program; //the target object for rendering
GLuint GeorgeID; //id that holds the texture
glm::mat4 viewMatrix, projectionMatrix, modelMatrix;
float lastTicks = 0.0f;

//animation nums
int* animIndices = new int[4] {3,7,11,15}; //the texture number in alas
int animFrame = 4; //there is 4 frame
int animIndex = 0; //starting from index 0
float animTime = 0.0f;

GLuint LoadTexture(const char* filePath){
    int w, h, n;
    unsigned char* image = stbi_load(filePath, &w, &h, &n, STBI_rgb_alpha);
    
    if (image == NULL){
        std::cout << "Unable to load image. Make sure the path is correct\n";
        assert(false);
    }
    
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); //when smaller, use nearest color
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); //when expand, use nearest color
    
    stbi_image_free(image);
    return textureID;
}

void DrawSpriteFromTextureAtlas(ShaderProgram *program, GLuint textureID, int index){
    //========================change needed======================================
    int cols = 4;
    int rows = 4;
    //========================change needed======================================
    float u = (float)(index % cols) / (float)cols;
    float v = (float)(index / cols) / (float)rows;
    float width = 1.0f / (float)cols; float height = 1.0f / (float)rows;
    float texCoords[] = { u, v + height, u + width, v + height, u + width, v,
        u, v + height, u + width, v, u, v};
    float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    glBindTexture(GL_TEXTURE_2D, textureID);
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}

void initialize(){
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("George", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
    #ifdef _WINDOWS
        glewInit();
    #endif
    //program.Load("shaders/vertex.glsl", "shaders/fragment.glsl");//load shader
    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");//load shader with texture
    GeorgeID = LoadTexture("george_0.png");
    modelMatrix = glm::mat4(1.0f);
    viewMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    
    //program.SetColor(1.0f, 1.0f, 0.0f, 1.0f);
    
    glUseProgram(program.programID);
    
    //enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    //background color
    glClearColor(0.4f, 0.4f, 0.4f, 1.0f);
}

void processInput(){
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
            gameIsRunning = false;
        }
    }
}


void render() {
    glClear(GL_COLOR_BUFFER_BIT);
    program.SetModelMatrix(modelMatrix);
    //draw the animation
    DrawSpriteFromTextureAtlas(&program, GeorgeID, animIndices[animIndex]);
    /*
    float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
    
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program.positionAttribute);
    
    glVertexAttribPointer(program.texCoordAttribute, 2,GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program.texCoordAttribute);
    
    glBindTexture(GL_TEXTURE_2D, GeorgeID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(program.positionAttribute);
    glDisableVertexAttribArray(program.texCoordAttribute);
     */
    SDL_GL_SwapWindow(displayWindow); }

void update(){
    //deltatime
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;
    
    animTime += deltaTime;
    if (animTime >= 0.25f) {
    animTime = 0.0f; animIndex++;
        if (animIndex >= animFrame) {
            animIndex = 0;
        }
    }
    
}
    

int main(int argc, char* argv[]) {
    initialize();
    while (gameIsRunning) {
        processInput();
        update();
        render();
    }
    SDL_Quit();
    return 0;
}
