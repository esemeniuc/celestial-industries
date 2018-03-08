#include "renderer.hpp"

Renderer::Renderer(
    std::shared_ptr<Shader> initShader,
    std::vector<SubObjectSource> subObjectSources
)
{
    shader = initShader;
    for (auto source : subObjectSources) {
        subObjects.push_back(loadSubObject(source));
    }

    instanceDataAttribute = glGetUniformBlockIndex(shader->program, "InstancesData");

    glGenBuffers(1, &instancesDataBuffer);
    glBindBuffer(GL_UNIFORM_BUFFER, instancesDataBuffer);
    glBindBufferBase(GL_UNIFORM_BUFFER, 2, instancesDataBuffer);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(ShaderInstancesData), NULL, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    instancesData.stride = subObjects.size();

}

SubObject Renderer::loadSubObject(SubObjectSource source)
{
    // Clearing errors
    gl_flush_errors();

    // Getting uniform locations for glUniform* calls
    viewProjectionUniform = glGetUniformLocation(shader->program, "vp");
    modelIndexUniform = glGetUniformLocation(shader->program, "modelIndex");
    materialUniformBlock = glGetUniformBlockIndex(shader->program, "MaterialInfo");

    // Getting attribute locations
    positionAttribute = glGetAttribLocation(shader->program, "in_position");
    texcoordAttribute = glGetAttribLocation(shader->program, "in_texcoord");
    normalAttribute = glGetAttribLocation(shader->program, "in_normal");


    OBJ::Data obj;
    std::string path = pathBuilder({ "data", "models" });
    if (!OBJ::Loader::loadOBJ(path, source.filename, obj)) {
        // Failure message should already be handled by loadOBJ
        throw "Failed to load subobject";
    }
    GLuint vbo_id;
    // Vertex Buffer creation
    glGenBuffers(1, &vbo_id);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
    glBufferData(GL_ARRAY_BUFFER, obj.data.size() * sizeof(OBJ::VertexData), obj.data.data(), GL_STATIC_DRAW);

    auto meshes = std::make_shared<std::vector<Mesh>>();
    for (auto group : obj.groups) {
        Mesh mesh;
        // Vertex Array (Container for Vertex + Index buffer)
        glGenVertexArrays(1, &mesh.vao);
        glBindVertexArray(mesh.vao);

        glBindBuffer(GL_ARRAY_BUFFER, vbo_id);

        mesh.vbo = vbo_id;
        // Index Buffer creation
        glGenBuffers(1, &mesh.ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, group.indices.size() * sizeof(unsigned int), group.indices.data(), GL_STATIC_DRAW);

        if (gl_has_errors())
            throw "Encountered GL error while making subobject";

        mesh.numIndices = group.indices.size();
        mesh.material = group.material;

        // Uniform block
        glGenBuffers(1, &mesh.ubo);
        glBindBuffer(GL_UNIFORM_BUFFER, mesh.ubo);

        ShaderMaterialData material = {
            glm::vec4(group.material.ambient, 1.0),
            glm::vec4(group.material.diffuse, 1.0),
            glm::vec4(group.material.specular, 1.0),
            group.material.hasDiffuseMap,
            false,
            false,
            false
        };

        glBindBufferBase(GL_UNIFORM_BUFFER, 1, mesh.ubo);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(ShaderMaterialData), &material, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        // Input data location as in the vertex buffer
        glEnableVertexAttribArray(positionAttribute);
        glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(OBJ::VertexData), (void*)0); // (void*)0 because we render from the start of the stride

        glEnableVertexAttribArray(texcoordAttribute);
        glVertexAttribPointer(texcoordAttribute, 2, GL_FLOAT, GL_FALSE, sizeof(OBJ::VertexData), (void*)sizeof(glm::vec3));

        glEnableVertexAttribArray(normalAttribute);
        glVertexAttribPointer(normalAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(OBJ::VertexData), (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));

        meshes->push_back(mesh);
    }
    return {
        meshes,
        source.parentMesh
    };
}

unsigned int Renderer::getNextId()
{
    if (instances.size() + 1 > maxInstances / subObjects.size()) {
        throw "Too many instances! Increase maxInstances if you want to do this";
    }
    return instances.size();
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

    glUniformMatrix4fv(viewProjectionUniform, 1, GL_FALSE, &viewProjection[0][0]);

    glBindBuffer(GL_UNIFORM_BUFFER, instancesDataBuffer);
    glUniformBlockBinding(shader->program, instanceDataAttribute, 2); // layout hardcoded in shader
    glBindBufferBase(GL_UNIFORM_BUFFER, 2, instancesDataBuffer);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(ShaderInstancesData), &instancesData, GL_DYNAMIC_DRAW);
    //glBindBuffer(GL_UNIFORM_BUFFER, 0);

    for (size_t i = 0; i < subObjects.size(); i++) {
        for (const Mesh& mesh : subObjects[i].meshes) {
            // Setting vertices and indices
            glBindVertexArray(mesh.vao);

            glBindBuffer(GL_UNIFORM_BUFFER, mesh.ubo);
            glUniformBlockBinding(shader->program, materialUniformBlock, 1); // layout hardcoded in shader
            glBindBufferBase(GL_UNIFORM_BUFFER, 1, mesh.ubo);

            glUniform1i(modelIndexUniform, i);

            if (mesh.material.hasDiffuseMap) {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, mesh.material.diffuseMap->id);
            }

            glDrawElementsInstanced(GL_TRIANGLES, mesh.numIndices, GL_UNSIGNED_INT, nullptr, instances.size());
        }
    }
}

void Renderer::updateModelMatrixStack(unsigned int instanceIndex)
{
    for (size_t i = 0; i < subObjects.size(); i++) {
        std::vector<glm::mat4> modelMatrices;
        int modelIndex = i;
        while (modelIndex != -1) {
            modelMatrices.push_back(instances[instanceIndex].matrixStack[modelIndex]);
            modelIndex = subObjects[modelIndex].parentMesh;
        }
        instancesData.modelMatrices[instanceIndex] = collapseMatrixVector(modelMatrices);
    }
}

Renderable::Renderable(std::shared_ptr<Renderer> initParent)
{
    parent = initParent;
    id = parent->getNextId();
    std::vector<glm::mat4> matrixStack;
    std::vector<glm::mat4> computedModelMatrices;
    for (size_t i = 0; i < parent->subObjects.size(); i++) {
        matrixStack.push_back(glm::mat4(1.0f));
    }
    parent->instances.push_back({
        true,
        matrixStack,
    });
}

void Renderable::shouldDraw(bool val)
{
    parent->instances[id].shouldDraw = val;
}

void Renderable::translate(glm::vec3 translation) {
    translate(0, translation);
    parent->updateModelMatrixStack(id);
}

void Renderable::rotate(float amount, glm::vec3 axis)
{
    rotate(0, amount, axis);
    parent->updateModelMatrixStack(id);
}

void Renderable::scale(glm::vec3 s)
{
    scale(0, s);
    parent->updateModelMatrixStack(id);
}

void Renderable::translate(int modelIndex, glm::vec3 translation)
{
    parent->instances[id].matrixStack[modelIndex] = glm::translate(parent->instances[id].matrixStack[modelIndex], translation);
    parent->updateModelMatrixStack(id);
}

void Renderable::rotate(int modelIndex, float amount, glm::vec3 axis)
{
    parent->instances[id].matrixStack[modelIndex] = glm::rotate(parent->instances[id].matrixStack[modelIndex], amount, axis);
    parent->updateModelMatrixStack(id);
}

void Renderable::scale(int modelIndex, glm::vec3 scale)
{

    parent->instances[id].matrixStack[modelIndex] = glm::scale(parent->instances[id].matrixStack[modelIndex], scale);
    parent->updateModelMatrixStack(id);
}

void Renderable::setModelMatrix(int modelIndex, glm::mat4 mat)
{
    parent->instances[id].matrixStack[modelIndex] = mat;
    parent->updateModelMatrixStack(id);
}

void Renderable::setModelMatrix(int modelIndex, glm::vec3 translation, float angle, glm::vec3 rotationAxis, glm::vec3 scale)
{
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::scale(model, scale);
    model = glm::rotate(model, angle, rotationAxis);
    model = glm::translate(model, translation);
    parent->instances[id].matrixStack[modelIndex] = model;
    parent->updateModelMatrixStack(id);
}
