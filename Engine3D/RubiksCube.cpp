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
	switch (type) 
	{
	case (Right):
		for (auto index : wall) {
			if (cube.second.find(type) != cube.second.end()) {
				wall.push_back(cube.first);
			}
		}
		break;
	}
}
