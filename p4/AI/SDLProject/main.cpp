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

#define PLATFORM_COUNT 16
#define ENEMY_COUNT 3



struct GameState {
    Entity* player;
    Entity* platforms;
    Entity* enemy;
    Entity* win;
    Entity* lose;
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


void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("One Man Three Guards", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 1280, 960);
    
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
    
   
    // Initialize Game Objects
    
    // Initialize Player
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(-4, -2, 0);
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0,-9.81f,0);
    state.player->speed = 2.0f;
    state.player->textureID = LoadTexture("knight_blue_idle_02.png");
    
    
    
    
    //ignore the boarder around player, so when collied, the feet touchs the ground
    state.player->topLen = 0.1;
    state.player->botLen = 0.35;
    state.player->leftLen = 0.4;
    state.player->rightLen = 0.2;
    
    state.player->jumpPower = 7.0f;
    
    //Initialize platform
    state.platforms = new Entity[PLATFORM_COUNT];
    state.platforms->entityType = PLATFORM;
    GLuint platformTextureID = LoadTexture("platformer.png");
    
    
    for (int i = 0; i < PLATFORM_COUNT; i++){
        state.platforms[i].textureID = platformTextureID;
        
        state.platforms[i].animIndices = new int[1] {1};
        state.platforms[i].animFrames = 1;
        state.platforms[i].animIndex = 0;
        state.platforms[i].animCols = 8;
        state.platforms[i].animRows = 8;
        state.platforms[i].topLen = 0.6;
        state.platforms[i].botLen = 0.5;
        state.platforms[i].leftLen = 0.5;
        state.platforms[i].rightLen = 0.5;
        if (i < 12){
            state.platforms[i].position = glm::vec3(-5 + i,-3.25f,0);
        }
        else if (i < 15){
            state.platforms[i].position = glm::vec3(-15 + i,-1.35f,0);
        }
        else {
            state.platforms[i].position = glm::vec3(-12 + i, 2.00,0);
        }
        
    }
    
    for (int i = 0; i < PLATFORM_COUNT; i++){
        state.platforms[i].Update(0,NULL, NULL, NULL, 0, 0);
    }
    
    //initialize enemy
    state.enemy= new Entity[ENEMY_COUNT];
    GLuint enemyTextureID = LoadTexture("rhino_idle_01.png");
    
    //enemy1
    state.enemy->entityType = ENEMY;
    state.enemy[0].textureID = enemyTextureID;
    state.enemy[0].position = glm::vec3(4,-2.25,0);
    state.enemy[0].topLen = 0.1;
    state.enemy[0].botLen = 0.35;
    state.enemy[0].leftLen = 0.3;
    state.enemy[0].rightLen = 0.3;
    
    state.enemy[0].aitype = WAITANDGO;
    state.enemy[0].aistate = IDLE;
    
    state.enemy[0].speed = 1;
    state.enemy[0].acceleration = glm::vec3(0,-9.81f,0);
    
    //enemy2
    //std::cout << 12345678 << std::endl;
    //std::cout << state.enemy[1].entityType << std::endl;
    state.enemy[1].entityType = ENEMY;
    state.enemy[1].textureID = enemyTextureID;
    state.enemy[1].position = glm::vec3(-2,-1.0,0);
    state.enemy[1].topLen = 0.1;
    state.enemy[1].botLen = 0.35;
    state.enemy[1].leftLen = 0.3;
    state.enemy[1].rightLen = 0.3;
    
    state.enemy[1].aitype = PATROLL;
    state.enemy[1].aistate = WALKING;
    state.enemy[1].movement = glm::vec3(1,0,0);
    //state.enemy[1].aistate = IDLE;
    
    state.enemy[1].speed = 1;
    state.enemy[1].acceleration = glm::vec3(0,-9.81f,0);
    
    //enemy3
    state.enemy[2].entityType = ENEMY;
    state.enemy[2].textureID = enemyTextureID;
    state.enemy[2].position = glm::vec3(3,4.0,0);
    state.enemy[2].topLen = 0.1;
    state.enemy[2].botLen = 0.35;
    state.enemy[2].leftLen = 0.3;
    state.enemy[2].rightLen = 0.3;
    
    state.enemy[2].speed = 1;
    state.enemy[2].aitype = JUMP;
    state.enemy[2].aistate = IDLE;
    state.enemy[2].acceleration = glm::vec3(0,-9.81f,0);
    state.enemy[2].jumpPower = 5.0f;
    
    //win
    state.win= new Entity[6];
    GLuint wordTextureID = LoadTexture("font1.png");
    
    for (int i = 0; i < 6; i++){
        state.win[i].textureID = wordTextureID;
        state.win[i].animCols = 16;
        state.win[i].animRows = 16;
        state.win[i].entityType = WORD;
        state.win[i].isActive = false;
    }
    state.win[0].animIndices = new int{89};//Y
    state.win[1].animIndices = new int{79};//O
    state.win[2].animIndices = new int{85};//U
    state.win[3].animIndices = new int{87};//w
    state.win[4].animIndices = new int{73};//I
    state.win[5].animIndices = new int{78};//N
    for (size_t i = 0; i < 6; i++){
        state.win[i].position = glm::vec3(i*0.8-2,0,0);
        std::cout << "set!\n";
    }
    
    //lose
    state.lose= new Entity[7];
    
    for (int i = 0; i < 7; i++){
        state.lose[i].textureID = wordTextureID;
        state.lose[i].animCols = 16;
        state.lose[i].animRows = 16;
        state.lose[i].entityType = WORD;
        state.lose[i].isActive = false;
    }
    state.lose[0].animIndices = new int{89};//Y
    state.lose[1].animIndices = new int{79};//O
    state.lose[2].animIndices = new int{85};//U
    state.lose[3].animIndices = new int{76};//L
    state.lose[4].animIndices = new int{79};//O
    state.lose[5].animIndices = new int{83};//S
    state.lose[6].animIndices = new int{69};//E
    for (size_t i = 0; i < 7; i++){
        state.lose[i].position = glm::vec3(i*0.8-2,0,0);
    }
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
                        if (state.player->collidedBottom){
                            state.player->jump = true;
                        }
                        break;
                }
                break; // SDL_KEYDOWN
        }
    }
    
    const Uint8 *keys = SDL_GetKeyboardState(NULL);

    if (keys[SDL_SCANCODE_LEFT]) {
        state.player->movement.x = -1.0f;
        state.player->animIndices = state.player->animLeft;
    }
    else if (keys[SDL_SCANCODE_RIGHT]) {
        state.player->movement.x = 1.0f;
        state.player->animIndices = state.player->animRight;
    }
    

    if (glm::length(state.player->movement) > 1.0f) {
        state.player->movement = glm::normalize(state.player->movement);
    }

}

void gameOver(){
    for (size_t t = 0; t < ENEMY_COUNT; t++){
        state.enemy[t].isActive = false;
        state.player->speed = 0;
    }
}

void win(){
    for (size_t t = 0; t < 6; t++){
        state.win[t].isActive = true;
    }
    
    state.player->speed = 0;
    //state.player->acceleration = glm::vec3(0);
}


#define FIXED_TIMESTEP 0.0166666f
float lastTicks = 0;
float accumulator = 0.0f;

bool launch = false;
void Update() {
    for(int i = 0; i< 6; i++){
        state.win[i].Update(0, NULL, NULL, NULL, 0, 0);
    }
    for(int i = 0; i< 7; i++){
        state.lose[i].Update(0, NULL, NULL, NULL, 0, 0);
    }
    
    //std::cout << state.player->position.x << " " << state.player->position.y << std::endl;
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
        
        
        //update enemy
        for(int i = 0; i < ENEMY_COUNT; i++){
            state.enemy[i].Update(FIXED_TIMESTEP, state.player, state.platforms, state.enemy, PLATFORM_COUNT, ENEMY_COUNT);
            //std::cout << "enemy2: " <<state.enemy[1].position.x << " " << state.enemy[1].position.y << std::endl;
        }
        std::cout<< "enemy: " << state.enemy[1].collidedTop <<std::endl;
        
        state.player->Update(FIXED_TIMESTEP, state.player, state.platforms, state.enemy, PLATFORM_COUNT, ENEMY_COUNT);
        //std::cout << state.player->lastCollision << std::endl;
        if (state.player->lastCollision == ENEMY){
            if (state.player->collidedBottom){
                launch = true;
                if (state.enemy[0].collidedTop){
                    state.enemy[0].isActive = false;
                    state.player->kill++;
                    state.enemy[0].collidedTop = false;
                }
                else if (state.enemy[1].collidedTop){
                    state.enemy[1].isActive = false;
                    state.player->kill++;
                    state.enemy[1].collidedTop = false;
                }
                else if (state.enemy[2].collidedTop){
                    state.enemy[2].isActive = false;
                    state.player->kill++;
                    state.enemy[2].collidedTop = false;
                }
            //                switch (state.player->lastOne) {
            //                    case JUMP:
            //                        state.enemy[2].isActive = false;
            //                        state.player->kill++;
            //                        break;
            //                    case PATROLL:
            //                        state.enemy[1].isActive = false;
            //                        state.player->kill++;
            //                        break;
            //                    case WAITANDGO:
            //                        state.enemy[0].isActive = false;
            //                        state.player->kill++;
            //                        break;
            
                else if (state.player->collidedRight || state.player->collidedLeft || state.player->collidedTop){
                    gameOver();
                    for (size_t t = 0; t < 7; t++){
                        state.lose[t].isActive = true;
                    }
                    std::cout << "lose\n";
                }
            }
            
        }
        if (state.player->kill == 3){
            std::cout << "win\n";
            win();
            
        }
        std::cout <<"===============\n";
        deltaTime -= FIXED_TIMESTEP;
        
        
    }
    
    
    accumulator = deltaTime;
    
    
    
    
}

void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    for(int i = 0; i < PLATFORM_COUNT; i++){
        state.platforms[i].Render(&program);
    }
    for(int i = 0; i < 6; i++){
        state.win[i].Render(&program);
    }
    for(int i = 0; i < 7; i++){
        state.lose[i].Render(&program);
    }
    for(int i = 0; i < ENEMY_COUNT; i++){
        state.enemy[i].Render(&program);
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
    std::cout << launch;
    Shutdown();
    return 0;
}
