//
// Created by eric on 2/24/18.
//

//#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
#include "genericunit.hpp"
//float inRange(const Entity& entity) {
//	return glm::length(glm::vec2(entity.getPosition() - this->getPosition()));
//}

TEST_CASE( "Unit range is accurate", "[factorial]" ) {
	const std::shared_ptr<Renderer> initParent;
//	Entity entity(initParent);
	GenericUnit genericUnit;
//	entity.translate();



//REQUIRE( inRange(Entity) == 1 );
//REQUIRE( Factorial(2) == 2 );
//REQUIRE( Factorial(3) == 6 );
//REQUIRE( Factorial(10) == 3628800 );
}