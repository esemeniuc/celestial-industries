#pragma once

#include <vector>
#include <utility>
#include "renderer.hpp"
#include "collisiondetector.hpp"

namespace Model {

	enum MeshType { //avoid enum class to avoid casting to integers
		NONE = -1, //for pathfinding.cpp, it needs a meshtype in aStarNode

		//level tile textures
		SAND_1,
		SAND_2,
		SAND_3,
		SAND_4,
		SAND_5,
		WATER,
		GRASS,
		HROAD,
		VROAD,

		//resource tile
		GEYSER,

		//level props
		WALL,
		BRICK_CUBE,
		TREE,
		YELLOWTREE,
		REDTREE,

		//buildable things
		MINING_TOWER,
		PHOTON_TOWER,
		SUPPLY_DEPOT,
		REFINERY,
		GUN_TURRET,
		FACTORY,
		COMMAND_CENTER,

		//movable units
		BALL,
		ENEMY_SPIKE_UNIT,
		ENEMY_RANGED_LINE_UNIT,
		ENEMY_RANGED_RADIUS_UNIT,
		FRIENDLY_FIRE_UNIT,
		FRIENDLY_RANGED_UNIT,

		//other
		TILE_CURSOR,

		MESHTYPES_COUNT //see https://stackoverflow.com/a/2102615
	};

	extern std::vector<std::pair<Model::MeshType, std::vector<SubObjectSource>>> meshSources;
	extern std::vector<std::shared_ptr<Renderer>> meshRenderers;
	extern CollisionDetector collisionDetector;

	Renderable createRenderable(MeshType type);

}
