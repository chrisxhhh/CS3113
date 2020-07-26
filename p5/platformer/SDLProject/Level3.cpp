#include "Level3.h"
#define LEVEL2_WIDTH 15
#define LEVEL2_HEIGHT 8
#define ENEMY_COUNT 1
#define BULLET_COUNT 10




unsigned int level3_data[] = {
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 3,
    3, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 3,
    3, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
    3, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3
};

void Level3::Initialize() {
    
    state->nextScene = -1;
    
    GLuint mapTextureID = Util::LoadTexture("tileset1.png");
    state->map = new Map(LEVEL2_WIDTH, LEVEL2_HEIGHT, level3_data, mapTextureID, 1.0f, 4, 1);
    // Initialize Game Objects
    // Initialize Player
    state->player = new Entity();
    state->player->entityType = PLAYER;
    state->player->position = glm::vec3(2, -3, 0);
    state->player->movement = glm::vec3(0);
    state->player->acceleration = glm::vec3(0,-9.81f,0);
    state->player->speed = 2.0f;
    
    state->player->textureID_idle = Util::LoadTexture("fox_idle.png");
    state->player->textureID_right = Util::LoadTexture("fox_run_right.png");
    state->player->textureID_left = Util::LoadTexture("fox_run_left.png");
    
    state->player->textureID = state->player->textureID_idle;
    state->player->animIdle = new int[4] {0,1,2,3};
    state->player->animRight = new int[6] {0, 1, 2, 3,4, 5};
    state->player->animLeft = new int[6] {5,4,3,2,1,0};

    state->player->animIndices = state->player->animIdle;
    state->player->animFrames = 4;
    state->player->animIndex = 0;
    state->player->animTime = 0;
    state->player->animCols = 2;
    state->player->animRows = 2;
    
    //ignore the boarder around player, so when collied, the feet touchs the ground
    state->player->height = 0.9f;
    state->player->width = 0.6f;
    
    state->player->jumpPower = 5.0f;
    
    //initialize enemy
    state->enemy= new Entity[ENEMY_COUNT];
    GLuint enemyTextureID = Util::LoadTexture("ctg.png");
    
    state->enemy->entityType = ENEMY;
    state->enemy[0].textureID = enemyTextureID;
    state->enemy[0].position = glm::vec3(6,-1,0);
    state->enemy[0].acceleration = glm::vec3(0,-9.81f,0);
    
    state->enemy[0].aitype = WAITANDGO;
    state->enemy[0].aistate = IDLE;
    //state->enemy[0].isActive = false;
    
    state->enemy[0].speed = 1;
    
    //initialize door
    state->door = new Entity();
    state->door->entityType = DOOR;
    state->door->textureID = Util::LoadTexture("door.png");
    state->door->position = glm::vec3(12,-3,0);
    
    state->door->width = 0.1f;
    state->door->height = 0.1f;
    
    state->door->Update(0, NULL, NULL,0 ,state->map, NULL);
    
    //intialize lives
    state->live = new Entity[state->lives];
    
    GLuint liveTextureID = Util::LoadTexture("gem-1.png");
    for (size_t t =0; t < state->lives ; t++){
        state->live[t].textureID = liveTextureID;
        state->live[t].entityType = LIVES;
        state->live[t].position = glm::vec3(t+0.5,-0.5,0);
        state->live[t].Update(0, NULL, NULL, 0, state->map, NULL);
    }
    
    //intialize bullet
    state->bullet = new Entity[10];
    GLuint bulletTextureID = Util::LoadTexture("laser-bolts.png");
    state->player->bulletUsed = 0;
    for(size_t t=0; t < BULLET_COUNT; t++){
        state->bullet[t].textureID = bulletTextureID;
        state->bullet[t].entityType = BULLET;
        state->bullet[t].position = state->player->position;
        state->bullet[t].animIndices = new int[1] {0};
        state->bullet[t].animFrames = 1;
        state->bullet[t].animIndex = 0;
        state->bullet[t].animTime = 0;
        state->bullet[t].animCols = 2;
        state->bullet[t].animRows = 2;
        state->bullet[t].speed = 2.0f;
        state->bullet[t].width = 0.1;
        state->bullet[t].height = 0.1;
        
        //state->bullet[t].Update(0, NULL, NULL, 0, state->map, NULL);
    }
}
void Level3::Update(float deltaTime) {
    state->enemy->Update(deltaTime, state->player, 0, NULL, state->map, NULL);
    state->player->Update(deltaTime, state->player, state->enemy, ENEMY_COUNT, state->map, state->door);
    
    for(size_t t = 0; t < BULLET_COUNT; t++){
        state->bullet[t].bulletUpdate(deltaTime, state->player, state->enemy, ENEMY_COUNT, state->map);
    }
    
    if (state->player->lastCollision == ENEMY){
        state->lives--;
        Initialize();
    }
    else if (state->player->lastCollision == DOOR){
        std::cout << "win\n";
        state->player->speed = 0;
    }
    if (state->player->position.y < -10){
        state->lives--;
        Initialize();
    }
    if (state->lives == 0){
        //Initialize();
        state->player->speed = 0;
    }
}

void Level3::Render(ShaderProgram *program) {
    state->map->Render(program);
    state->door->Render(program);
    state->enemy->Render(program);
    for(size_t t = 0; t < BULLET_COUNT; t++){
        state->bullet[t].Render(program);
    }
    state->player->Render(program);
    for(size_t t = 0; t < state->lives; t++){
        state->live[t].Render(program);
    }
    GLuint textureID = Util::LoadTexture("font1.png");
    if (state->lives ==0){
        Util::DrawText(program, textureID, "YOU LOSE", 0.8, 0, glm::vec3(2,-2,0));
    }
    else if(state->player->lastCollision == DOOR){
        Util::DrawText(program, textureID, "YOU WIN", 0.8, 0, glm::vec3(7,-2,0));
    }
}
