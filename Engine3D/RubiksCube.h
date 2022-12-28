#pragma once
#include <vector>

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
class RubiksCube
{
public:
	RubiksCube() :  RightWall(std::vector<int>()),
					LeftWall(std::vector<int>()),
					UpWall(std::vector<int>()),
					DownWall(std::vector<int>()),
					FrontWall(std::vector<int>()),
					BackWall(std::vector<int>()){};

	void AddToWall(WallType type, int index);
	std::vector<int> GetWall(WallType type);

private:
	std::vector<int> RightWall;
	std::vector<int> LeftWall;
	std::vector<int> UpWall;
	std::vector<int> DownWall;
	std::vector<int> FrontWall;
	std::vector<int> BackWall;
};

