#include "objbulkrenderer.hpp"

CompositeObjectBulkRenderer::CompositeObjectBulkRenderer(
    std::shared_ptr<Shader> initShader,
    std::vector<SubObject> initSubobjects
)
{
    shader = initShader;
    subObjects = initSubobjects;
}

void CompositeObjectBulkRenderer::addSubOjbect(SubObject subObject)
{
    subObjects.push_back(subObject);
}

unsigned int CompositeObjectBulkRenderer::getNextId()
{
    return objects.size();
}

glm::mat4 CompositeObjectBulkRenderer::collapseMatrixVector(std::vector<glm::mat4> v)
{
    glm::mat4 result = glm::mat4(1.0f);
    for (int i = v.size()-1; i >= 0; i--)
        result *= v[i];
    return result;
}
void CompositeObjectBulkRenderer::render(glm::mat4 viewProjection)
{
    // Setting shaders
    glUseProgram(shader->program);

    // Getting uniform locations for glUniform* calls
    GLuint mvp_uloc = glGetUniformLocation(shader->program, "mvp");
    GLuint ambient_uloc = glGetUniformLocation(shader->program, "material_ambient");
    GLuint diffuse_uloc = glGetUniformLocation(shader->program, "material_diffuse");
    GLuint specular_uloc = glGetUniformLocation(shader->program, "material_specular");
    GLuint hasDiffuseMap_uloc = glGetUniformLocation(shader->program, "hasDiffuseMap");

    for (size_t i = 0; i < subObjects.size(); i++) {
        for (const Mesh& mesh : subObjects[i].meshes) {
            // Setting vertices and indices
            glBindVertexArray(mesh.vao);
            if (mesh.material.hasDiffuseMap) {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, mesh.material.diffuseMap->id);
            }
            for (const CompositeObjectData& object : objects) {
                if (object.shouldDraw) {
                    std::vector<glm::mat4> modelMatrices;
                    int modelIndex = i;
                    while (modelIndex != -1) {
                        modelMatrices.push_back(object.matrixStack[modelIndex]);
                        modelIndex = subObjects[modelIndex].parentMesh;
                    }
                    glm::mat4 mvp = viewProjection * collapseMatrixVector(modelMatrices);
                    glUniformMatrix4fv(mvp_uloc, 1, GL_FALSE, &mvp[0][0]);
                    // Drawing!
                    glDrawElements(GL_TRIANGLES, mesh.numIndices, GL_UNSIGNED_INT, nullptr);
                }
            }
        }
    }
}

CompositeObjectBulkRenderable::CompositeObjectBulkRenderable(std::shared_ptr<CompositeObjectBulkRenderer> initParent)
{
    parent = initParent;
    id = parent->getNextId();
    std::vector<glm::mat4> matrixStack;
    for (size_t i = 0; i < parent->subObjects.size(); i++)
        matrixStack.push_back(glm::mat4(1.0f));
    parent->objects.push_back({
        true,
        matrixStack
    });
}

void CompositeObjectBulkRenderable::shouldDraw(bool val)
{
    parent->objects[id].shouldDraw = val;
}

void CompositeObjectBulkRenderable::translate(glm::vec3 translation) {
    translate(0, translation);
}

void CompositeObjectBulkRenderable::rotate(float amount, glm::vec3 axis)
{
    rotate(0, amount, axis);
}

void CompositeObjectBulkRenderable::scale(glm::vec3 s)
{
    scale(0, s);
}

void CompositeObjectBulkRenderable::translate(int modelIndex, glm::vec3 translation)
{
    parent->objects[id].matrixStack[modelIndex] = glm::translate(parent->objects[id].matrixStack[modelIndex], translation);
}

void CompositeObjectBulkRenderable::rotate(int modelIndex, float amount, glm::vec3 axis)
{
    parent->objects[id].matrixStack[modelIndex] = glm::rotate(parent->objects[id].matrixStack[modelIndex], amount, axis);
}

void CompositeObjectBulkRenderable::scale(int modelIndex, glm::vec3 scale)
{

    parent->objects[id].matrixStack[modelIndex] = glm::scale(parent->objects[id].matrixStack[modelIndex], scale);
}

void CompositeObjectBulkRenderable::setModelMatrix(int modelIndex, glm::mat4 mat)
{
    parent->objects[id].matrixStack[modelIndex] = mat;
}

void CompositeObjectBulkRenderable::setModelMatrix(int modelIndex, glm::vec3 translation, float angle, glm::vec3 rotationAxis, glm::vec3 scale)
{
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::scale(model, scale);
    model = glm::rotate(model, angle, rotationAxis);
    model = glm::translate(model, translation);
    parent->objects[id].matrixStack[modelIndex] = model;
}
