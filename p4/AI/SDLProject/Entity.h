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

enum EntityType{PLAYER, PLATFORM, ENEMY, WORD};

enum AIType{WALKER, WAITANDGO, PATROLL, JUMP};
enum AIState{IDLE, WALKING, ATTACK};

class Entity {
public:
    
    EntityType entityType;
    AIType aitype;
    AIState aistate;
    
    glm::vec3 position;
    glm::vec3 movement;
    glm::vec3 acceleration;
    glm::vec3 velocity;
    
    float width = 1;
    float height = 1;
    
    float topLen = 0.5;
    float botLen = 0.5;
    float leftLen = 0.5;
    float rightLen = 0.5;
    
    
    
    //determine if this entity can jump
    bool jump = false;
    float jumpPower = 0;
    
    float speed;
    
    
    GLuint textureID;
    
    glm::mat4 modelMatrix;
    
    int *animRight = NULL;
    int *animLeft = NULL;
    int *animUp = NULL;
    int *animDown = NULL;

    int *animIndices = NULL;
    int animFrames = 0;
    int animIndex = 0;
    float animTime = 0;
    int animCols = 0;
    int animRows = 0;
    
    // if not active, no update no render no checkCollsion
    bool isActive = true;
    
    
    bool collidedTop = false;
    bool collidedBottom = false;
    bool collidedLeft = false;
    bool collidedRight = false;
    Entity();
    EntityType lastCollision;
    AIType lastOne;
    int kill = 0;
    bool checkCollision(Entity* other);
    void checkCollisionsY(Entity *objects, int objectCount);
    void checkCollisionsX(Entity *objects, int objectCount);
    void Update(float deltaTime, Entity* player,  Entity* platforms, Entity* enemy, int platformCount, int enemyCount);
    void Render(ShaderProgram *program);
    void DrawSpriteFromTextureAtlas(ShaderProgram *program, GLuint textureID, int index);
    
    //special method for this game
    void DrawPlayer(ShaderProgram *program, GLuint textureID, int index);
    
    //for enemy
    void AI(Entity* player, Entity* platforms, int platformCount);
    void AIWalker();
    void AIWaitAndGo(Entity* player);
    void jumper(Entity* player);
    bool checkPoint(Entity* platforms, int platformCount);
    
};
