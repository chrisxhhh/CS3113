/*
 Lunar Lander
 Chris Xu
 CS3113
 land the rocket to the target using left and right key, press space to restart anytime
 */
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

#include "Entity.h"

#define PLATFORM_COUNT 38
#define MISSION 7
#define WIN 7
#define LOSS 6

struct GameState {
    Entity* player;
    Entity* platforms;
    Entity* target;
    Entity* missonText;
    Entity* winText;
    Entity* loseText;
    //bool condition;
};

GameState state;

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

GLuint LoadTexture(const char* filePath) {
    int w, h, n;
    unsigned char* image = stbi_load(filePath, &w, &h, &n, STBI_rgb_alpha);
    
    if (image == NULL) {
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

void start(){
    // Initialize Player
    state.player = new Entity();
    state.player->position = glm::vec3(2.6f,3.25f,0);
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0,-0.30f,0);
    state.player->speed = 2.5f;
    state.player->textureID = LoadTexture("shuttle.png");
    state.player->entityType = PLAYER;
    
    
    
    
    //ignore the boarder around player, so when collied, the feet touchs the ground
    state.player->height = 0.9f;
    state.player->width = 0.8f;
    
    //Initialize platform
    state.platforms = new Entity[PLATFORM_COUNT];
    GLuint platformTextureID = LoadTexture("tileset.png");
    
    //boarder
    for (int i = 0 ; i < PLATFORM_COUNT; i++){
        state.platforms[i].animCols = 4;
        state.platforms[i].animRows = 1;
        state.platforms[i].animIndices = new int{3};
        state.platforms[i].textureID = platformTextureID;
        state.platforms[i].entityType = PLATFORM;
        if (i < 11){
            state.platforms[i].position = glm::vec3(i-5,-3.25f,0);
        }
        else if(i < 19){
            state.platforms[i].position = glm::vec3(-4.5,i-14,0);
        }
        else if (i < 27){
            state.platforms[i].position = glm::vec3(4.5,i-22,0);
        }
        else if (i < 33){
            state.platforms[i].position = glm::vec3(i-31,3.25f,0);
        }
        
    }
    
    state.platforms[33].position = glm::vec3(-4,1.0,0);
    state.platforms[34].position = glm::vec3(-3,1.0,0);
    state.platforms[35].position = glm::vec3(-2,1.0,0);
    state.platforms[36].position = glm::vec3(3.5,-0.25,0);
    state.platforms[37].position = glm::vec3(2.5,-0.25,0);
    
    
    for (int i = 0; i < PLATFORM_COUNT; i++){
        state.platforms[i].Update(0, NULL,NULL, 0,0);
    }
    
    //initialize target
    state.target = new Entity();
    state.target->textureID = LoadTexture("target.png");
    state.target->position = glm::vec3(1.5,-3.2,0);
    state.target->entityType = TARGET;
    
    state.target->Update(0, NULL,NULL, 0,0);
    
    //initialize win
    //MISSION
    state.missonText = new Entity[MISSION];
    GLuint wordTextureID = LoadTexture("font1.png");
    for (int i = 0; i < MISSION; i++){
        state.missonText[i].textureID = wordTextureID;
        state.missonText[i].animCols = 16;
        state.missonText[i].animRows = 16;
        state.missonText[i].entityType = WORD;
        state.missonText[i].isActive = false;
    }
    state.missonText[0].animIndices = new int{77};//M
    state.missonText[1].animIndices = new int{73};//I
    state.missonText[2].animIndices = new int{83};//S
    state.missonText[3].animIndices = new int{83};//S
    state.missonText[4].animIndices = new int{73};//I
    state.missonText[5].animIndices = new int{79};//O
    state.missonText[6].animIndices = new int{78};//N
    for (size_t i = 0; i < MISSION; i++){
        state.missonText[i].position = glm::vec3(i*0.5-4,0,0);
    }
    
    //SUCCEED
    state.winText = new Entity[WIN];
    for (size_t i = 0; i < WIN; i++){
        state.winText[i].textureID = wordTextureID;
        state.winText[i].animCols = 16;
        state.winText[i].animRows = 16;
        state.winText[i].entityType = WORD;
        state.winText[i].isActive = false;
    }
    state.winText[0].animIndices = new int{83};//S
    state.winText[1].animIndices = new int{85};//U
    state.winText[2].animIndices = new int{67};//C
    state.winText[3].animIndices = new int{67};//C
    state.winText[4].animIndices = new int{69};//E
    state.winText[5].animIndices = new int{69};//E
    state.winText[6].animIndices = new int{68};//D
    for (size_t i = 0; i < WIN; i++){
        state.winText[i].position = glm::vec3(i*0.5,0,0);
    }
    
    state.loseText = new Entity[LOSS];
    for (size_t i = 0; i < LOSS; i++){
        state.loseText[i].textureID = wordTextureID;
        state.loseText[i].animCols = 16;
        state.loseText[i].animRows = 16;
        state.loseText[i].entityType = WORD;
        state.loseText[i].isActive = false;
    }
    state.loseText[0].animIndices = new int{70};//F
    state.loseText[1].animIndices = new int{65};//A
    state.loseText[2].animIndices = new int{73};//I
    state.loseText[3].animIndices = new int{76};//L
    state.loseText[4].animIndices = new int{69};//E
    state.loseText[5].animIndices = new int{68};//D
    for (size_t i = 0; i < LOSS; i++){
        state.loseText[i].position = glm::vec3(i*0.5,0,0);
    }
}
void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Lunar Lander", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    //glViewport(0, 0, 1280, 960);
    
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
    start();
   
    // Initialize Game Objects
    
    
}


void ProcessInput() {
    
    state.player->movement = glm::vec3(0);
    
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                gameIsRunning = false;
                break;
                
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_LEFT:
                        // Move the player left
                        break;
                        
                    case SDLK_RIGHT:
                        // Move the player right
                        break;
                        
                    case SDLK_SPACE:
                        start();
                        break;
                }
                break; // SDL_KEYDOWN
        }
    }
    
    const Uint8 *keys = SDL_GetKeyboardState(NULL);

    if (keys[SDL_SCANCODE_LEFT]) {
        //state.player->movement.x = -1.0f;
        state.player->acceleration.x = -15.0f;
        std::cout << state.player->acceleration.x << std::endl;
    }
    else if (keys[SDL_SCANCODE_RIGHT]) {
        //state.player->movement.x = 1.0f;
        state.player->acceleration.x = 15.0f;
        std::cout << state.player->acceleration.x << std::endl;
    }
    if (glm::length(state.player->movement) > 1.0f) {
        state.player->movement = glm::normalize(state.player->movement);
    }

}

#define FIXED_TIMESTEP 0.0166666f
float lastTicks = 0;
float accumulator = 0.0f;

void win(){
    for (size_t t = 0; t < MISSION; t++){
        state.missonText[t].isActive = true;
    }
    for (size_t i = 0; i < WIN; i++){
        state.winText[i].isActive = true;
    }
    
    state.player->speed = 0;
    //state.player->acceleration = glm::vec3(0);
}
void lose(){
    for (size_t t = 0; t < MISSION; t++){
        state.missonText[t].isActive = true;
    }
    for (size_t i = 0; i < LOSS; i++){
        state.loseText[i].isActive = true;
    }
    state.player->speed = 0;
    state.player->velocity = glm::vec3(0);
}
void Update() {
    for(int i = 0; i< MISSION; i++){
        state.missonText[i].Update(0, NULL, NULL, 0, 0);
    }
    for(size_t t = 0; t< WIN; t++){
        state.winText[t].Update(0, NULL, NULL, 0, 0);
    }
    for(size_t t = 0; t< LOSS; t++){
        state.loseText[t].Update(0, NULL, NULL, 0, 0);
    }
    if (state.player->lastCollision == TARGET){
        std::cout << "win\n";
        win();
        
    }
    else if (state.player->lastCollision == PLATFORM) {
        std::cout << "lose\n";
        lose();
    }
    
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
        state.player->Update(FIXED_TIMESTEP, state.platforms, state.target, PLATFORM_COUNT,1);
        deltaTime -= FIXED_TIMESTEP;
    }
    accumulator = deltaTime;
}

void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    for(int i = 0; i < PLATFORM_COUNT; i++){
        state.platforms[i].Render(&program);
    }
    
    state.target->Render(&program);
    
    for(int i = 0; i < MISSION; i++){
        state.missonText[i].Render(&program);
    }
    for(int i = 0; i < WIN; i++){
        state.winText[i].Render(&program);
    }
    for(int i = 0; i < LOSS; i++){
        state.loseText[i].Render(&program);
    }
    //draw player last
    state.player->Render(&program);
    
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
