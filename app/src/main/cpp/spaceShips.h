#pragma once
#include "base.h"

enum class SpaceShipType {
	FIGHTER,
	RIPPER,
	SCOUT
};

inline const std::vector<cpVect> fighterVertices = { {40, 0}, { -10, -20 }, {-10, 20}, };
inline const std::vector<cpVect> ripperVertices = { {60, 0}, {-10, -30}, {-10, 30} };
inline const std::vector<cpVect> scoutVertices = { {100, 0}, {60, -50}, {-10, -50}, {-10, 50}, {60, 50} };

inline const std::vector<cpVect> asteroidVertices = { {50, 50}, {50, -50}, {-50, -50}, {-50, 50} };

inline float GetShipSize(SpaceShipType type) {
	switch (type) {
	case SpaceShipType::FIGHTER:
		return 100;
	case SpaceShipType::RIPPER:
		return 100;
	case SpaceShipType::SCOUT:
		return 100;
	default:
		return 100;
	}
}