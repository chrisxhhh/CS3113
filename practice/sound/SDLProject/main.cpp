#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1

#include <SDL_mixer.h> //sound
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, paddle1, paddle2, ball, projectionMatrix;

glm::vec3 p1_position = glm::vec3(-4.85, 0, 0);
glm::vec3 p1_movement = glm::vec3(0, 0, 0);

glm::vec3 p2_position = glm::vec3(4.85, 0, 0);
glm::vec3 p2_movement = glm::vec3(0, 0, 0);

glm::vec3 ball_position = glm::vec3(0, 0, 0);
glm::vec3 ball_movement = glm::vec3(0, 0, 0);
float speed = 1.0f;
float ballSpeed = 1.0f;
bool gameOver = false;
bool collision = false;

Mix_Music* music;
Mix_Chunk* bounce;

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    displayWindow = SDL_CreateWindow("Pong!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    //glViewport(0, 0, 1280, 960);
    
    program.Load("shaders/vertex.glsl", "shaders/fragment.glsl");
    
    viewMatrix = glm::mat4(1.0f);
    paddle1 = glm::mat4(1.0f);
    paddle2 = glm::mat4(1.0f);
    ball = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    program.SetColor(0.0f, 0.0f, 0.0f, 1.0f);
    
    glUseProgram(program.programID);
    
    glClearColor(0.4f, 0.4f, 0.4f, 1.0f);
    
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    music = Mix_LoadMUS("dooblydoo.mp3");
    bounce = Mix_LoadWAV("bounce.wav");
    Mix_PlayMusic(music, -1);
    Mix_VolumeMusic(MIX_MAX_VOLUME / 4);
}

void resetLayout(){
    p1_position = glm::vec3(-4.85, 0, 0);
    p2_position = glm::vec3(4.85, 0, 0);
    ball_position = glm::vec3(0, 0, 0);
    ball_movement = glm::vec3(0, 0, 0);
    ballSpeed = 1.0f;
    collision = false;
}

void ProcessInput() {
    SDL_Event event;
    p1_movement = glm::vec3(0, 0, 0);
    p2_movement = glm::vec3(0, 0, 0);
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                gameIsRunning = false;
                break;

            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_SPACE:
                        //reset
                        if (!gameOver){
                            ball_movement = glm::vec3(-1, 1, 0);
                            gameOver = true;
                        }
                        else{
                            resetLayout();
                            gameOver = false;
                        }
                        break;
                    case SDLK_i:
                        //speed up
                        ballSpeed += 0.5;
                        break;
                    case SDLK_k:
                        //speed down
                        if (ballSpeed > 0.5)  ballSpeed -= 0.5;
                        break;
                }
            break; // SDL_KEYDOWN
        }
    }
    const Uint8 *keys = SDL_GetKeyboardState(NULL);
    if (keys[SDL_SCANCODE_W]) {
        p1_movement.y = 2.0f;
    }
    else if (keys[SDL_SCANCODE_S]) {
        p1_movement.y = -2.0f;
    }
    if (keys[SDL_SCANCODE_UP]) {
        p2_movement.y = 2.0f;
    }
    else if (keys[SDL_SCANCODE_DOWN]) {
        p2_movement.y = -2.0f;
    }
}



void ballMove(){
    if (ball_position.y >= 3.45 || ball_position.y <= -3.45){
        ball_movement.y = -ball_movement.y;
    }
    //left
    float left_x = fabs(ball_position.x - p1_position.x ) - 0.45;
    float left_y = fabs(ball_position.y - p1_position.y) - 0.6;
    if (left_x < 0 && left_y < 0){
        ball_movement.x = -ball_movement.x;
        collision = true;
    }
    float right_x = fabs(ball_position.x - p2_position.x ) - 0.45;
    float right_y = fabs(ball_position.y - p2_position.y) - 0.6;
    if (right_x < 0 && right_y < 0){
        ball_movement.x = -ball_movement.x;
        collision = true;
    }
    if(ball_position.x <= -4.75 ||ball_position.x >= 4.75){
        ball_movement = glm::vec3(0,0,0);
    }
}

float lastTicks = 0.0;
void Update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;

    p1_position += p1_movement * speed * deltaTime;
    paddle1 = glm::mat4(1.0f);
    paddle1 = glm::translate(paddle1, p1_position);
    
    p2_position += p2_movement * speed * deltaTime;
    paddle2 = glm::mat4(1.0f);
    paddle2 = glm::translate(paddle2, p2_position);
    
    collision = false;
    ballMove();
    ball_position += ball_movement * ballSpeed * deltaTime;
    ball = glm::mat4(1.0f);
    ball = glm::translate(ball, ball_position);
    
    if (collision){
        Mix_PlayChannel(-1, bounce,0);
    }
    
}

void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    program.SetModelMatrix(paddle1);
    

    float vertices1[] = {-0.15, -0.7, 0.15, -0.7, 0.15, 0.7,
        -0.15, -0.7, 0.15, 0.7 , -0.15, 0.7};
    float vertices3[] = {-0.3, -0.3, 0.3, -0.3, 0.3, 0.3,
                        -0.3,-0.3, 0.3,0.3, -0.3,0.3};
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices1);
    glEnableVertexAttribArray(program.positionAttribute);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    program.SetModelMatrix(paddle2);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    program.SetModelMatrix(ball);
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices3);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(program.positionAttribute);
    
    SDL_GL_SwapWindow(displayWindow);
}

void Shutdown() {
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
