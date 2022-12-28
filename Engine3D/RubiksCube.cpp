#include "RubiksCube.h"
#include <set>

void RubiksCube::AddToWall(WallType type, int index)
{
	for (std::pair<int, std::set<WallType>> cube: Cubes) {
		if (cube.first == index) {
			cube.second.insert(type);
			return;
		}
	}
	std::set<WallType> typeset = std::set<WallType>();
	typeset.insert(type);
	Cubes.insert(std::pair<int, std::set<WallType>>(index, typeset));
}

void RubiksCube::RemoveFromWall(WallType type, int index)
{
	for (std::pair<int, std::set<WallType>> cube : Cubes) {
		if (cube.first == index) {
			cube.second.erase(type);
			return;
		}
	}

}

std::vector<int> RubiksCube::GetWall(WallType type)
{
	std::vector<int> wall = std::vector<int>();
	for (std::pair<int, std::set<WallType>> cube : Cubes) {
		if (cube.second.find(type) != cube.second.end()) {
			wall.push_back(cube.first);
		}
	}
	return wall;
}

void RubiksCube::RotateWall(WallType type, Direction direction)
{
	std::vector<int> wall = GetWall(type);
	for (auto index : wall) {
		std::set<WallType> cube = Cubes[index];
		for (WallType cubeType : cube) {
			if (cubeType != type) {
				WallType* replaceType = ReplaceType(cubeType, type, direction);
				if (replaceType != nullptr) {
					cube.erase(cubeType);
					cube.insert(*replaceType);
				}
			}
		}
	}
}

WallType* RubiksCube::ReplaceType(WallType currentType, WallType rotationType, Direction direction) {
	WallType* typeptr = nullptr;
	switch (rotationType) {
	case (Right):
		switch (currentType) {
		case (Up):
			if (direction == Clockwise)
				typeptr = new WallType(Back);
			else
				typeptr = new WallType(Front);
		case (Down):
			if (direction == Clockwise)
				typeptr = new WallType(Front);
			else
				typeptr = new WallType(Back);

		case (Front):
			if (direction == Clockwise)
				typeptr = new WallType(Up);
			else
				typeptr = new WallType(Down);

		case (Back):
			if (direction == Clockwise)
				typeptr = new WallType(Down);
			else
				typeptr = new WallType(Up);
		}
		break;
	case (Left):
		switch (currentType) {
		case (Up):
			if (direction == Clockwise)
				typeptr = new WallType(Front);
			else
				typeptr = new WallType(Back);
		case (Down):
			if (direction == Clockwise)
				typeptr = new WallType(Back);
			else
				typeptr = new WallType(Front);

		case (Front):
			if (direction == Clockwise)
				typeptr = new WallType(Down);
			else
				typeptr = new WallType(Up);

		case (Back):
			if (direction == Clockwise)
				typeptr = new WallType(Up);
			else
				typeptr = new WallType(Down);
		}
		break;
	case (Up):
		switch (currentType) {
		case (Right):
			if (direction == Clockwise)
				typeptr = new WallType(Back);
			else
				typeptr = new WallType(Front);
		case (Left):
			if (direction == Clockwise)
				typeptr = new WallType(Front);
			else
				typeptr = new WallType(Back);

		case (Front):
			if (direction == Clockwise)
				typeptr = new WallType(Right);
			else
				typeptr = new WallType(Left);

		case (Back):
			if (direction == Clockwise)
				typeptr = new WallType(Left);
			else
				typeptr = new WallType(Right);
		}
		break;
	case (Down):
		switch (currentType) {
		case (Right):
			if (direction == Clockwise)
				typeptr = new WallType(Front);
			else
				typeptr = new WallType(Back);
		case (Left):
			if (direction == Clockwise)
				typeptr = new WallType(Back);
			else
				typeptr = new WallType(Front);

		case (Front):
			if (direction == Clockwise)
				typeptr = new WallType(Right);
			else
				typeptr = new WallType(Down);

		case (Back):
			if (direction == Clockwise)
				typeptr = new WallType(Down);
			else
				typeptr = new WallType(Right);
		}
		break;
	case (Back):
		switch (currentType) {
		case (Up):
			if (direction == Clockwise)
				typeptr = new WallType(Left);
			else
				typeptr = new WallType(Right);
		case (Down):
			if (direction == Clockwise)
				typeptr = new WallType(Right);
			else
				typeptr = new WallType(Left);

		case (Left):
			if (direction == Clockwise)
				typeptr = new WallType(Up);
			else
				typeptr = new WallType(Down);

		case (Right):
			if (direction == Clockwise)
				typeptr = new WallType(Down);
			else
				typeptr = new WallType(Up);
		}
		break;
	case (Front):
		switch (currentType) {
		case (Up):
			if (direction == Clockwise)
				typeptr = new WallType(Right);
			else
				typeptr = new WallType(Left);
		case (Down):
			if (direction == Clockwise)
				typeptr = new WallType(Left);
			else
				typeptr = new WallType(Right);

		case (Right):
			if (direction == Clockwise)
				typeptr = new WallType(Up);
			else
				typeptr = new WallType(Down);

		case (Left):
			if (direction == Clockwise)
				typeptr = new WallType(Down);
			else
				typeptr = new WallType(Up);
		}
		break;
	}
	return typeptr;
}
