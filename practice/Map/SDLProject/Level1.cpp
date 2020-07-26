#include "Level1.h"
#define LEVEL1_WIDTH 14
#define LEVEL1_HEIGHT 8
#define ENEMY_COUNT 1


unsigned int level1_data[] = {
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1,
    3, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2,
    3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2
};

void Level1::Initialize() {
    
    state.nextScene = -1;
    
    GLuint mapTextureID = Util::LoadTexture("tileset1.png");
    state.map = new Map(LEVEL1_WIDTH, LEVEL1_HEIGHT, level1_data, mapTextureID, 1.0f, 4, 1);
    // Initialize Game Objects
    // Initialize Player
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(5, 0, 0);
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0,-9.81f,0);
    state.player->speed = 2.0f;
    state.player->textureID = Util::LoadTexture("george_0.png");
    
    state.player->animRight = new int[4] {3, 7, 11, 15};
    state.player->animLeft = new int[4] {1, 5, 9, 13};
    state.player->animUp = new int[4] {2, 6, 10, 14};
    state.player->animDown = new int[4] {0, 4, 8, 12};

    state.player->animIndices = state.player->animRight;
    state.player->animFrames = 4;
    state.player->animIndex = 0;
    state.player->animTime = 0;
    state.player->animCols = 4;
    state.player->animRows = 4;
    
    //ignore the boarder around player, so when collied, the feet touchs the ground
    state.player->height = 0.8f;
    state.player->width = 0.8f;
    
    state.player->jumpPower = 5.0f;
    
    //initialize enemy
    state.enemy= new Entity[ENEMY_COUNT];
    GLuint enemyTextureID = Util::LoadTexture("ctg.png");
    
    state.enemy->entityType = ENEMY;
    state.enemy[0].textureID = enemyTextureID;
    state.enemy[0].position = glm::vec3(4,-2.25,0);
    
    state.enemy[0].aitype = WAITANDGO;
    state.enemy[0].aistate = IDLE;
    state.enemy[0].isActive = false;
    
    state.enemy[0].speed = 1;
    
    
}
void Level1::Update(float deltaTime) {
    state.player->Update(deltaTime, state.player, state.enemy, ENEMY_COUNT, state.map);
    
    
    if (state.player->position.x >= 12){
        state.nextScene = 1;
    }
}

void Level1::Render(ShaderProgram *program) {
    state.map->Render(program);
    state.player->Render(program);
}
