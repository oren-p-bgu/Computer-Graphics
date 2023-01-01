#pragma once
#include <vector>
#include "MovableGLM.h"
#include <set>
#include <map>
enum WallType {
	Right,
	Left,
	Up,
	Down,
	Front,
	Back
};
enum Direction {
	Clockwise,
	CounterClockwise
};
class RubiksCube : public MovableGLM
{
public:
	RubiksCube() : Cubes(std::map<int, std::set<WallType>>()){};

	void AddToWall(WallType type, int index);
	void RemoveFromWall(WallType type, int index);
	std::vector<int> GetWall(WallType type);
	void RotateWall(WallType type, Direction direction);

	WallType* ReplaceType(WallType currentType, WallType rotationType, Direction direction);

	std::map<int, std::set<WallType>> Cubes;
};

