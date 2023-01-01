#include "RubiksCube.h"
#include <set>

void RubiksCube::AddToWall(WallType type, int index)
{

	std::set<WallType> typeset = std::set<WallType>();
	typeset.insert(type);
	for (std::pair<int, std::set<WallType>> cube: Cubes) {
		if (cube.first == index) {
			std::set<WallType> cubeTypes = cube.second;
			typeset.insert(cubeTypes.begin(), cubeTypes.end());
			Cubes.erase(index);
			Cubes.insert(std::pair<int, std::set<WallType>>(index, typeset));
			return;
		}
	}
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
		std::set<std::pair<WallType, WallType>> toReplace = std::set<std::pair<WallType, WallType>>();
		for (WallType cubeType : cube) {
			if (cubeType != type) {
				WallType* replaceType = ReplaceType(cubeType, type, direction);
				if (replaceType != nullptr) {
					toReplace.insert(std::pair<WallType, WallType>(cubeType, *replaceType));
				}
			}
		}
		for (std::pair<WallType, WallType> replacePair : toReplace) {
			cube.erase(replacePair.first);
		}
		for (std::pair<WallType, WallType> replacePair : toReplace) {
			cube.insert(replacePair.second);
		}
		Cubes[index] = cube;
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
			break;
		case (Down):
			if (direction == Clockwise)
				typeptr = new WallType(Front);
			else
				typeptr = new WallType(Back);
						break;

		case (Front):
			if (direction == Clockwise)
				typeptr = new WallType(Up);
			else
				typeptr = new WallType(Down);
						break;

		case (Back):
			if (direction == Clockwise)
				typeptr = new WallType(Down);
			else
				typeptr = new WallType(Up);
						break;
		}
		break;
	case (Left):
		switch (currentType) {
		case (Up):
			if (direction == Clockwise)
				typeptr = new WallType(Front);
			else
				typeptr = new WallType(Back);
						break;
		case (Down):
			if (direction == Clockwise)
				typeptr = new WallType(Back);
			else
				typeptr = new WallType(Front);
						break;

		case (Front):
			if (direction == Clockwise)
				typeptr = new WallType(Down);
			else
				typeptr = new WallType(Up);
						break;

		case (Back):
			if (direction == Clockwise)
				typeptr = new WallType(Up);
			else
				typeptr = new WallType(Down);
						break;
		}
		break;
	case (Up):
		switch (currentType) {
		case (Right):
			if (direction == Clockwise)
				typeptr = new WallType(Front);
			else
				typeptr = new WallType(Back);
						break;
		case (Left):
			if (direction == Clockwise)
				typeptr = new WallType(Back);
			else
				typeptr = new WallType(Front);
						break;

		case (Front):
			if (direction == Clockwise)
				typeptr = new WallType(Left);
			else
				typeptr = new WallType(Right);
						break;

		case (Back):
			if (direction == Clockwise)
				typeptr = new WallType(Right);
			else
				typeptr = new WallType(Left);
						break;
		}
		break;
	case (Down):
		switch (currentType) {
		case (Right):
			if (direction == Clockwise)
				typeptr = new WallType(Back);
			else
				typeptr = new WallType(Front);
						break;
		case (Left):
			if (direction == Clockwise)
				typeptr = new WallType(Front);
			else
				typeptr = new WallType(Back);
						break;

		case (Front):
			if (direction == Clockwise)
				typeptr = new WallType(Right);
			else
				typeptr = new WallType(Left);
						break;

		case (Back):
			if (direction == Clockwise)
				typeptr = new WallType(Left);
			else
				typeptr = new WallType(Right);
						break;
		}
		break;
	case (Back):
		switch (currentType) {
		case (Up):
			if (direction == Clockwise)
				typeptr = new WallType(Left);
			else
				typeptr = new WallType(Right);
						break;
		case (Down):
			if (direction == Clockwise)
				typeptr = new WallType(Right);
			else
				typeptr = new WallType(Left);
						break;

		case (Left):
			if (direction == Clockwise)
				typeptr = new WallType(Down);
			else
				typeptr = new WallType(Up);
						break;

		case (Right):
			if (direction == Clockwise)
				typeptr = new WallType(Up);
			else
				typeptr = new WallType(Down);
						break;
		}
		break;
	case (Front):
		switch (currentType) {
		case (Up):
			if (direction == Clockwise)
				typeptr = new WallType(Right);
			else
				typeptr = new WallType(Left);
						break;
		case (Down):
			if (direction == Clockwise)
				typeptr = new WallType(Left);
			else
				typeptr = new WallType(Right);
						break;

		case (Right):
			if (direction == Clockwise)
				typeptr = new WallType(Down);
			else
				typeptr = new WallType(Up);
						break;

		case (Left):
			if (direction == Clockwise)
				typeptr = new WallType(Up);
			else
				typeptr = new WallType(Down);
						break;
		}
		break;
	}
	return typeptr;
}
