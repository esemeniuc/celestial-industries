//
// Created by eric on 2/24/18.
//

//#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
#include "unitcomp.hpp"
#include "world.hpp"
#include "global.hpp"

TEST_CASE("Unit range is accurate", "[generic_unit]") {
	world.init();
//	world.initMeshTypes(Model::meshSources);
//	std::shared_ptr<Renderer> parent = world.meshRenderers[Model::MeshType::SAND_2];

	std::shared_ptr<Entity> genericUnit1 = std::make_shared<Entity>();
	std::shared_ptr<Entity> genericUnit2 = std::make_shared<Entity>();
	
	std::cout << genericUnit2->getPosition().x << '\n';
	std::cout << genericUnit2->getPosition().y << '\n';
	std::cout << genericUnit2->getPosition().z << '\n';
	genericUnit1->translate({1.0, 0.0, 0.0});
	genericUnit2->translate({3.0, 3.0, 0.0});
	std::cout << genericUnit2->getPosition().x << '\n';
	std::cout << genericUnit2->getPosition().y << '\n';
	std::cout << genericUnit2->getPosition().z << '\n';

	REQUIRE(genericUnit1->canSee(genericUnit2));
	REQUIRE(genericUnit1->inAttackRange(genericUnit2));
	genericUnit2->translate({10.0, 10.0, 0.0});

	REQUIRE(!genericUnit1->canSee(genericUnit2));
	REQUIRE(!genericUnit1->inAttackRange(genericUnit2));

}