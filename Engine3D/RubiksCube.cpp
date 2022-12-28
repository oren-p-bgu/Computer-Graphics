#include "RubiksCube.h"


void RubiksCube::AddToWall(WallType type, int index)
{
	switch (type) {
	case (Right):
		RightWall.push_back(index);
		break;
	case (Left):
		LeftWall.push_back(index);
		break;
	case (Up):
		UpWall.push_back(index);
		break;
	case (Down):
		DownWall.push_back(index);
		break;
	case (Front):
		FrontWall.push_back(index);
		break;
	case (Back):
		BackWall.push_back(index);
		break;
	}
}

std::vector<int> RubiksCube::GetWall(WallType type)
{
	switch (type) {
	case (Right):
		return RightWall;
	case (Left):
		return LeftWall;
	case (Up):
		return UpWall;
	case (Down):
		return DownWall;
	case (Front):
		return FrontWall;
	case (Back):
		return BackWall;
	}

}
