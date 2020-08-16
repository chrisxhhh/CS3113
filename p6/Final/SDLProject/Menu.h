#include "Scene.h"

class Menu : public Scene {
public:
    glm::mat4 modelMatrix;
    void Initialize() override;
    void Update(float deltaTime) override;
    void Render(ShaderProgram *program) override;
};

