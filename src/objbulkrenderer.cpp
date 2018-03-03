#include "objbulkrenderer.hpp"

Renderer::Renderer(
    std::shared_ptr<Shader> initShader,
    std::vector<SubObject> initSubobjects
)
{
    shader = initShader;
    subObjects = initSubobjects;
}

void Renderer::addSubOjbect(SubObject subObject)
{
    subObjects.push_back(subObject);
}

unsigned int Renderer::getNextId()
{
    return objects.size();
}

glm::mat4 Renderer::collapseMatrixVector(std::vector<glm::mat4> v)
{
    glm::mat4 result = glm::mat4(1.0f);
    for (int i = v.size()-1; i >= 0; i--)
        result *= v[i];
    return result;
}
void Renderer::render(glm::mat4 viewProjection)
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
            glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);

            // Input data location as in the vertex buffer
            GLuint in_position_loc = glGetAttribLocation(shader->program, "in_position");
            glEnableVertexAttribArray(in_position_loc);
            glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(OBJ::VertexData), (void*)0); // (void*)0 because we render from the start of the stride

            // Diffuse map stuff
            GLint in_texcoord_loc = glGetAttribLocation(shader->program, "in_texcoord");
            glEnableVertexAttribArray(in_texcoord_loc); // Oddly enough we ALWAYS get texcoords, even for textureless stuff
            glVertexAttribPointer(in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(OBJ::VertexData), (void*)sizeof(glm::vec3));

            GLuint in_normal_loc = glGetAttribLocation(shader->program, "in_normal");
            glEnableVertexAttribArray(in_normal_loc);
            glVertexAttribPointer(in_normal_loc, 3, GL_FLOAT, GL_FALSE, sizeof(OBJ::VertexData), (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));

            if (mesh.material.hasDiffuseMap) {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, mesh.material.diffuseMap->id);
            }

            // Setting uniform values to the currently bound program
            glUniform3f(ambient_uloc, mesh.material.ambient.x, mesh.material.ambient.y, mesh.material.ambient.z);
            glUniform3f(diffuse_uloc, mesh.material.diffuse.x, mesh.material.diffuse.y, mesh.material.diffuse.z);
            glUniform3f(specular_uloc, mesh.material.specular.x, mesh.material.specular.y, mesh.material.specular.z);
            glUniform1i(hasDiffuseMap_uloc, mesh.material.hasDiffuseMap);
            for (const RenderableInstanceData& object : objects) {
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

Renderable::Renderable(std::shared_ptr<Renderer> initParent)
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

void Renderable::shouldDraw(bool val)
{
    parent->objects[id].shouldDraw = val;
}

void Renderable::translate(glm::vec3 translation) {
    translate(0, translation);
}

void Renderable::rotate(float amount, glm::vec3 axis)
{
    rotate(0, amount, axis);
}

void Renderable::scale(glm::vec3 s)
{
    scale(0, s);
}

void Renderable::translate(int modelIndex, glm::vec3 translation)
{
    parent->objects[id].matrixStack[modelIndex] = glm::translate(parent->objects[id].matrixStack[modelIndex], translation);
}

void Renderable::rotate(int modelIndex, float amount, glm::vec3 axis)
{
    parent->objects[id].matrixStack[modelIndex] = glm::rotate(parent->objects[id].matrixStack[modelIndex], amount, axis);
}

void Renderable::scale(int modelIndex, glm::vec3 scale)
{

    parent->objects[id].matrixStack[modelIndex] = glm::scale(parent->objects[id].matrixStack[modelIndex], scale);
}

void Renderable::setModelMatrix(int modelIndex, glm::mat4 mat)
{
    parent->objects[id].matrixStack[modelIndex] = mat;
}

void Renderable::setModelMatrix(int modelIndex, glm::vec3 translation, float angle, glm::vec3 rotationAxis, glm::vec3 scale)
{
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::scale(model, scale);
    model = glm::rotate(model, angle, rotationAxis);
    model = glm::translate(model, translation);
    parent->objects[id].matrixStack[modelIndex] = model;
}
