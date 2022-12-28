#include "RubiksCube.h"
#include <set>

void RubiksCube::AddToWall(WallType type, int index)
{
	for (auto cube : Cubes) {
		if (cube.first == index) {
			cube.second.insert(type);
			return;
		}
	}
}

void RubiksCube::RemoveFromWall(WallType type, int index)
{
	for (auto cube : Cubes) {
		if (cube.first == index) {
			cube.second.erase(type);
			return;
		}
	}

}

std::vector<int> RubiksCube::GetWall(WallType type)
{
	std::vector<int> wall = std::vector<int>();
	for (auto cube : Cubes) {
		if (cube.second.find(type) != cube.second.end()) {
			wall.push_back(cube.first);
		}
	}
	return wall;
}

void RubiksCube::RotateWall(WallType type)
{
	std::vector<int> wall = GetWall(type);
	for (auto index : wall) {
		auto cube = Cubes[index];
		if (cube) {

		}
	}
}

void RubiksCube::ReplaceType(WallType currentType, WallType rotationType) {
	switch (rotationType) {
	case (Right):
		
		break;
	case (Left):
		break;
	case (Up):
		break;
	case (Down):
		break;
	case (Back):
		break;
	case (Front):
		break;
	}
}
