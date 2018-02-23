#include "objrenderable.hpp"

bool OBJRenderable::init(std::shared_ptr<std::vector<Mesh>> initMeshes, std::shared_ptr<Shader> initShader)
{
    meshes = initMeshes;
    shader = initShader;
    return true;
}


void OBJRenderable::draw(glm::mat4 viewProjection)
{
    glm::mat4 mvp = viewProjection * model;

    // Setting shaders
    glUseProgram(shader->program);

    // Getting uniform locations for glUniform* calls
    GLuint mvp_uloc = glGetUniformLocation(shader->program, "mvp");
    GLuint ambient_uloc = glGetUniformLocation(shader->program, "material_ambient");
    GLuint diffuse_uloc = glGetUniformLocation(shader->program, "material_diffuse");
    GLuint specular_uloc = glGetUniformLocation(shader->program, "material_specular");
    GLuint hasDiffuseMap_uloc = glGetUniformLocation(shader->program, "hasDiffuseMap");

    for (auto mesh : meshes) {
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
        glUniformMatrix4fv(mvp_uloc, 1, GL_FALSE, &mvp[0][0]);

        glUniform3f(ambient_uloc, mesh.material.ambient.x, mesh.material.ambient.y, mesh.material.ambient.z);
        glUniform3f(diffuse_uloc, mesh.material.diffuse.x, mesh.material.diffuse.y, mesh.material.diffuse.z);
        glUniform3f(specular_uloc, mesh.material.specular.x, mesh.material.specular.y, mesh.material.specular.z);
        glUniform1i(hasDiffuseMap_uloc, mesh.material.hasDiffuseMap);


        // Drawing!
        glDrawElements(GL_TRIANGLES, mesh.numIndices, GL_UNSIGNED_INT, nullptr);
    }
}

void OBJRenderable::destroy()
{
}

void OBJRenderable::translate(glm::vec3 translation)
{
    model = glm::translate(model, translation);
}

OBJBulkRenderer::OBJBulkRenderer(std::shared_ptr<Shader> initShader, std::shared_ptr<std::vector<Mesh>> initMeshes)
{
    shader = initShader;
    meshes = initMeshes;
}

void OBJBulkRenderer::addToQueue(glm::mat4 modelMatrix)
{
    models.push_back(modelMatrix);
}

unsigned int OBJBulkRenderer::getNextId()
{
    return models.size();
}

void OBJBulkRenderer::render(glm::mat4 viewProjection)
{
    // Setting shaders
    glUseProgram(shader->program);

    // Getting uniform locations for glUniform* calls
    GLuint mvp_uloc = glGetUniformLocation(shader->program, "mvp");
    GLuint ambient_uloc = glGetUniformLocation(shader->program, "material_ambient");
    GLuint diffuse_uloc = glGetUniformLocation(shader->program, "material_diffuse");
    GLuint specular_uloc = glGetUniformLocation(shader->program, "material_specular");
    GLuint hasDiffuseMap_uloc = glGetUniformLocation(shader->program, "hasDiffuseMap");

    for (auto mesh : meshes) {
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

        for (glm::mat4 model : models) {
            glm::mat4 mvp = viewProjection * model;
            glUniformMatrix4fv(mvp_uloc, 1, GL_FALSE, &mvp[0][0]);
            // Drawing!
            glDrawElements(GL_TRIANGLES, mesh.numIndices, GL_UNSIGNED_INT, nullptr);
        }
    }
}

OBJBulkRenderable::OBJBulkRenderable(std::shared_ptr<OBJBulkRenderer> initParent)
{
    parent = initParent;
    id = parent->getNextId();
    parent->models.push_back(glm::mat4(1.0f));
}

void OBJBulkRenderable::translate(glm::vec3 translation)
{
    parent->models[id] = glm::translate(parent->models[id], translation);
}
