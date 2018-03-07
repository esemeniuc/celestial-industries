#pragma once
#include "common.hpp"

struct SubObject {
    std::shared_ptr<std::vector<Mesh>> meshes;
    int parentMesh;
};

struct ShaderData {
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    bool hasDiffuseMap;
};

struct RenderableInstanceData {
    bool shouldDraw;
    std::vector<glm::mat4> matrixStack; // Must be of identical length as subobject's meshes. enforced in constructor.
};

struct SubObjectSource {
    std::string filename;
    int parentMesh;
};

class Renderer {
    std::shared_ptr<Shader> shader;
    glm::mat4 collapseMatrixVector(std::vector<glm::mat4> v);
public:
    std::vector<SubObject> subObjects;
    std::vector<RenderableInstanceData> instances;
    Renderer(
        std::shared_ptr<Shader> initShader,
        std::vector<SubObjectSource> subObjectSources
    );
    SubObject loadSubObject(SubObjectSource source);
    unsigned int getNextId();
    void render(glm::mat4 viewProjection);
private:
    // TODO: replace with uniform buffers
    GLuint mvpUniform, ambientUniform, diffuseUniform, specularUniform,
        hasDiffusemapUniform, positionAttribute, texcoordAttribute,
        normalAttribute;
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