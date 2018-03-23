#include "model.hpp"

namespace Model {
    std::vector<std::pair<Model::MeshType, std::vector<SubObjectSource>>> meshSources = {
        { Model::MeshType::SAND_1,      { { "sand1.obj",       -1 } } },
        { Model::MeshType::SAND_2,      { { "sand1.obj",       -1 } } },
        { Model::MeshType::SAND_3,      { { "sand1.obj",       -1 } } },
		{ Model::MeshType::SAND_4,		{ { "sand1.obj",       -1 } } },
		{ Model::MeshType::SAND_5,		{ { "sand1.obj",       -1 } } },
        { Model::MeshType::WALL,        { { "wall.obj",        -1 } } },
        { Model::MeshType::BRICK_CUBE,  { { "brickCube.obj",   -1 } } },
        { Model::MeshType::MINING_TOWER,{ { "miningTower.obj", -1 } } },
        { Model::MeshType::PHOTON_TOWER,{ { "photonTower.obj", -1 } } },
        { Model::MeshType::TREE,        { { "Tree1.obj",       -1 } } },
		{ Model::MeshType::YELLOWTREE,	{ { "Tree2.obj",       -1 } } },
		{ Model::MeshType::REDTREE,		{ { "Tree3.obj",       -1 } } },
        { Model::MeshType::BALL,        { { "ball.obj",        -1 } } },
        { Model::MeshType::GUN_TURRET,  { { "TurretBase.obj",  -1 },{ "TurretTop.obj", 0 },{ "TurretGunsLeft.obj", 1 },{ "TurretGunsRight.obj", 1 } } },
        { Model::MeshType::GEYSER,      { { "sand1.obj",       -1 } } },
		{ Model::MeshType::WATER,		{ { "Water1.obj",      -1 } } },
		{ Model::MeshType::GRASS,		{ { "grass1.obj",      -1 } } },
		{ Model::MeshType::HROAD,		{ { "Road2.obj",       -1 } } },
		{ Model::MeshType::VROAD,		{ { "Road1.obj",       -1 } } },
    };
	
    std::vector<std::shared_ptr<Renderer>> meshRenderers(meshSources.size());
    CollisionDetector collisionDetector;
    
    Renderable createRenderable(MeshType type)
    {
        return Renderable(meshRenderers[type]);
    }
}
