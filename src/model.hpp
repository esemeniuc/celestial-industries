#pragma once
#include <vector>
#include <utility>
#include "renderer.hpp"
#include "collisiondetector.hpp"

// glfw
#define NOMINMAX
#include <gl3w.h>
#include <GLFW/glfw3.h>

namespace Model {

		enum MeshType { //avoid enum class to avoid casting to integers
			SAND_1,
			SAND_2,
			SAND_3,
			SAND_4,
			SAND_5,
			WALL,
			BRICK_CUBE,
			MINING_TOWER,
			PHOTON_TOWER,
			TREE,
			YELLOWTREE,
			REDTREE,
			BALL,
			GUN_TURRET,
			GEYSER,
			PARTICLE,
			TILE_CURSOR,
			ENEMY_SPIKE_UNIT,
			ENEMY_RANGED_LINE_UNIT,
			ENEMY_RANGED_RADIUS_UNIT,
			FRIENDLY_FIRE_UNIT,
			FRIENDLY_RANGED_UNIT,
			WATER,
			GRASS,
			HROAD,
			VROAD,
		};

    extern std::vector<std::pair<Model::MeshType, std::vector<SubObjectSource>>> meshSources;
    extern std::vector<std::shared_ptr<Renderer>>meshRenderers;
    extern CollisionDetector collisionDetector;
    Renderable createRenderable(MeshType type);
}
