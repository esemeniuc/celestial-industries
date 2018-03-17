#pragma once
#include "common.hpp"
#include "shader.hpp"

// A Mesh is a collection of a VertexBuffer and an IndexBuffer. A VAO
// represents a Vertex Array Object and is the container for 1 or more Vertex Buffers and 
// an Index Buffer
struct Mesh
{
    GLuint vao;
    GLuint vbo;
    GLuint ibo;
    GLuint ubo;
    GLuint numIndices;
    OBJ::Material material;
};

struct SubObject {
    std::shared_ptr<std::vector<Mesh>> meshes;
    int parentMesh;
};

struct RenderableInstanceData {
    bool shouldDraw;
    std::vector<glm::mat4> matrixStack;
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
    void updateModelMatrixStack(unsigned int modelIndex, bool updateHierarchically=true);
    glm::mat4 getModelMatrix(unsigned int id, unsigned int modelIndex);
private:
    // TODO: replace with uniform buffers
    GLuint viewProjectionUniform, modelIndexUniform, instanceDataAttribute, materialUniformBlock, positionAttribute, texcoordAttribute, normalAttribute, instancesDataBuffer;
    static const unsigned int maxInstances = Config::MAX_TOTAL_SUBOBJECTS_PER_RENDERER;

    struct ShaderInstancesData {
        unsigned int stride;
        float padding1;
        float padding2;
        float padding3;
        glm::mat4 modelMatrices[maxInstances];
    };

    struct ShaderMaterialData {
        glm::vec4 ambient;
        glm::vec4 diffuse;
        glm::vec4 specular;
        bool hasDiffuseMap;
        bool padding1;  // Padding is needed because std140 dictates everything is in steps of 4 bytes. I beleive it will actually allow us to
        bool padding2;  // use these padding bools, but if thewy arent there we're going to read garbage on those that are :)
        bool padding3;
    };

    ShaderInstancesData instancesData;
};

class Renderable {
private:

public:
    std::shared_ptr<Renderer> parent;
    unsigned int id;
    Renderable(std::shared_ptr<Renderer> initParent);
    void shouldDraw(bool val);

    void setModelMatricesFromComputed();
    /*
    Model index is the index of the model matrix to be updated. This is generally 0 as most models are made of 1 object and only have
    1 model matrix. It may also be 0 because the first model matrix is usually the root of a renderable made of many objects. If you want
    to modify the model matrix of the 3rd element of your renderable (and all it's children in turn) you specify 2 (0 indexing).
    */
    void translate(int modelIndex, glm::vec3 translation, bool updateHeirarchically = true);
    /*
    Model index is the index of the model matrix to be updated. This is generally 0 as most models are made of 1 object and only have
    1 model matrix. It may also be 0 because the first model matrix is usually the root of a renderable made of many objects. If you want
    to modify the model matrix of the 3rd element of your renderable (and all it's children in turn) you specify 2 (0 indexing).
    */
    void rotate(int modelIndex, float amount, glm::vec3 axis, bool updateHeirarchically = true);
    /*
    Model index is the index of the model matrix to be updated. This is generally 0 as most models are made of 1 object and only have
    1 model matrix. It may also be 0 because the first model matrix is usually the root of a renderable made of many objects. If you want
    to modify the model matrix of the 3rd element of your renderable (and all it's children in turn) you specify 2 (0 indexing).
    */
    void scale(int modelIndex, glm::vec3 scale, bool updateHeirarchically = true);
    /*
    Model index is the index of the model matrix to be updated. This is generally 0 as most models are made of 1 object and only have
    1 model matrix. It may also be 0 because the first model matrix is usually the root of a renderable made of many objects. If you want
    to modify the model matrix of the 3rd element of your renderable (and all it's children in turn) you specify 2 (0 indexing).
    */
    void setModelMatrix(int modelIndex, glm::mat4 mat, bool updateHeirarchically = true);
    /*
    Model index is the index of the model matrix to be updated. This is generally 0 as most models are made of 1 object and only have
    1 model matrix. It may also be 0 because the first model matrix is usually the root of a renderable made of many objects. If you want
    to modify the model matrix of the 3rd element of your renderable (and all it's children in turn) you specify 2 (0 indexing).
    */
    void setModelMatrix(int modelIndex, glm::vec3 translation = { 0,0,0 }, float angle = 0, glm::vec3 rotationAxis = { 0,1,0 }, glm::vec3 scale = { 1,1,1 }, bool updateHeirarchically = true);

    // When subobject modelIndex is not provided it is assumed you wish to apply the transformation to the whole model
    void translate(glm::vec3 translation, bool updateHeirarchically = true);
    void rotate(float amount, glm::vec3 axis, bool updateHeirarchically = true);
    void scale(glm::vec3 scale, bool updateHeirarchically = true);

    virtual ~Renderable() {};
};