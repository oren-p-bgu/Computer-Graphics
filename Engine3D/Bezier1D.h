#pragma once

//#include "Mesh.h"
#include "../Engine3D/shape.h"

class Bezier1D : public Shape
{
	std::vector<glm::mat4> segments;
	int resT;
	 const static glm::mat4 M;
	 float MoveControlPoint(int segment, int indx, float dx, float dy, bool preserveC1);  //change the position of one control point. when preserveC1 is true it may affect other  control points 
	 float currentLocation; // Value of 0 to 1, represents point on combination of all segments
	 bool direction;
public:
	Bezier1D() : Bezier1D(3) {}
	Bezier1D(int segNum) : Bezier1D(segNum, 100, 3, 0) {}
	Bezier1D(int segNum, int res,int mode, int viewport = 0);
	IndexedModel GetLine(std::vector<glm::mat4> startingSegments, int resT);	//generates a model for MeshConstructor Constructor with resT vertices. See Mesh.h/cpp for a IndexedModel defenition and how to intialize it
	glm::vec4 GetControlPoint(int segment, int indx) const; //returns a control point in the requested segment. indx will be 0,1,2,3, for p0,p1,p2,p3
	
	glm::vec4 GetPointOnCurve(int segment, float t) const; //returns point on curve in the requested segment for the value of t
	glm::vec4 GetPointOnCurve(std::vector<glm::mat4> semgents, int segment, float t) const;
	glm::vec3 GetVelocity(int segment, float t); //returns the derivative of the curve in the requested segment for the value of t
	glm::vec4 GetCurrentLocation();
	glm::vec3 GetCurrentVelocity();
	void next(float stepSize);

	void AddSegment(glm::vec4 p1, glm::vec4 p2, glm::vec4 p3);  // adds a segment at the end of the curve
	void CurveUpdate(int pointIndx, float dx, float dy, bool preserveC1 = false);  //changes the line in by using ChangeLine function of MeshConstructor and MoveControlPoint 
	void ChangeSegment(int segIndx, glm::vec4 p1, glm::vec4 p2, glm::vec4 p3); // changes three control point of a segment
	void ResetCurve(int segNum); // to the init state
	static std::vector<glm::mat4> StartingSegments(int segNum);
	inline int GetSegmentsNum() const { return segments.size(); }
	~Bezier1D(void);
};

