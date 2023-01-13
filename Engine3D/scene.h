#pragma once
#include "shader.h"
#include "shape.h"
#include "camera.h"
#include "VertexArray.hpp"
#include <vector>
#include "RubiksCube.h"
#include "Bezier1D.h"


class Scene : public MovableGLM
{

public:
	enum axis{xAxis,yAxis,zAxis};
	enum transformations{xTranslate, yTranslate, zTranslate, xRotate, yRotate, zRotate, xScale, yScale, zScale, xCameraTranslate, yCameraTranslate, zCameraTranslate};
	enum modes{POINTS, LINES, LINE_LOOP, LINE_STRIP, TRIANGLES, TRIANGLE_STRIP, TRIANGLE_FAN, QUADS};
	enum buffers{COLOR, DEPTH, STENCIL, BACK, FRONT, ACCUM, NONE };
	enum shapes{Axis, Plane, Cube, Octahedron, Tethrahedron, LineCopy, MeshCopy};
	
	Scene();
	Scene(float angle,float relationWH,float near, float far);
	
	void AddShapeFromFile(const std::string& fileName,int parent,unsigned int mode);
	virtual void AddShape(int type,int parent,unsigned int mode);
	void AddShapeCopy(int indx,int parent,unsigned int mode);
	
	void AddShader(const std::string& fileName);
	void AddTexture(const std::string& textureFileName, bool for2D);
	void AddTexture(int width,int height, unsigned char *data);

	void AddCamera(glm::vec3& pos , float fov, float relationWH, float zNear, float zFar);

	void ZeroShapesTrans();

	virtual void Update(const glm::mat4 &MVP,const glm::mat4 &Normal,const int  shaderIndx) = 0;
	virtual void WhenTranslate(){};
	virtual void WhenRotate(){};
	virtual void Motion(){};
	virtual void Draw(int shaderIndx,int cameraIndx,int buffer,bool toClear,bool debugMode);
	virtual void Draw(int shaderIndx, int cameraIndx, int buffer, bool toClear, bool debugMode, int viewport);
	virtual ~Scene(void);

	void MoveCamera(int cameraIndx,int type,float amt);
	void ShapeTransformation(int type,float amt);

	float Picking(int x,int y);
	void Resize(int width,int hight);

	inline void SetParent(int indx,int newValue) {chainParents[indx]=newValue;}
	inline float GetNear(int cameraIndx){return cameras[cameraIndx]->GetNear();}
	inline float GetFar(int cameraIndx){return cameras[cameraIndx]->GetFar();}

	void ReadPixel(int x, int y);
	
	inline float GetAngle(int cameraIndx) {return cameras[cameraIndx]->GetAngle();}
	inline void Activate() {isActive = true; }
	inline void Deactivate() {isActive = false;}
	void HideShape(int shpIndx);
	void UnhideShape(int shpIndx);

	void UpdatePosition(float xpos, float ypos);
	void MouseProccessing(int button);
	void ScrollProcessing(double xoffest, double yoffset);
	void RotateCube(float amount, glm::vec3 direction);
	bool inline IsActive() const { return isActive;} 

	void RotateCubeWall(WallType type);
	void ToggleDirection() {
		if (direction ==Clockwise) {
			direction = CounterClockwise;
		}
		else {
			direction = Clockwise;
		}
	}
	void ToggleContinuity() { continuity = !continuity; }
	void ToggleActive() { isActive = !isActive; }
	
	inline void SetShapeTex(int shpIndx,int texIndx){shapes[shpIndx]->SetTexture(texIndx);} 
	inline void SetShapeShader(int shpIndx,int shdrIndx){shapes[shpIndx]->SetShader(shdrIndx);} 
	
private:	
	
	std::vector<Camera*> cameras; 

	float depth;
	int xold, yold,xrel, yrel;
	int cameraIndx;
	void Clear(float r, float g, float b, float a);

protected:
	std::vector<Shape*> shapes;
	std::vector<Shader*> shaders;
	std::vector<int> chainParents;
	std::vector<Texture*> textures;

	RubiksCube rubiksCube;
	Direction direction;
	bool continuity;
	Bezier1D* bez;
	
	int pickedShape;
	
	bool isActive;
};

