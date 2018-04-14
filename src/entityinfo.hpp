//
// Created by eric on 4/11/18.
//

#pragma once

#include <array>
#include <unordered_map>
#include "aicomp.hpp"

//stores info that never changes
namespace EntityInfo {

	extern std::array<const char*, Model::MeshType::MESHTYPES_COUNT> nameLookupTable;
	extern std::unordered_map<GamePieceClass, const char*> gamePieceClassLookupTable;
	extern std::unordered_map<GamePieceOwner, const char*> gamePieceOwnerLookupTable;

}



