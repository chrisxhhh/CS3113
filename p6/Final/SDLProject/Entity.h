#pragma once
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
#include "Map.h"



enum EntityType{PLAYER, PLATFORM, ENEMY, BULLET, KEY, BACKGROUND};

enum AIType{WALKER, TRACK, TURRET};
enum AIState{IDLE, WALKING, ATTACKING};

class Entity {
public:
    
    EntityType entityType;
    AIType aitype;
    AIState aistate;
    
    glm::vec3 position;
    glm::vec3 movement;
    glm::vec3 acceleration;
    glm::vec3 velocity;
    
    //for enemy
    int direction = 1;
    
    float width = 1;
    float height = 1;
    
    float speed;
    bool bulletMove = false;
    bool flying = false;
    int bulletUsed;
    Entity* bulletOwner;
    Entity* myBullet;
    Entity* lastCollision;
    
    int keyNum;
    int enemyRemain;
    
    GLuint textureID;
    GLuint* textureIDs[4];
    int textureIndex;
    
    glm::mat4 modelMatrix;
    
//    int *animRight = NULL;
//    int *animLeft = NULL;
//    int *animUp = NULL;
//    int *animDown = NULL;
//
//    int *animIndices = NULL;
//    int animFrames = 0;
//    int animIndex = 0;
//    float animTime = 0;
//    int animCols = 0;
//    int animRows = 0;
    
    // if not active, no update no render no checkCollsion
    bool isActive = true;
    
    bool collidedTop = false;
    bool collidedBottom = false;
    bool collidedLeft = false;
    bool collidedRight = false;
    Entity();
    
    //for entity
    bool checkCollision(Entity* other);
    void checkCollisionY(Entity *objects, int objectCount);
    void checkCollisionX(Entity *objects, int objectCount);
    
    //for map
    void CheckCollisionY(Map* map);
    void CheckCollisionX(Map* map);
    
    void Update(float deltaTime, Entity* player, Entity* enemy, int enemyCount,  Map* map, Entity* Key, int keyCount);
    void Render(ShaderProgram *program);
    void DrawSpriteFromTextureAtlas(ShaderProgram *program, GLuint textureID, int index);
    
    //for enemy
    void AI(Entity* player);
    void AIWalker();
    void AIWaitAndGo(Entity* player);
    void AITRACK(Entity* player);
    
    //for bullet
    void bulletUpdate(float deltaTime, Entity* player, Entity* enemy, int enemyCount, Map* map);
    
    
};
