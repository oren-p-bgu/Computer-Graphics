#include "game.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.c>
#include <numbers>
#include <iostream>
#include <fstream>
#include "RubiksCube.h"
#include <Bezier1D.h>
# define PI           3.14159265358979323846  /* pi */
static void printMat(const glm::mat4 mat)
{
	std::cout<<" matrix:"<<std::endl;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
			std::cout<< mat[j][i]<<" ";
		std::cout<<std::endl;
	}
}

Game::Game() : Scene()
{
}

Game::Game(float angle ,float relationWH, float near1, float far1) : Scene(angle,relationWH,near1,far1)
{ 	
}

void Game::AddControlPoints(Bezier1D* bez) {
	int segNum = bez->GetSegmentsNum();
	glm::vec4 center = bez->GetControlPoint(0, 0);
	AddShape(Octahedron, -1, TRIANGLES);
	shapes[2]->MyTranslate(glm::vec3(center), 0);
	float scale = 0.1;
	shapes[2]->MyScale(glm::vec3(scale));
	for (int segment = 0; segment < segNum; segment++) {
		for (int indx = 1; indx < 4; indx++) {
			glm::vec4 center = bez->GetControlPoint(segment, indx);
			AddShape(Octahedron, -1, TRIANGLES);
			shapes[2+segment*3 + indx]->MyTranslate(glm::vec3(center), 0);
			shapes[2+segment*3 + indx]->MyScale(glm::vec3(scale));
		}
	}
}


void Game::AddBezierCurve(int segNum) {
	if (shapes.size() > 1) {
		shapes.erase(std::next(shapes.begin(), 1), shapes.end());
	}

	Bezier1D* bez = new Bezier1D(segNum);
	this->bez = bez;
	chainParents.push_back(-1);
	shapes.push_back(bez);
	SetShapeTex(1, 0);

	AddControlPoints(bez);

}

RubiksCube Game::BuildRubiksCube(int dimension) {
	RubiksCube cube = RubiksCube();
	int index = 0;
	//AddShape(Plane,-1,TRIANGLES); // debug plane
	//SetShapeTex(index, 0);
	//shapes[index]->MyScale(glm::vec3(3, 3, 3));
	//index++;
	float step = 2 / ((float) dimension - 1);
	float scale = 1 / ((float) dimension - 1);
	for (float i = -1; i <= 1; i+= step) {

		for (float j = -1; j <= 1; j+= step) {

			for (float k = -1; k <= 1; k+= step) {

				if (i == -1 || i == 1 || j == -1 || j == 1 || k == -1 || k == 1)
				{
					// Only adds the outermost, visible cubes
					AddShape(Cube,-1,TRIANGLES);
					SetShapeTex(index, 0);
					shapes[index]->MyScale(glm::vec3(scale, scale, scale));
					shapes[index]->MyTranslate(glm::vec3(i, j, k), 0);
					if (i == -1) {
						cube.AddToWall(Left,index);
					}
					if (i == 1) {
						cube.AddToWall(Right, index);
					}
					if (j == -1) {
						cube.AddToWall(Down, index);
					}
					if (j == 1) {
						cube.AddToWall(Up, index);
					}
					if (k == -1) {
						cube.AddToWall(Back, index);
					}
					if (k == 1) {
						cube.AddToWall(Front, index);
					}
					index++;
				}
			}
		}
	}
	return cube;
}

void Game::Init()
{		

	AddShader("../res/shaders/pickingShader");	
	AddShader("../res/shaders/basicShader");
	
	AddTexture("../res/textures/plane.png", false);

	AddShape(Cube, -1, TRIANGLES);
	SetShapeTex(0, 0);
	shapes[0]->MyScale(glm::vec3(0.3,0.3,0.3));

	AddBezierCurve(3);
	
	pickedShape = 0;
	
	MoveCamera(0,zTranslate,10);
	pickedShape = -1;
	
	//ReadPixel(); //uncomment when you are reading from the z-buffer
}

unsigned char* Game::Grayscale(int width, int height, unsigned char* data)
{
	std::vector<unsigned char> grayscaledVector;
	for (int i = 0; i < width * height*4; i=i+4) {
		unsigned char avg = (data[i] + data[i + 1] + data[i + 2]) / 3;
		grayscaledVector.push_back(avg);
		grayscaledVector.push_back(avg);
		grayscaledVector.push_back(avg);
		grayscaledVector.push_back(data[i+3]);
	}
	unsigned char* grayscaled;
	grayscaled = (unsigned char*)malloc(grayscaledVector.size() * sizeof(unsigned char));
	std::copy(grayscaledVector.begin(), grayscaledVector.end(), grayscaled);
	return grayscaled;
}

unsigned char* Game::EdgeDetection(int width, int height, unsigned char* data)
{
	int sobelXMatrix[3][3] = {	{1,0,-1}, 
											{2,0,-2},
											{1,0,-1} };

	int sobelYMatrix[3][3] = {	{1,2,1}, 
											{0,0,0},
											{-1,-2,-1} };

	int lowThreshold = 0.9 * 255;
// Make a 2d version of the data for easier handling.	
	std::vector<std::vector<int>> data2d(height, std::vector<int> (width));

	for (int row = 0; row < height; row++) {
		for (int col = 0; col < width; col++) {
			data2d[row][col] = data[row * width * 4 + col * 4];
		}
	}

	std::vector<std::vector<int>> gradientMatrix(height, std::vector<int> (width));
	std::vector<std::vector<int>> edgesMatrix(height, std::vector<int> (width));

	// GradientDirectionMatrix in degrees.
	std::vector<std::vector<int>> gradientDirectionMatrix(height, std::vector<int> (width));

	int max_gradient = 0;
	for (int row = 0; row < height; row++) {
		for (int col = 0; col < width; col++) {
			int sumX = 0;
			int sumY = 0;
			for (int i = -1; i < 2; i++) {
				for (int j = -1; j < 2; j++) {
					if (i == 0 && j == 0) {
						continue;
					}
					int x = row + i;
					int y = col + j;
					if (x < 0 || x > width - 1 || y < 0 || y > height - 1) {
						continue;
					}

					sumX = sumX + sobelXMatrix[i + 1][j + 1] * data2d[x][y];
					sumY = sumY + sobelYMatrix[i + 1][j + 1] * data2d[x][y];
				}
			}


			int gradient = sqrt(sumX * sumX + sumY * sumY);
			if (gradient > max_gradient) {
				max_gradient = gradient;
			}
			gradientMatrix[row][col] = gradient;

			int gradientDirection = atan2(sumY, sumX);
			gradientDirectionMatrix[row][col] = trunc(gradientDirection * 180 / PI + 0.5);
		}
	}
	for (int row = 0; row < height; row++) {
		for (int col = 0; col < width; col++) {
			if (gradientMatrix[row][col] < lowThreshold) {
				edgesMatrix[row][col] = 0;
				continue;
			}

			int prev = 0;
			int next = 0;
			int direction = gradientDirectionMatrix[row][col];
			int intensity = gradientMatrix[row][col];
			// This section maps a direction and rounds it to the nearest cardinal direction to compare to the 
			// correct pixels and find the local maximum.
			// Direction East-West
			if (direction <= -157.5 || (direction >= -22.5 && direction <= 22.5) || (direction >= 157.5) ) {
				if (col + 1 < width && intensity >= gradientMatrix[row][col + 1]) {
					next = gradientMatrix[row][col + 1];
				}
				if (col != 0 && intensity >= gradientMatrix[row][col -1]) {
					prev = gradientMatrix[row][col - 1];
				}
			}
// Direction North West-South East
			if ((direction >= 22.5 && direction <= 67.5) || (direction >= -157.5 && direction <= -112.5) ) {
				if (col + 1 < width && row != 0 && intensity >= gradientMatrix[row - 1][col + 1]) {
					next = gradientMatrix[row - 1][col + 1];
				}
				if (col != 0 && row + 1 < height && intensity >= gradientMatrix[row + 1][col -1]) {
					prev = gradientMatrix[row + 1][col - 1];
				}
			}
// Direction North-South
			if ((direction >= 67.5 && direction <= 112.5) || (direction >= -112.5 && direction <= -67.5) ) {
				if (row + 1 < width && intensity >= gradientMatrix[row+1][col]) {
					next = gradientMatrix[row + 1][col];
				}
				if (row != 0 && intensity >= gradientMatrix[row-1][col]) {
					prev = gradientMatrix[row - 1][col];
				}
			}
// Direction North East-South West
			if ((direction >= 112.5 && direction <= 157.5) || (direction >= -67.5 && direction <= -22.5) ) {
				if (col + 1 < width && intensity >= gradientMatrix[row][col + 1]) {
					next = gradientMatrix[row][col + 1];
				}
				if (col != 0 && intensity >= gradientMatrix[row][col -1]) {
					prev = gradientMatrix[row][col - 1];
				}
			}
			
			if (intensity > prev && intensity > next) {
				edgesMatrix[row][col] = 255;
			}
			else {
				edgesMatrix[row][col] = 0;
			}
		}
	}

	std::ofstream outputFile;
	outputFile.open("../img4.txt");
	std::vector<unsigned char> gradientIntensitiesVector;
	for (int row = 0; row < height; row++) {
		for (int col = 0; col < width; col++) {
			outputFile << std::to_string(edgesMatrix[row][col] / 255) << ",";
			gradientIntensitiesVector.push_back(edgesMatrix[row][col]);
			gradientIntensitiesVector.push_back(edgesMatrix[row][col]);
			gradientIntensitiesVector.push_back(edgesMatrix[row][col]);
			gradientIntensitiesVector.push_back(data[row * width * 4 + col * 4 + 3]);
		}
	}
	outputFile.close();
	
	unsigned char* gradientIntensities;
	gradientIntensities = (unsigned char*)malloc(gradientIntensitiesVector.size() * sizeof(unsigned char));
	std::copy(gradientIntensitiesVector.begin(), gradientIntensitiesVector.end(), gradientIntensities);
	return gradientIntensities;
}

unsigned char* Game::Halftones(int width, int height, unsigned char* data)
{
	const int MAX_INTENSITY = 255;
	const int NUM_INTENSITIES = 4;
	// Make a 2d version of the data for easier handling.	
	std::vector<std::vector<double>> data2d(height, std::vector<double>(width));

	int max = 0;
	int min = 255;
	for (int row = 0; row < height; row++) {
		for (int col = 0; col < width; col++) {
			data2d[row][col] = data[row * width * 4 + col * 4];
			if (data2d[row][col] > max) {
				max = data2d[row][col];
			}
			if (data2d[row][col] < min) {
				min = data2d[row][col];
			}
		}
	}

	// Tried improving contrast, didn't seem to do much.
	double factor = 255.0 / (double)(max-min);
	for (int row = 0; row < height; row++) {
		for (int col = 0; col < width; col++) {
			data2d[row][col] = trunc((double)(data2d[row][col] - min) * factor);
		}
			
	}


	std::vector<std::vector<int>> halftonedMatrix(height, std::vector<int>(width));

	for (int row = 0; row < height-1; row+=2) {
		for (int col = 0; col < width-1; col+=2) {
			double average = (data2d[row][col] + data2d[row][col + 1] + data2d[row + 1][col] + data2d[row + 1][col + 1])/4.0;
			int intensity = trunc(((double)average / (double)MAX_INTENSITY) * (double)NUM_INTENSITIES + 0.5);
			switch (intensity) {
			case 0:
				halftonedMatrix[row][col] = 0;
				halftonedMatrix[row][col+1] = 0;
				halftonedMatrix[row+1][col] = 0;
				halftonedMatrix[row+1][col+1] = 0;
				break;
			case 1:
				halftonedMatrix[row][col] = 255;
				halftonedMatrix[row][col+1] = 0;
				halftonedMatrix[row+1][col] = 0;
				halftonedMatrix[row+1][col+1] = 0;
				break;
			case 2:
				halftonedMatrix[row][col] = 255;
				halftonedMatrix[row][col+1] = 0;
				halftonedMatrix[row+1][col] = 0;
				halftonedMatrix[row+1][col+1] = 255;
				break;
			case 3:
				halftonedMatrix[row][col] = 255;
				halftonedMatrix[row][col+1] = 255;
				halftonedMatrix[row+1][col] = 0;
				halftonedMatrix[row+1][col+1] = 255;
				break;
			case 4:
				halftonedMatrix[row][col] = 255;
				halftonedMatrix[row][col+1] = 255;
				halftonedMatrix[row+1][col] = 255;
				halftonedMatrix[row+1][col+1] = 255;
				break;
			}
		}
	}
	std::ofstream outputFile;
	outputFile.open("../img5.txt");

	std::vector<unsigned char> halftonedVector;
	for (int row = 0; row < height; row++) {
		for (int col = 0; col < width; col++) {
			outputFile << std::to_string((int)(halftonedMatrix[row][col] / 255)) << ",";
			halftonedVector.push_back(halftonedMatrix[row][col]);
			halftonedVector.push_back(halftonedMatrix[row][col]);
			halftonedVector.push_back(halftonedMatrix[row][col]);
			halftonedVector.push_back(data[row * width * 4 + col * 4 + 3]);
		}
	}
	outputFile.close();

	unsigned char* halftoned;
	halftoned = (unsigned char*)malloc(halftonedVector.size() * sizeof(unsigned char));
	std::copy(halftonedVector.begin(), halftonedVector.end(), halftoned);
	return halftoned;
}

unsigned char* Game::FSAlgorithm(int width, int height, unsigned char* data)
{
	const int NUM_INTENSITIES = 15;
	const int MAX_INTENSITY = 255;
	// Make a 2d version of the data for easier handling.	
	std::vector<std::vector<double>> data2d(height, std::vector<double>(width));

	for (int row = 0; row < height; row++) {
		for (int col = 0; col < width; col++) {
			data2d[row][col] = data[row * width * 4 + col * 4];
		}
	}

	std::vector<std::vector<int>> fsMatrix(height, std::vector<int>(width));

	for (int row = 0; row < height; row++) {
		for (int col = 0; col < width; col++) {
			int intensity = trunc(((double)data2d[row][col] / (double)MAX_INTENSITY) * (double)NUM_INTENSITIES + 0.5);
			fsMatrix[row][col] = trunc((double)intensity * ((double)MAX_INTENSITY / (double)NUM_INTENSITIES));
			double error = data2d[row][col] - fsMatrix[row][col];
			if (col + 1 < width) {
				data2d[row][col + 1] += error * 7.0 / 16.0;
			}
			if (col != 0 && row + 1 < height) {
				data2d[row + 1][col - 1] += error * 3.0 / 16.0;
			}
			if (row + 1 < height) {
				data2d[row + 1][col] += error * 5.0 / 16.0;
			}
			if (col + 1 < width && row + 1 < height) {
				data2d[row + 1][col + 1] += error * 1.0 / 16.0;
			}
		}
	}

	std::ofstream outputFile;
	outputFile.open("../img6.txt");
	std::vector<unsigned char> fsVector;
	for (int row = 0; row < height; row++) {
		for (int col = 0; col < width; col++) {
			outputFile << std::to_string((int)trunc(((double)fsMatrix[row][col] / (double)MAX_INTENSITY) * (double)NUM_INTENSITIES + 0.5)) << ",";
			fsVector.push_back(fsMatrix[row][col]);
			fsVector.push_back(fsMatrix[row][col]);
			fsVector.push_back(fsMatrix[row][col]);
			fsVector.push_back(data[row * width * 4 + col * 4 + 3]);
		}
	}
	outputFile.close();

	unsigned char* fsalgo;
	fsalgo = (unsigned char*)malloc(fsVector.size() * sizeof(unsigned char));
	std::copy(fsVector.begin(), fsVector.end(), fsalgo);
	return fsalgo;
}

void Game::Update(const glm::mat4 &MVP,const glm::mat4 &Model,const int  shaderIndx)
{
	Shader *s = shaders[shaderIndx];
	int r = ((pickedShape+1) & 0x000000FF) >>  0;
	int g = ((pickedShape+1) & 0x0000FF00) >>  8;
	int b = ((pickedShape+1) & 0x00FF0000) >> 16;
	s->Bind();
	s->SetUniformMat4f("MVP", MVP);
	s->SetUniformMat4f("Normal",Model);
	s->SetUniform4f("lightDirection", 0.0f , 0.0f, -1.0f, 0.0f);
	if(shaderIndx == 0)
		s->SetUniform4f("lightColor",r/255.0f, g/255.0f, b/255.0f,1.0f);
	else 
		s->SetUniform4f("lightColor",0.7f,0.8f,0.1f,1.0f);
	s->Unbind();
}

void Game::WhenRotate()
{
}

void Game::WhenTranslate()
{
}

void Game::Motion()
{
	if(isActive)
	{
		glm::vec4 newLocation = bez->GetCurrentLocation();
		shapes[0]->MyTranslate(glm::vec3(newLocation), 1);
		glm::vec3 velocity = bez->GetCurrentVelocity();
		shapes[0]->MyRotate(velocity, glm::vec3(0, 1, 0));
		bez->next(0.01);
	}
}

Game::~Game(void)
{
}


