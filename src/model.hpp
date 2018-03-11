#pragma once
#include <vector>
#include <map>
#include <utility>
#include "objloader.hpp"

// glfw
#define NOMINMAX
#include <gl3w.h>
#include <GLFW/glfw3.h>

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

namespace Model {
    enum class MeshType {
        SAND_1,
        SAND_2,
        SAND_3,
        WALL,
        BRICK_CUBE,
        MINING_TOWER,
        PHOTON_TOWER,
        TREE,
        GUN_TURRET,
        BALL
    };

    extern std::vector<std::pair<Model::MeshType, std::vector<SubObjectSource>>> meshSources;
}