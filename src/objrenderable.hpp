#pragma once
#include "common.hpp"

class OBJBulkRenderer {
    std::shared_ptr<Shader> shader;
    std::shared_ptr<std::vector<Mesh>> meshes;
public:
    std::vector<glm::mat4> models;
    OBJBulkRenderer(std::shared_ptr<Shader> initShader, std::shared_ptr<std::vector<Mesh>> initMeshes);
    void addToQueue(glm::mat4 modelMatrix);
    unsigned int getNextId();
    void render(glm::mat4 viewProjection);
};

class OBJRenderable : public Renderable {
public:
    bool init(std::shared_ptr<std::vector<Mesh>> initMeshes, std::shared_ptr<Shader> shader);
    void draw(glm::mat4 viewProjection)override;
    void destroy();
    void translate(glm::vec3 translation);
};

// TODO: does not support deletion of tiles. This is a future us problem and it will be handled by gravestoning.
class OBJBulkRenderable { // Note: does NOT extend Renderable. This is intentional as it is not compatible with Renderable
private:
    std::shared_ptr<OBJBulkRenderer> parent;
    unsigned int id;
public:
    OBJBulkRenderable(std::shared_ptr<OBJBulkRenderer> initParent);
    void translate(glm::vec3 translation);
};