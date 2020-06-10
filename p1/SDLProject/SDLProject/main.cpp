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
#include "stb_image.h"

SDL_Window* displayWindow;
bool gameIsRunning = true;
ShaderProgram program;
glm::mat4 viewMatrix, professorMatrix, foxMatrix, projectionMatrix;
GLuint textureID1, textureID2;


float professor = 0;
float fox = 0;
float fox_rotate = 0;

GLuint LoadtTexture(const char* filePath){
    int w,h,n;
    unsigned char* image = stbi_load(filePath, &w, &h, &n, STBI_rgb_alpha);
    
    if (image == NULL){
        std::cout << "Unable to load image. Make sure the path is correct\n";
        assert(false);
    }
    
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    stbi_image_free(image);
    return textureID;
    
}
void Initialize(){
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("professor and fox", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
    #ifdef _WINDOWS
        glewInit();
    #endif
    //I don't know why, but with retina screen I have to double my glViewport size to show it right
    glViewport(0, 0, 1280, 960);
    
    
    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
    
    viewMatrix = glm::mat4(1.0f);
    professorMatrix = glm::mat4(1.0f);
    
    foxMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    
    glUseProgram(program.programID);
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    
    textureID1 = LoadtTexture("ctg.png");
    textureID2 = LoadtTexture("fox.png");
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
}

void ProcessInput(){
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
            gameIsRunning = false;
        }
    }
}

float lastTicks = 0.0f;

void Update(){
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;
    professor += 0.7f * deltaTime;
    fox += 0.5f * deltaTime;
    fox_rotate += 90.0f * deltaTime;
    
    professorMatrix = glm::mat4(1.0f);
    professorMatrix = glm::translate(professorMatrix, glm::vec3(professor,0.0f,0.0f));
    foxMatrix = glm::mat4(1.0f);
    foxMatrix = glm::translate(foxMatrix, glm::vec3(fox, 0.0f, 0.0f));
    foxMatrix = glm::rotate(foxMatrix, glm::radians(fox_rotate), glm::vec3(0.0f,0.0f,1.0f));
}

void Render(){
    glClear(GL_COLOR_BUFFER_BIT);
    
    
    float vertices1[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float vertices2[] = { -0.5, 2.5, 0.5, 2.5, 0.5, 3.5, -0.5, 2.5, 0.5, 3.5, -0.5, 3.5 };
    float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
    
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices1);
    glEnableVertexAttribArray(program.positionAttribute);
    glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program.texCoordAttribute);
    
    program.SetModelMatrix(professorMatrix);
    glBindTexture(GL_TEXTURE_2D, textureID1);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices2);
    program.SetModelMatrix(foxMatrix);
    glBindTexture(GL_TEXTURE_2D, textureID2);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(program.positionAttribute);
    glDisableVertexAttribArray(program.texCoordAttribute);
    
    SDL_GL_SwapWindow(displayWindow);
    
}

void Shutdown(){
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    
    Initialize();

    while (gameIsRunning) {
        
        ProcessInput();
        Update();
        Render();
        
    }
    
    Shutdown();
    return 0;
}
