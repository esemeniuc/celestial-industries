#include "models.hpp"

namespace Models {
    std::vector<std::pair<Models::MeshType, std::vector<SubObjectSource>>> meshSources = {
        { Models::MeshType::SAND_1,      { { "sand1.obj",       -1 } } },
        { Models::MeshType::SAND_2,      { { "sand2.obj",       -1 } } },
        { Models::MeshType::SAND_3,      { { "sand3.obj",       -1 } } },
        { Models::MeshType::WALL,        { { "wall.obj",        -1 } } },
        { Models::MeshType::BRICK_CUBE,  { { "brickCube.obj",   -1 } } },
        { Models::MeshType::MINING_TOWER,{ { "miningTower.obj", -1 } } },
        { Models::MeshType::PHOTON_TOWER,{ { "photonTower.obj", -1 } } },
        { Models::MeshType::TREE,        { { "treeTile1.obj",   -1 } } },
        { Models::MeshType::BALL,        { { "ball.obj",        -1 } } },
        { Models::MeshType::GUN_TURRET,  { { "TurretBase.obj",  -1 },{ "TurretTop.obj", 0 },{ "TurretGunsLeft.obj", 1 },{ "TurretGunsRight.obj", 1 } } },
    };
}