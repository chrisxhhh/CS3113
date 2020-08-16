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

#include "Util.h"
#include "Entity.h"
#include "Map.h"
#include "Scene.h"
#include "Menu.h"
#include "Level1.h"
#include "Level2.h"
#include "Effects.h"

Scene* currentScene;
Scene* sceneList[3];
Effects* effects;

Mix_Music* music;
Mix_Chunk* shoot;

void SwitchToScene(Scene *scene) {
    currentScene = scene;
    
    currentScene->Initialize();
    effects->Start(FADEIN, 0.5f);
}

GameState state;
SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;


void Initialize() {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    displayWindow = SDL_CreateWindow("World of Tanks FLAT ", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    //glViewport(0, 0, 640, 480);
    
    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
    
    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    
    glUseProgram(program.programID);
    
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
   
    
    
//    //initialize map MOVED TO MAP.CPP
//    GLuint mapTextureID = Util::LoadTexture("tileset1.png");
//    state.map = new Map(LEVEL1_WIDTH, LEVEL1_HEIGHT, level1_data, mapTextureID, 1.0f, 4, 1);
    
    
    sceneList[0] = new Menu();
    sceneList[1] = new Level1();
    sceneList[2] = new Level2();
    effects = new Effects(projectionMatrix, viewMatrix);
    SwitchToScene(sceneList[0]);
    
    //effects
    
    effects->Start(FADEIN, 0.5f);
    
    //sound
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    music = Mix_LoadMUS("themeSound.wav");
    shoot = Mix_LoadWAV("shoot.wav");
    Mix_PlayMusic(music, -1);
    Mix_VolumeMusic(MIX_MAX_VOLUME / 4);
}

void ProcessInput() {
    
    currentScene->state.player->movement = glm::vec3(0);
    //currentScene->state.bullet[currentScene->state.player->bulletUsed].movement = glm::vec3(0);
    
    const Uint8 *keys = SDL_GetKeyboardState(NULL);

    if (keys[SDL_SCANCODE_A]) {
        currentScene->state.player->movement.x = -1.0f;
        currentScene->state.player->textureIndex = 2;
    }
    else if (keys[SDL_SCANCODE_D]) {
        currentScene->state.player->movement.x = 1.0f;
        currentScene->state.player->textureIndex = 3;
    }
    else if (keys[SDL_SCANCODE_W]) {
        currentScene->state.player->movement.y = 1.0f;
        currentScene->state.player->textureIndex = 0;
    }
    else if (keys[SDL_SCANCODE_S]) {
        currentScene->state.player->movement.y = -1.0f;
        currentScene->state.player->textureIndex = 1;
    }
    
    
    if (glm::length(currentScene->state.player->movement) > 1.0f) {
        currentScene->state.player->movement = glm::normalize(currentScene->state.player->movement);
    }
    
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                gameIsRunning = false;
                break;
                
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_SPACE:
                        if ( currentScene->state.player->bulletUsed <= 79 && currentScene->state.inMenu == false){
                            
                            if (currentScene->state.player->textureIndex == 0){
                                currentScene->state.bullet[currentScene->state.player->bulletUsed].movement.y = 1.0f;
                                currentScene->state.bullet[currentScene->state.player->bulletUsed].movement.x = 0;
                                currentScene->state.player->acceleration.y = -50;
                            }
                            if (currentScene->state.player->textureIndex == 1){
                                currentScene->state.bullet[currentScene->state.player->bulletUsed].movement.y = -1.0f;
                                currentScene->state.bullet[currentScene->state.player->bulletUsed].movement.x = 0;
                                currentScene->state.player->acceleration.y = 50;
                            }
                            if (currentScene->state.player->textureIndex == 2){
                                currentScene->state.bullet[currentScene->state.player->bulletUsed].movement.x = -1.0f;
                                currentScene->state.bullet[currentScene->state.player->bulletUsed].movement.y = 0;
                                currentScene->state.player->acceleration.x = 50;
                            }
                            if (currentScene->state.player->textureIndex == 3){
                                currentScene->state.bullet[currentScene->state.player->bulletUsed].movement.x = 1.0f;
                                currentScene->state.bullet[currentScene->state.player->bulletUsed].movement.y = 0;
                                currentScene->state.player->acceleration.x = -50;
                            }
                            currentScene->state.bullet[currentScene->state.player->bulletUsed].bulletMove = true;
                            currentScene->state.player->bulletUsed++;
                            Mix_PlayChannel(-1, shoot,0);
                        }
                        break;
                    case SDLK_RETURN:
                        if (currentScene->state.inMenu == true){
                            currentScene->state.nextScene = 1;
                            effects->Start(FADEIN, 0.5);
                        }
//                        else if (currentScene->state.player->isActive == false){
//
//                        }
                        break;
                }
                break; // SDL_KEYDOWN
        }
    }

}

#define FIXED_TIMESTEP 0.0166666f
float lastTicks = 0;
float accumulator = 0.0f;

void Update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;
    deltaTime += accumulator;
    if (deltaTime < FIXED_TIMESTEP) {
        accumulator = deltaTime;
        return;
    }
    while (deltaTime >= FIXED_TIMESTEP) {
    // Update. Notice it's FIXED_TIMESTEP. Not deltaTime
        currentScene->Update(FIXED_TIMESTEP);
        
        effects->Update(FIXED_TIMESTEP);
        deltaTime -= FIXED_TIMESTEP;
    }
    accumulator = deltaTime;
    
    viewMatrix = glm::mat4(1.0f);
    //stop scorlling in the end
    float view_x;
    float view_y;
    if (currentScene->state.player->position.x > 4.5 && currentScene->state.player->position.x < 14.5 ) {
        view_x = -currentScene->state.player->position.x;
    }
    else if (currentScene->state.player->position.x < 4.5){
        view_x = -4.5;
    }
    else if (currentScene->state.player->position.x > 14.5){
        view_x = -14.5;
    }
    if (currentScene->state.player->position.y < -3.25 && currentScene->state.player->position.y > -15.75){
        view_y = -currentScene->state.player->position.y;
    }
    else if (currentScene->state.player->position.y > -3.25   ) {
        view_y = 3.25;
    }
    else if (currentScene->state.player->position.y < -15.75){
        view_y = 15.75;
    }
    viewMatrix = glm::translate(viewMatrix, glm::vec3(view_x, view_y, 0));
        
    //must after viewmatrix
    //viewMatrix = glm::translate(viewMatrix, effects->viewOffset);
    //std::cout << currentScene->state.player->position.x << "  " << currentScene->state.player->position.y << std::endl;
    //std::cout << currentScene->state.player->bulletUsed << std::endl;
}

void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    program.SetViewMatrix(viewMatrix);
    
    glUseProgram(program.programID);
    currentScene->Render(&program);
    
    
    effects->Render();
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
        
        if (currentScene->state.nextScene >=0){
            SwitchToScene(sceneList[currentScene->state.nextScene]);
        }
        
        Render();
    }
    
    Shutdown();
    return 0;
}
