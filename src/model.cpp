#include "model.hpp"



namespace Model {
	std::vector<std::pair<Model::MeshType, std::vector<SubObjectSource>>> meshSources = {
		//level tile textures
		{ Model::MeshType::SAND_1,					{ { "sand1.obj",		-1 } } },
		{ Model::MeshType::SAND_2,					{ { "sand1.obj",		-1 } } },
		{ Model::MeshType::SAND_3,					{ { "sand1.obj",		-1 } } },
		{ Model::MeshType::SAND_4,					{ { "sand1.obj",		-1 } } },
		{ Model::MeshType::SAND_5,					{ { "sand1.obj",		-1 } } },
		{ Model::MeshType::WATER,					{ { "Water1.obj",		-1 } } },
		{ Model::MeshType::GRASS,					{ { "grass1.obj",		-1 } } },
		{ Model::MeshType::HROAD,					{ { "Road2.obj",		-1 } } },
		{ Model::MeshType::VROAD,					{ { "Road1.obj",		-1 } } },

		//resource tile
		{ Model::MeshType::GEYSER,					{ { "sand1.obj",		-1 } } },

		//level props
		{ Model::MeshType::WALL,					{ { "wall.obj",			-1 } } },
		{ Model::MeshType::BRICK_CUBE,				{ { "brickCube.obj",	-1 } } },
		{ Model::MeshType::TREE,					{ { "Tree1.obj",		-1 } } },
		{ Model::MeshType::YELLOWTREE,				{ { "Tree2.obj",		-1 } } },
		{ Model::MeshType::REDTREE,					{ { "Tree3.obj",		-1 } } },
		{ Model::MeshType::ENEMY_PORTAL,			{ { "portal.obj",		-1 } } },

		//buildable things
		{ Model::MeshType::MINING_TOWER,			{ { "miningTower.obj",	-1 } } },
		{ Model::MeshType::SUPPLY_DEPOT,			{ { "supply_depot.obj",	-1 } } },
		{ Model::MeshType::REFINERY,				{ { "Refinery.obj",		-1 } } },
		{ Model::MeshType::PHOTON_TOWER,			{ { "photonTower.obj",	-1 } } },
		{ Model::MeshType::GUN_TURRET,				{ { "TurretBase.obj",	-1 },{ "TurretTop.obj", 0 },{ "TurretGunsLeft.obj", 1 },{ "TurretGunsRight.obj", 1 } } },
		{ Model::MeshType::FACTORY,					{ { "factory.obj",						-1 } } },
		{ Model::MeshType::COMMAND_CENTER,			{ { "command_center.obj",				-1 } } },

		//movable units
		{ Model::MeshType::BALL,					{ { "ball.obj",							-1 } } },
		{ Model::MeshType::ENEMY_SPIKE_UNIT,		{ { "enemySpikeUnit.obj",				-1 } } },
		{ Model::MeshType::ENEMY_RANGED_LINE_UNIT,	{ { "enemyRangedLineUnit.obj",			-1 } } },
		{ Model::MeshType::ENEMY_RANGED_RADIUS_UNIT,{ { "enemyRangeRadiusUnitBase.obj",		-1 },{ "enemyRangeRadiusUnitTop.obj", 0} } },
		{ Model::MeshType::FRIENDLY_FIRE_UNIT,		{ { "friendlyFireUnit.obj",				-1 } } },
		{ Model::MeshType::FRIENDLY_RANGED_UNIT,	{ { "friendlyRangedUnitBase.obj",		-1 },{ "friendlyRangedUnitPillar.obj", 0 } ,{ "friendlyRangedUnitTop.obj", 1 } } },

		// weapons

		{ Model::MeshType::BEAM,					{ { "beam.obj",							-1 } } },
		{ Model::MeshType::BULLET,					{ { "bullet.obj",						-1 } } },

		//other
		{ Model::MeshType::TILE_CURSOR,				{ { "tileSelector.obj",					-1 } } },


	};


	std::vector<std::shared_ptr<Renderer>> meshRenderers(Model::MeshType::MESHTYPES_COUNT);
	CollisionDetector collisionDetector;

	Renderable createRenderable(MeshType type)
	{
		return Renderable(meshRenderers[type]);
	}
}
