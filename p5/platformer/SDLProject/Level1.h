#include "Scene.h"

class Level1 : public Scene {
public:
    using Scene::Scene;
    void Initialize() override;
    void Update(float deltaTime) override;
    void Render(ShaderProgram *program) override;
};
