//
// Created by eric on 4/14/18.
//
#pragma once

#include <ostream> //for overloaded << operator
#include <cmath> //for round
#include "model.hpp" //for meshtype

// used to build a graph of nodes for the AI pathfinder to traverse each tile node.
struct AStarNode {
	int rowCoord, colCoord, movementCost;
	float fScore;
	Model::MeshType type;

	AStarNode() = default;

	AStarNode(int _colCoord,
			  int _rowCoord,
			  int _movementCost,
			  float _fScore,
			  Model::MeshType _type) : rowCoord(_rowCoord),
									   colCoord(_colCoord),
									   movementCost(_movementCost),
									   fScore(_fScore), //don't need to round this since its not part of the == check
									   type(_type) {}

	bool operator==(const AStarNode& rhs) const {
		return rowCoord == rhs.rowCoord &&
			   colCoord == rhs.colCoord;
	}

	bool operator!=(const AStarNode& rhs) const {
		return !(rhs == *this);
	}

	friend std::ostream& operator<<(std::ostream& os, const AStarNode& state) {
		os << "rowCoord: " << state.rowCoord << " colCoord: " << state.colCoord << " movementCost: "
		   << state.movementCost << " fScore: " << state.fScore;
		return os;
	}
};
