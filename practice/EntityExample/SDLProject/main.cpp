'/#define GL_SILENCE_DEPRECATION

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

#include "Entity.h"

#define ENEMY_COUNT 3

//game state
struct GameState{
    Entity* player;
    Entity* enemy;
};
GameState state;

SDL_Window* displayWindow;
bool gameIsRunning = true;
ShaderProgram program; //the target object for rendering

glm::mat4 viewMatrix, projectionMatrix, modelMatrix;

//delta time
float lastTicks = 0.0f;



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
    //playerTextureID = LoadTexture("player.png");
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
    
    //initialize player
    state.player = new Entity();
    state.player->position = glm::vec3(0);
    state.player->movement = glm::vec3(0);
    state.player->speed = 1.0f;
    state.player->textureID= LoadTexture("george_0.png");
    //player animation
    state.player->animRight = new int[4]{3,7,11,15};
    state.player->animLeft = new int[4]{1,5,9,13};
    state.player->animUp = new int[4]{2,6,10,14};
    state.player->animDown = new int[4]{0,4,8,12};
    state.player->animIndices = state.player->animRight;
    state.player->animFrame = 4;
    state.player->animIndex = 0;
    state.player->animTime = 0;
    state.player->animCols = 4;
    state.player->animRows = 4;
    
    //initialize enemy
    state.enemy = new Entity[ENEMY_COUNT];
    
    
    GLuint enemyTextureID = LoadTexture("ctg.png");
    state.enemy[0].textureID = enemyTextureID;
    state.enemy[0].position = glm:: vec3(-2.0f, -2.0f, 0.0f);
    
    state.enemy[1].textureID = enemyTextureID;
    state.enemy[1].position = glm:: vec3(0.0f,-2.0f,0.0f);
    
    state.enemy[2].textureID = enemyTextureID;
    state.enemy[2].position = glm:: vec3(2.0f, -2.0f,0.0f);
    
}

void processInput(){
    state.player->movement = glm::vec3(0);
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                gameIsRunning = false;
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym){
                    case SDLK_LEFT:
                        //MOVE LEFT
                        break;
                    case SDLK_RIGHT:
                        //MOVE RIGHT
                        break;
                    case SDLK_SPACE:
                        break;
                }
                break;//SDL_KEYDOWN
        }
    }
    const Uint8* keys = SDL_GetKeyboardState(NULL);
    
    if (keys[SDL_SCANCODE_LEFT]){
        state.player->movement.x = -1.0f;
        state.player->animIndices = state.player->animLeft;
    }
    else if (keys[SDL_SCANCODE_RIGHT]){
        state.player->movement.x = 1.0f;
        state.player->animIndices = state.player->animRight;
    }
    
    if (keys[SDL_SCANCODE_UP]){
        state.player->movement.y = 1.0f;
        state.player->animIndices = state.player->animUp;
    }
    else if (keys[SDL_SCANCODE_DOWN]){
        state.player->movement.y = -1.0f;
        state.player->animIndices = state.player->animDown;
    }
    if (glm::length(state.player->movement) > 1.0f){
        state.player->movement = glm::normalize(state.player->movement);
    }
}

void update(){
    //update delta time
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;
    
    state.player->Update(deltaTime);
    for (int i = 0; i< ENEMY_COUNT; i++){
        state.enemy[i].Update(deltaTime);
    }
}

void render() {
    glClear(GL_COLOR_BUFFER_BIT);
    state.player->Render(&program);
    
    for (int i = 0; i< ENEMY_COUNT; i++){
        state.enemy[i].Render(&program);
    }
    SDL_GL_SwapWindow(displayWindow); }



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
