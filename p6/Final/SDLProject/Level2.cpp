#include "Level2.h"
#define LEVEL1_WIDTH 20
#define LEVEL1_HEIGHT 20

#define ENEMY_COUNT 9
#define TURRET_COUNT 4
#define TRACK_COUNT 5
#define BULLET_COUNT 39
#define PLAYER_BULLET 30
#define KEY_TOTAL 3

unsigned int level2_data[] = {
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 1, 1, 1, 1, 0, 0, 0, 2, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 2,
    2, 0, 0, 1, 1, 1, 0, 0, 2, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 2,
    2, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2,
    2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2,
    2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2,
    2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2,
    2, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2,
    2, 0, 0, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 2,
    2, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 2,
    2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2,
    2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2,
    2, 0, 0, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2,
    2, 0, 0, 2, 0, 0, 2, 0, 2, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 2,
    2, 0, 0, 2, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 0, 2,
    2, 0, 0, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 2, 0, 2,
    2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 2, 0, 2, 0, 2,
    2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 2, 0, 0, 2, 2, 2, 1, 2,
    2, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2
    
};

void Level2::Initialize() {
    
    state.nextScene = -1;
    GLuint mapTextureID = Util::LoadTexture("tile_set.png");
    state.map = new Map(LEVEL1_WIDTH, LEVEL1_HEIGHT, level2_data, mapTextureID, 1.0f, 3, 1);
    // Initialize Game Objects
    //======================================================================================
    // Initialize Player
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(9, -13, 0);
    state.player->movement = glm::vec3(0);
    state.player->speed = 2.0f;
    state.player->keyNum = 0;
    state.player->enemyRemain = ENEMY_COUNT;
    
    //4 directions
    state.player->textureIDs[0] = new GLuint(Util::LoadTexture("Tank/blueFront.png"));
    state.player->textureIDs[1] = new GLuint(Util::LoadTexture("Tank/blueBack.png"));
    state.player->textureIDs[2] = new GLuint(Util::LoadTexture("Tank/blueLeft.png"));
    state.player->textureIDs[3] = new GLuint(Util::LoadTexture("Tank/blueRight.png"));
    
    state.player->textureIndex = 0;
    state.player->textureID = *state.player->textureIDs[state.player->textureIndex];
    
    //ignore the boarder around player, so when collied, the feet touchs the ground
    state.player->height = 0.9f;
    state.player->width = 0.8f;
    
    //======================================================================================
    //Initialize keys
    state.key = new Entity[KEY_TOTAL];
    GLuint keyTextureID = Util::LoadTexture("key.png");
    for (size_t t = 0; t < KEY_TOTAL ; t++){
        state.key[t].entityType = KEY;
        state.key[t].textureIDs[0] = &keyTextureID;
        state.key[t].textureIndex = 0;
    }
    state.key[0].position = glm::vec3(9, -1, 0);
    state.key[1].position = glm::vec3(18, -9, 0);
    state.key[2].position = glm::vec3(18, -16, 0);
    
    for (size_t t = 0; t < KEY_TOTAL ; t++){
        state.key[t].Update(0, NULL, NULL, 0, state.map, NULL, 0);
    }
    
    //======================================================================================
    //initialize enemy
    state.enemy = new Entity[ENEMY_COUNT];
    for (size_t t = 0; t < ENEMY_COUNT ; t++){
        state.enemy[t].entityType = ENEMY;
    }
    //turret
    GLuint TurretTextureID = Util::LoadTexture("tank/turret.png");
    for (size_t t = 0; t < TURRET_COUNT ; t++){
        state.enemy[t].aitype = TURRET;
        state.enemy[t].textureIDs[0] = new GLuint(TurretTextureID);
        state.enemy[t].textureIndex = 0;
    }
    state.enemy[0].position = glm::vec3(9, -3, 0);
    state.enemy[1].position = glm::vec3(10, -3, 0);
    state.enemy[2].position = glm::vec3(13, -4, 0);
    state.enemy[3].position = glm::vec3(18, -11, 0);
    
    //track
    GLuint textureID1 = Util::LoadTexture("tank/redFront.png");
    GLuint textureID2 = Util::LoadTexture("tank/redBack.png");
    GLuint textureID3 = Util::LoadTexture("tank/redLeft.png");
    GLuint textureID4 = Util::LoadTexture("tank/redRight.png");
    for (size_t t = TURRET_COUNT; t < ENEMY_COUNT; t++){
        state.enemy[t].aitype = TRACK;
        state.enemy[t].textureIDs[0] = new GLuint(textureID1);
        state.enemy[t].textureIDs[1] = new GLuint(textureID2);
        state.enemy[t].textureIDs[2] = new GLuint(textureID3);
        state.enemy[t].textureIDs[3] = new GLuint(textureID4);
        state.enemy[t].textureIndex = 1;
        state.enemy[t].textureID = *state.enemy[t].textureIDs[state.enemy[t].textureIndex];
        state.enemy[t].movement = glm::vec3(0);
        state.enemy[t].speed = 2.0f;
        state.enemy[t].height = 0.9f;
        state.enemy[t].width = 0.8f;
    }
    state.enemy[4].position = glm::vec3(3,-7,0);
    state.enemy[5].position = glm::vec3(1,-5,0);
    state.enemy[6].position = glm::vec3(5,-11,0);
    state.enemy[7].position = glm::vec3(17,-2,0);
    state.enemy[8].position = glm::vec3(11,-16,0);
    state.enemy[9].position = glm::vec3(14,-9,0);
    //======================================================================================
    //intialize bullet
    state.bullet = new Entity[BULLET_COUNT];
    GLuint bulletTextureID = Util::LoadTexture("bullet.png");
    state.player->bulletUsed = 0;
    for(size_t t=0; t < BULLET_COUNT; t++){
        state.bullet[t].textureID = bulletTextureID;
        state.bullet[t].entityType = BULLET;
        state.bullet[t].speed = 4.0f;
        state.bullet[t].width = 0.1;
        state.bullet[t].height = 0.1;
    }
    for(size_t t = 0; t < PLAYER_BULLET;t++){//30
        state.bullet[t].bulletOwner = state.player;
        state.bullet[t].position = state.bullet[t].bulletOwner->position;
    }
    //turret
    state.bullet[30].bulletOwner = state.enemy; //turret one
    state.bullet[30].position = state.bullet[30].bulletOwner->position;
    state.bullet[31].bulletOwner = state.enemy+1; //turret two
    state.bullet[31].position = state.bullet[31].bulletOwner->position;
    state.bullet[32].bulletOwner = state.enemy+2; //turret 3
    state.bullet[32].position = state.bullet[32].bulletOwner->position;
    state.bullet[33].bulletOwner = state.enemy+3; //turret 4
    state.bullet[33].position = state.bullet[33].bulletOwner->position;
    
    //track
    state.bullet[34].bulletOwner = state.enemy + 4; //enemy 1
    state.bullet[34].position = state.bullet[34].bulletOwner->position;
    state.bullet[35].bulletOwner = state.enemy + 5; //enemy 2
    state.bullet[35].position = state.bullet[35].bulletOwner->position;
    state.bullet[36].bulletOwner = state.enemy + 6; //enemy 3
    state.bullet[36].position = state.bullet[36].bulletOwner->position;
    state.bullet[37].bulletOwner = state.enemy + 7; //enemy 4
    state.bullet[37].position = state.bullet[37].bulletOwner->position;
    state.bullet[38].bulletOwner = state.enemy + 8; //enemy 5
    state.bullet[38].position = state.bullet[38].bulletOwner->position;
    
    //ammo
    state.player->myBullet = state.bullet + 0;
    for( size_t t = 0; t < ENEMY_COUNT; t++){
        state.enemy[t].myBullet = state.bullet + 30 + t;
    }
    
    
    
    //state.enemy[1].isActive = false;
}
void Level2::Update(float deltaTime) {
    
    if (state.player->keyNum == KEY_TOTAL && state.player->enemyRemain == 0){
        state.player->movement = glm::vec3(0);
        std::cout << "you win!\n";
        return;
    }
    
    for(size_t t = 0; t < BULLET_COUNT; t++){
        state.bullet[t].bulletUpdate(deltaTime, state.player, state.enemy, ENEMY_COUNT, state.map);
    }
    
    for (size_t t = 0; t < ENEMY_COUNT; t++){
        state.enemy[t].Update(deltaTime, state.player, state.enemy, ENEMY_COUNT, state.map, state.key, KEY_TOTAL);
    }
    
    
    state.player->Update(deltaTime, state.player, state.enemy, ENEMY_COUNT, state.map, state.key, KEY_TOTAL);
    
    
}

void Level2::Render(ShaderProgram *program) {
    state.map->Render(program);
    for(size_t t = 0; t < KEY_TOTAL; t++){
        state.key[t].Render(program);
    }
    for(size_t t = 0; t < BULLET_COUNT; t++){
        state.bullet[t].Render(program);
    }
    for (size_t t = 0; t < ENEMY_COUNT; t++){
        state.enemy[t].Render(program);
    }
    state.player->Render(program);
    
    GLuint textureID = Util::LoadTexture("font1.png");
    if (state.player->isActive == false){ 
        std::cout << "draw\n";
        Util::DrawText(program, textureID, "You Lost", 1.0f, -0.4, glm::vec3(state.player->position.x-2, state.player->position.y +1 ,0));
        //Util::DrawText(program, textureID, "Press SPACE", 0.8f, -0.4, glm::vec3(3, -4, 0));
    }
    if (state.player->keyNum == KEY_TOTAL && state.player->enemyRemain == 0){
        Util::DrawText(program, textureID, "You Win", 1.0f, -0.4, glm::vec3(state.player->position.x-2, state.player->position.y +1 ,0));
    }
}
