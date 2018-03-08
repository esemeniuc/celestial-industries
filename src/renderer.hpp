#pragma once
#include "common.hpp"
#include "model.hpp"

class Renderer {
    std::shared_ptr<Shader> shader;
    glm::mat4 collapseMatrixVector(std::vector<glm::mat4> v);
public:
    std::vector<SubObject> subObjects;
    std::vector<RenderableInstanceData> objects;
    Renderer(
        std::shared_ptr<Shader> initShader,
        std::vector<SubObject> initSubobjects
    );
    void addSubOjbect(SubObject subObject);
    unsigned int getNextId();
    void render(glm::mat4 viewProjection);
};

class Renderable {
private:
    std::shared_ptr<Renderer> parent;
    unsigned int id;
public:
    Renderable(std::shared_ptr<Renderer> initParent);
    void shouldDraw(bool val);

    void translate(int modelIndex, glm::vec3 translation);
    void rotate(int modelIndex, float amount, glm::vec3 axis);
    void scale(int modelIndex, glm::vec3 scale);
    void setModelMatrix(int modelIndex, glm::mat4 mat);
    void setModelMatrix(int modelIndex, glm::vec3 translation = { 0,0,0 }, float angle = 0, glm::vec3 rotationAxis = { 0,1,0 }, glm::vec3 scale = { 1,1,1 });

    // When subobject modelIndex is not provided it is assumed you wish to apply the transformation to the whole model
    void translate(glm::vec3 translation);
    void rotate(float amount, glm::vec3 axis);
    void scale(glm::vec3 scale);

    virtual ~Renderable() {};
};