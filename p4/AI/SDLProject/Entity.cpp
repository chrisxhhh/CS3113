#include "Entity.h"

Entity::Entity()
{
    position = glm::vec3(0);
    movement = glm::vec3(0);
    acceleration = glm::vec3(0);
    velocity = glm::vec3(0);
    speed = 0;
    
    modelMatrix = glm::mat4(1.0f);
}

bool Entity::checkCollision(Entity* other){
    //std::cout << "checking\n";
    if  (isActive == false || other->isActive == false) return false;
    float xdist;
    float ydist;
    
    float centerDistanceX = position.x - other->position.x;
    if (centerDistanceX <= 0){
        xdist = -centerDistanceX - rightLen - other->leftLen;
    }
    else {
        xdist = centerDistanceX - leftLen - other->rightLen;
    }
    float centerDistanceY = position.y - other->position.y;
    if (centerDistanceY <= 0){
        ydist = -centerDistanceY - topLen - other->botLen;
    }
    else {
        ydist = centerDistanceY - botLen - other->topLen;
    }
    
    if (xdist < 0 && ydist < 0) {
        lastCollision = other->entityType;
        if( lastCollision == ENEMY){
            lastOne = other->aitype;
        }
        return true;
    }
    return false;
}

 void Entity::checkCollisionsY(Entity *objects, int objectCount){
    for (int i = 0; i < objectCount; i++){
        Entity *object = &objects[i];
        if (checkCollision(object)){
            float ydist = fabs(position.y - object->position.y);
            //std::cout << "ydist: " << ydist << std::endl;
            float penetrationY;
            if (velocity.y > 0){
                penetrationY = fabs(ydist - topLen - object->botLen);
                
                position.y -= penetrationY;
                velocity.y = 0;
                collidedTop = true;
                object->collidedBottom = true;
            }
            else if(velocity.y < 0) {
                penetrationY = fabs(ydist - botLen - object->topLen);
                //std::cout << "penetrY: " << penetrationY << std::endl;
                position.y += penetrationY;
                velocity.y = 0;
                collidedBottom = true;
                object->collidedTop = true;
            }
        }
    }
 }

 void Entity::checkCollisionsX(Entity *objects, int objectCount){
    for (int i = 0; i < objectCount; i++){
        Entity* object = &objects[i];
        if (checkCollision(object)){
            float xdist = fabs(position.x - object->position.x);
            float penetrationX;
            if (velocity.x > 0){
                penetrationX = fabs(xdist - rightLen - object->leftLen);
                position.x -= penetrationX;
                velocity.x = 0;
                collidedRight = true;
                object->collidedLeft = true;
                
            }
            else if (velocity.x <0){
                penetrationX = fabs(xdist - leftLen - object->rightLen);
                position.x += penetrationX;
                velocity.x = 0;
                collidedLeft = true;
                object->collidedRight= true;
            }
        }
    }
 }

void Entity::AIWalker(){
    movement = glm::vec3(-1,0,0);
}

void Entity::jumper(Entity* player){
    switch(aistate){
        case IDLE:
            if((position.x - player->position.x) < 3.0f){
                
                aistate = ATTACK;
                jump = true;
            }
            break;
        case ATTACK:{
            //std::cout << "jump"<< std::endl;
            movement = glm::vec3(-1,0,0);
            
        }
    }
}
void Entity::AIWaitAndGo(Entity* player){
    switch(aistate){
        case IDLE:
            if (glm::distance(position, player->position) < 3.0f){
                aistate = WALKING;
            }
            break;
            
        case WALKING:
            if (player->position.x < position.x){
                movement = glm::vec3(-1,0,0);
            }
            else{
                movement = glm::vec3(1,0,0);
            }
            
            break;
    }
    
}


bool Entity::checkPoint(Entity* platforms, int platformCount){
    glm::vec3 leftCheck = glm::vec3(position.x-1, position.y-1, 0);
    glm::vec3 rightCheck = glm::vec3(position.x+1, position.y-1, 0);
    int countLeft = 0;
    int countRight = 0;
    for (int i = 0; i < platformCount; i++){
        Entity* object = &platforms[i];
        
        float xDistLeft = fabs(leftCheck.x - object->position.x);
        float xDistRight = fabs(rightCheck.x - object->position.x);
        float yDistLeft = fabs(leftCheck.y - object->position.y);
        float yDistRight = fabs(rightCheck.y - object->position.y);
        //std::cout << xDistLeft << " " << yDistLeft << std::endl;
        //if it did not collide
        if (xDistLeft > 0.6 && yDistLeft < 0.7){
            countLeft++;
        }
        if (xDistRight > 0.6 && yDistRight < 0.7){
            countRight++;
        }
    }
    std::cout << "number: "<<  countLeft << " " << countRight << std::endl;
    if (countLeft == 3 || countRight == 3){
        return false;
    }
    return true;
}

void Entity::AI(Entity* player, Entity* platforms, int platformCount){
    switch(aitype){
        case WALKER:
            AIWalker();
            break;
        case WAITANDGO:
            AIWaitAndGo(player);
            break;
        case PATROLL:
            if (!checkPoint(platforms, platformCount)){
                //std::cout << "checked!" << std::endl;
                movement = -movement;
            }
            else {
                //std::cout << "also checked\n" ;
            }
            //movement = glm::vec3(0);

            break;
        case JUMP:
            jumper(player);
    }
}


void Entity::Update(float deltaTime, Entity* player, Entity* platforms, Entity* enemy, int platformCount, int enemyCount)
{
    if (isActive == false){
        return;
    }
    
    collidedTop = false;
    collidedBottom = false;
    collidedLeft = false;
    collidedRight = false;
    
    if(entityType == ENEMY){
        AI(player, platforms, platformCount);
    }
    
    if (animIndices != NULL) {
        if (glm::length(movement) != 0) {
            animTime += deltaTime;

            if (animTime >= 0.25f)
            {
                animTime = 0.0f;
                animIndex++;
                if (animIndex >= animFrames)
                {
                    animIndex = 0;
                }
            }
        } else {
            animIndex = 0;
        }
    }
    
    //jump
    if (jump){
        jump = false; 
        velocity.y += jumpPower;
    }
    //check collistion but dont move
//    for (int i=0 ; i < platformCount; i++){
//        if (checkCollision(&platforms[i])){
//            return;
//        }
//    }
    
    
    velocity.x = movement.x * speed;
    velocity += acceleration * deltaTime;
    
    //update x and y separately, so we can check collistion in to direction
    position.y += velocity.y * deltaTime; // Move on Y
    checkCollisionsY(platforms, platformCount);
    if (entityType == PLAYER){
        checkCollisionsY(enemy, enemyCount);
    }
    
    
    position.x += velocity.x * deltaTime; // Move on X
    checkCollisionsX(platforms, platformCount);
    if (entityType == PLAYER){
        checkCollisionsX(enemy, enemyCount);
    }
    
    
    
    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
}

void Entity::DrawSpriteFromTextureAtlas(ShaderProgram *program, GLuint textureID, int index)
{
    float u = (float)(index % animCols) / (float)animCols;
    float v = (float)(index / animCols) / (float)animRows;
    
    float width = 1.0f / (float)animCols;
    float height = 1.0f / (float)animRows;
    
    float texCoords[] = { u, v + height, u + width, v + height, u + width, v,
        u, v + height, u + width, v, u, v};
    
    float vertices[]  = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);
    
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}



void Entity::Render(ShaderProgram *program) {
    if (isActive == false){
        return;
    }
    program->SetModelMatrix(modelMatrix);
    
    if (animIndices != NULL) {
        DrawSpriteFromTextureAtlas(program, textureID, animIndices[animIndex]);
        return;
    }
    
    float vertices[]  = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
    
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);
    
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}
