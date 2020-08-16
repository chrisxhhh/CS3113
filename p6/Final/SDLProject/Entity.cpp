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
    if  (isActive == false || other->isActive == false) return false;
    float xdist = fabs(position.x - other->position.x) - ((width + other->width)/2.0f);
    float ydist = fabs(position.y - other->position.y) - ((height + other->height)/2.0f);
    
    if (xdist < 0 && ydist < 0) {
        lastCollision = other;
        return true;
    }
    return false;
}

 void Entity::checkCollisionY(Entity *objects, int objectCount){
    for (int i = 0; i < objectCount; i++){
        Entity *object = &objects[i];
        if (checkCollision(object)){
            float ydist = fabs(position.y - object->position.y);
            float penetrationY = fabs(ydist - (height / 2.0f) - (object->height / 2.0f));
            if (velocity.y > 0) {
                position.y -= penetrationY;
                velocity.y = 0;
                collidedTop = true;
            }
            else if (velocity.y < 0){
                position.y += penetrationY;
                velocity.y = 0;
                collidedBottom = true;
            }
        }
    }
 }

 void Entity::checkCollisionX(Entity *objects, int objectCount){
    for (int i = 0; i < objectCount; i++){
        Entity *object = &objects[i];
        if (checkCollision(object)){
            float xdist = fabs(position.x - object->position.x);
            float penetrationX = fabs(xdist - (width / 2.0f) - (object->width / 2.0f));
            if (velocity.x > 0) {
                position.x -= penetrationX;
                velocity.x = 0;
                collidedRight = true;
            }
            else if (velocity.x < 0) {
                position.x += penetrationX;
                velocity.x = 0;
                collidedLeft = true;
            }
        }
    }
 }

//====================================for map=======================================
void Entity::CheckCollisionY(Map *map){
    // Probes for tiles
    glm::vec3 top = glm::vec3(position.x, position.y + (height / 2), position.z);
    glm::vec3 top_left = glm::vec3(position.x - (width / 2), position.y + (height / 2), position.z);
    glm::vec3 top_right = glm::vec3(position.x + (width / 2), position.y + (height / 2), position.z);
    
    glm::vec3 bottom = glm::vec3(position.x, position.y - (height / 2), position.z);
    glm::vec3 bottom_left = glm::vec3(position.x - (width / 2), position.y - (height / 2), position.z);
    glm::vec3 bottom_right = glm::vec3(position.x + (width / 2), position.y - (height / 2), position.z);

    float penetration_x = 0;
    float penetration_y = 0;
    if (map->IsSolid(top, &penetration_x, &penetration_y) && velocity.y > 0) {
        position.y -= penetration_y;
        velocity.y = 0;
        collidedTop = true;
        if (entityType == BULLET) bulletMove = false;
    }
    
    else if (map->IsSolid(top_left, &penetration_x, &penetration_y) && velocity.y > 0) {
        position.y -= penetration_y; velocity.y = 0;
        collidedTop = true;
        if (entityType == BULLET) bulletMove = false;
    }
    else if (map->IsSolid(top_right, &penetration_x, &penetration_y) && velocity.y > 0) {
        position.y -= penetration_y; velocity.y = 0;
        collidedTop = true;
        if (entityType == BULLET) bulletMove = false;
    }

    if (map->IsSolid(bottom, &penetration_x, &penetration_y) && velocity.y < 0) {
        position.y += penetration_y;
        velocity.y = 0;
        collidedBottom = true;
        if (entityType == BULLET) bulletMove = false;
    }
    else if (map->IsSolid(bottom_left, &penetration_x, &penetration_y) && velocity.y < 0) {
        position.y += penetration_y;
        velocity.y = 0;
        collidedBottom = true;
        if (entityType == BULLET) bulletMove = false;
    }
    else if (map->IsSolid(bottom_right, &penetration_x, &penetration_y) && velocity.y < 0) {
        position.y += penetration_y;
        velocity.y = 0;
        collidedBottom = true;
        if (entityType == BULLET) bulletMove = false;
    }
}

void Entity::CheckCollisionX(Map *map){
// Probes for tiles
    glm::vec3 left = glm::vec3(position.x - (width / 2), position.y, position.z);
    glm::vec3 right = glm::vec3(position.x + (width / 2), position.y, position.z);
    float penetration_x = 0; float penetration_y = 0;
    if (map->IsSolid(left, &penetration_x, &penetration_y) && velocity.x < 0) {
        position.x += penetration_x + 0.05;
        velocity.x = 0;
        collidedLeft = true;
        if (entityType == BULLET) bulletMove = false;
    }
    if (map->IsSolid(right, &penetration_x, &penetration_y) && velocity.x > 0) {
        position.x -= penetration_x + 0.05;
        velocity.x = 0;
        collidedRight = true;
        if (entityType == BULLET) bulletMove = false;
    }    
}


void Entity::AITRACK(Entity* player){
    movement = glm::vec3(0);
    if (myBullet->bulletMove == false){
        myBullet->movement = glm::vec3(0);
        flying = false;
    }
    
    float x_dist = player->position.x - position.x;
    float y_dist = player->position.y - position.y;
    std::cout <<"x_dist: " << x_dist << std::endl;
    std::cout <<"y_dist: " << y_dist << std::endl;
    
    int lottery = rand() % 60;
    
    if (collidedTop || collidedBottom || collidedLeft || collidedRight || lottery == 0){
        direction = rand() % 4;
    }
    
    if (direction == 0){
        movement.y = 1;
    }
    else if (direction == 1){
        movement.y = -1;
    }
    else if (direction == 2){
        movement.x = -1;
    }

    else if (direction == 3){
        movement.x = 1;
    }
    
    if (myBullet->bulletMove == false && lottery == 30){
        myBullet->bulletMove = true;
    }
    
    
    
    if (movement.y > 0 ) textureIndex = 0;
    else if (movement.y < 0) textureIndex = 1;
    else if (movement.x > 0) textureIndex = 3;
    else if (movement.x < 0) textureIndex = 2;
}
void Entity::AI(Entity* player){
    switch(aitype){
        case TURRET:
            if (player->position.y - position.y < 0 && player->position.y - position.y > -4 && player->position.x - position.x < 0.3){
                std::cout << "active1\n";
                myBullet->movement.y = -1.0f;
                myBullet->bulletMove = true;
            }
            //if bullet collided, goes back to owner
            break;
        case TRACK:
            AITRACK(player);
            if (myBullet->bulletMove == true){
                if (!flying){
                    if (movement.y > 0 ) myBullet->movement.y = 1.0f;
                    else if (movement.y < 0) myBullet->movement.y = -1.0f;
                    else if (movement.x > 0) myBullet->movement.x = 1.0f;
                    else if (movement.x < 0) myBullet->movement.x = -1.0f;
                    flying = true;
                }
            }
            break;
    }
}


void Entity::Update(float deltaTime, Entity* player, Entity* enemy, int enemyCount,  Map* map,  Entity* key, int keyCount){
    if (isActive == false){
        return;
    }
    
    collidedTop = false;
    collidedBottom = false;
    collidedLeft = false;
    collidedRight = false;
    
    
    textureID = *textureIDs[textureIndex];
    
    switch (entityType) {
        case PLAYER:
            if (bulletUsed > 29){
                bulletUsed = 0;
            }
            for (size_t t = 0; t < keyCount; t++){
                if (checkCollision(key+t)){
                    lastCollision->isActive = false;
                    keyNum++;
                }
            }
            if (keyNum == 3){
                std::cout<< "collected\n";
            }
            break;
            
        default:
            break;
    }
    
    //player shooting
    if (acceleration.x > 0){
        acceleration.x -= 1.0f;
    }
    else if (acceleration.x < 0){
        acceleration.x += 1.0f;
    }
    if (acceleration.y > 0){
        acceleration.y -= 1.0f;
    }
    else if (acceleration.y < 0){
        acceleration.y += 1.0f;
    }
    
    
    velocity.x = movement.x * speed;
    velocity.y = movement.y * speed;
    velocity += acceleration * deltaTime;
    
    //update x and y together
    //velocity += acceleration * deltaTime;
    
    // Move on Y
    position.y += velocity.y * deltaTime;
    CheckCollisionY(map);
    if (entityType == PLAYER){
        checkCollisionY(enemy, enemyCount);
    }
    
    
    // Move on X
    position.x += velocity.x * deltaTime;
    CheckCollisionX(map);
    if (entityType == PLAYER){
        checkCollisionX(enemy, enemyCount);
    }
    
    if(entityType == ENEMY){
        AI(player);
    }
    
    
    
    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
}

//void Entity::DrawSpriteFromTextureAtlas(ShaderProgram *program, GLuint textureID, int index)
//{
//    float u = (float)(index % animCols) / (float)animCols;
//    float v = (float)(index / animCols) / (float)animRows;
//
//    float width = 1.0f / (float)animCols;
//    float height = 1.0f / (float)animRows;
//
//    float texCoords[] = { u, v + height, u + width, v + height, u + width, v,
//        u, v + height, u + width, v, u, v};
//
//    float vertices[]  = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
//
//    glBindTexture(GL_TEXTURE_2D, textureID);
//
//    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
//    glEnableVertexAttribArray(program->positionAttribute);
//
//    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
//    glEnableVertexAttribArray(program->texCoordAttribute);
//
//    glDrawArrays(GL_TRIANGLES, 0, 6);
//
//    glDisableVertexAttribArray(program->positionAttribute);
//    glDisableVertexAttribArray(program->texCoordAttribute);
//}

void Entity::Render(ShaderProgram *program) {
    if (isActive == false){
        return;
    }
    program->SetModelMatrix(modelMatrix);
    
//    if (animIndices != NULL) {
//        DrawSpriteFromTextureAtlas(program, textureID, animIndices[animIndex]);
//        return;
//    }
    
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

void Entity::bulletUpdate(float deltaTime, Entity* player, Entity* enemy, int enemyCount, Map* map){
    if (isActive == false){
        return;
    }
    switch (bulletOwner->entityType) {
        case PLAYER:
            if (bulletMove == true){
                std::cout << "shoot\n";
                for (size_t t = 0 ; t < enemyCount; t++){
                    if (checkCollision(enemy + t)){
                        lastCollision->isActive = false;
                        lastCollision->myBullet->isActive = false;
                        bulletMove = false;
                        bulletOwner->enemyRemain--;
                    }
                }
            }
            break;
            
        case ENEMY:
            if (bulletMove == true){
                if (checkCollision(player)){
                    player->isActive = false;
                    bulletMove = false;
                    
                }
            }
            break;
    }
    
    CheckCollisionX(map);
    CheckCollisionY(map);
    
    
    if(!bulletMove){
        position = bulletOwner->position;
        
    }
    
    
    
    velocity.x = movement.x * speed;
    velocity.y = movement.y * speed;
    position.x += velocity.x * deltaTime;
    position.y += velocity.y * deltaTime;

    
    modelMatrix = glm::mat4(1.0f);
    
    modelMatrix = glm::translate(modelMatrix, position);
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2,0.2,1));
}

