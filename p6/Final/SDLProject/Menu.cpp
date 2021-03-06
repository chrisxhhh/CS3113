#include "Menu.h"
#define LEVEL1_WIDTH 0
#define LEVEL1_HEIGHT 0


unsigned int menu_data[] = {
//    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
//    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
//    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
//    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
//    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
//    3, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 3,
//    3, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 3,
//    3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3
};

void Menu::Initialize() {
    
    state.nextScene = -1;
    state.inMenu = true;
    
    GLuint mapTextureID = Util::LoadTexture("tile_set.png");
    state.map = new Map(LEVEL1_WIDTH, LEVEL1_HEIGHT, menu_data, mapTextureID, 1.0f, 3, 1);
    
    state.background = new Entity();
    GLuint backgroundID = Util::LoadTexture("tankBackground.png");
    state.background->textureIDs[0] = &backgroundID;
    state.background->textureIndex = 0;
    state.background->entityType = BACKGROUND;
    state.background->position = glm::vec3(0,0,0);
    state.background->Update(0, NULL, NULL, 0, state.map, NULL, 0);
    //GLuint mapTextureID = Util::LoadTexture("tileset1.png");
    //state.map = new Map(LEVEL1_WIDTH, LEVEL1_HEIGHT, level1_data, mapTextureID, 1.0f, 4, 1);
    // Initialize Game Objects
    
    state.player = new Entity();
    state.player->entityType = PLAYER;
    
    
    //state->player->Update(0, NULL, NULL, 0, state->map, NULL);
    //state.live = new Entity[state->lives];
    
    
    
}
void Menu::Update(float deltaTime) {
    
}


void Menu::Render(ShaderProgram *program) {
    //state.background->Render(program);
    
    
    //render the background
    modelMatrix = glm::mat4(1.0f);
    program->SetModelMatrix(modelMatrix);
    
    float vertices[]  = { -10, -7.5, 10, -7.5, 10, 7.5, -10, -7.5, 10, 7.5, -10, 7.5 };
    //float vertices[]  = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
    
    glBindTexture(GL_TEXTURE_2D, state.background->textureID);
    
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);
    
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
    
    
    
    GLuint textureID = Util::LoadTexture("font1.png");
    Util::DrawText(program, textureID, "World", 0.8f, -0.4, glm::vec3(1, -2, 0));
    Util::DrawText(program, textureID, "of", 0.8f, -0.4, glm::vec3(4, -2, 0));
    Util::DrawText(program, textureID, "Tank", 0.8f, -0.25, glm::vec3(6, -2, 0));
    Util::DrawText(program, textureID, "FLAT", 0.8f, -0.25, glm::vec3(4, -3, 0));
    Util::DrawText(program, textureID, "Enter --> START", 0.5f, -0.25, glm::vec3(2, -4, 0));
    Util::DrawText(program, textureID, "SPACE --> Shoot", 0.5f, -0.25, glm::vec3(2, -4.5, 0));
    Util::DrawText(program, textureID, "Kill All", 0.5f, -0.25, glm::vec3(2, -5, 0));
    Util::DrawText(program, textureID, "and", 0.5f, -0.25, glm::vec3(2, -5.5, 0));
    Util::DrawText(program, textureID, "Collect Keys", 0.5f, -0.25, glm::vec3(2, -6, 0));
}
