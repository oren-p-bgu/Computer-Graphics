#include "Bezier1D.h"


Bezier1D::Bezier1D(int segNum,int res,int mode, int viewport) : controlOcts(std::vector<Shape*>()), currentLocation(0), direction(true),  resT(res), Shape(GetLine(StartingSegments(segNum), res),mode)
{  
    ResetCurve(segNum);
}

glm::mat4 const Bezier1D::M = glm::mat4(glm::vec4(-1, 3, -3, 1),
    glm::vec4(3, -6, 3, 0),
    glm::vec4(-3, 3, 0, 0),
    glm::vec4(1, 0, 0, 0));

IndexedModel Bezier1D::GetLine(std::vector<glm::mat4> startingSegments, int res) 
{
    
    IndexedModel model;

    std::vector<LineVertex> vertices;
    float stepSize = (float)startingSegments.size() / (float) res;
    for (float i = 0, index = 0; i <=startingSegments.size(); i += stepSize, index++) {
        float segment;
        float t = std::modf(i, &segment);
        glm::vec4 point = GetPointOnCurve(startingSegments, (int)segment, t);
        vertices.push_back(LineVertex(glm::vec3(point), glm::vec3(1, 0, 0)));
        model.indices.push_back(index);
    }

    for (LineVertex lineVertex : vertices) {
        model.positions.push_back(*lineVertex.GetPos());
        model.colors.push_back(*lineVertex.GetColor());
    }

    return model;
}

glm::vec4 Bezier1D::GetControlPoint(int segment, int indx) const
{
    if (segment < GetSegmentsNum())
        return segments[segment][indx];
    return segments[GetSegmentsNum() - 1][3];
}

glm::vec4 Bezier1D::GetPointOnCurve(int segment, float t) const
{
    return GetPointOnCurve(segments, segment, t);
}

glm::vec4 Bezier1D::GetPointOnCurve(std::vector<glm::mat4> segments, int segment, float t) const
{
    glm::mat4 G = glm::transpose(segments[segment]);
    glm::vec4 T = glm::vec4(pow(t, 3),pow(t,2), t, 1);
    glm::vec4 TM = T * M;
    glm::vec4 TMG = TM * G;
    return TMG;
}


glm::vec3 Bezier1D::GetVelocity(int segment, float t)
{
    // Taken the equation from wikipedia
    glm::vec4 derivative = (GetControlPoint(segment, 1) - GetControlPoint(segment, 0)) * (float)(3 * pow((1 - t), 2));
    derivative += (GetControlPoint(segment, 2) - GetControlPoint(segment, 1)) * (float)(6 * (1 - t) * t);
    derivative += (GetControlPoint(segment, 3) - GetControlPoint(segment, 2)) * (float)(3 * pow( t, 2));
    return glm::vec3(derivative);
}

// void Bezier1D::SplitSegment(int segment, int t){}

void Bezier1D::AddSegment(glm::vec4 p1, glm::vec4 p2, glm::vec4 p3)
{
    glm::vec4 p0 = segments.back()[3];
    segments.push_back(glm::mat4(p0, p1, p2, p3));
}

void Bezier1D::AddControlOct(Shape* oct)
{
    controlOcts.push_back(oct);
}

void Bezier1D::ChangeSegment(int segIndx,glm::vec4 p1, glm::vec4 p2, glm::vec4 p3)
{
    glm::vec4 p0 = segments[segIndx-1][3];
    segments[segIndx] = glm::mat4(p0, p1, p2, p3);
}

glm::vec4 Bezier1D::GetCurrentLocation()
{
    float segSize = 1.0 / (float)GetSegmentsNum();
    int segment = std::floor(currentLocation / segSize);
    float t = (float) GetSegmentsNum()* (currentLocation - (float)segment * segSize);

    return GetPointOnCurve(segment, t);
}

float Bezier1D::MoveControlPoint(int segment, int indx, float dx,float dy,float dz, bool preserveC1)
{
    glm::vec4 cp = GetControlPoint(segment, indx);
    glm::vec4 newcp = cp + glm::vec4(dx, dy, dz, 0);
    if (segment >= GetSegmentsNum()) {
        segment = GetSegmentsNum() - 1;
        indx = 3;
    }
    segments[segment][indx] = newcp;
    if (indx == 0) {
        if (preserveC1) {
            segments[segment][1] += glm::vec4(dx, dy, dz, 0);
            MoveControlOct(segment, 1, dx, dy, dz);
        }
        if (segment != 0) {
            segments[segment - 1][3] = newcp;
			if (preserveC1) {
				segments[segment-1][2] += glm::vec4(dx, dy, dz, 0);
                MoveControlOct(segment-1, 2, dx, dy, dz);
			}
        }
    }
    if (indx == 3) {
        if (preserveC1) {
            segments[segment][2] += glm::vec4(dx, dy, dz, 0);
            MoveControlOct(segment, 2, dx, dy, dz);
        }
        if (segment != (GetSegmentsNum() - 1)) {
            segments[segment + 1][0] = newcp;
			if (preserveC1) {
				segments[segment+1][1] += glm::vec4(dx, dy, dz, 0);
                MoveControlOct(segment+1, 1, dx, dy, dz);
			}
        }
    }
    return 0; //not suppose to reach here
}

void Bezier1D::CurveUpdate(int pointIndx, float dx, float dy,float dz,  bool preserveC1)
{
    pointIndx += std::floor(pointIndx / 3);
    int segment = pointIndx / 4;
    int index = pointIndx % 4;
    MoveControlPoint(segment, index, dx, dy,dz, preserveC1);
    mesh->ChangeLine(GetLine(segments, resT));
    
}

void Bezier1D::MoveControlOct(int segment, int indx, float dx, float dy, float dz)
{
    int index = segment * 3 + indx;
    controlOcts[index]->MyTranslate(glm::vec3(dx, dy, dz), 0);
}

void Bezier1D::ResetCurve(int segNum)
{
    segments.clear();
    int straightSegments = segNum - 2;
    segments.push_back(glm::mat4(glm::vec4(0,0,0,0), glm::vec4(0,0.5,0,0), glm::vec4(0.5,1,0,0), glm::vec4(1,1,0,0)));
    for (int i = 1; i <= straightSegments; i++)
    {
        segments.push_back(glm::mat4(glm::vec4(i,1,0,0), glm::vec4(i+0.2,1,0,0), glm::vec4(i+0.8,1,0,0), glm::vec4(i+1,1,0,0)));
    }
    segments.push_back(glm::mat4(glm::vec4(straightSegments+1,1,0,0), glm::vec4(straightSegments+1.5,1,0,0), glm::vec4(straightSegments+2,0.5,0,0), glm::vec4(straightSegments+2,0,0,0)));
}

glm::vec3 Bezier1D::GetCurrentVelocity()
{
    float segSize = 1.0 / (float)GetSegmentsNum();
    int segment = std::floor(currentLocation / segSize);
    float t = (float) GetSegmentsNum()* (currentLocation - (float)segment * segSize);

    return GetVelocity(segment, t);
}

std::vector<glm::mat4> Bezier1D::StartingSegments(int segNum) {
    std::vector<glm::mat4> segments = std::vector<glm::mat4>();
    int straightSegments = segNum - 2;
    segments.push_back(glm::mat4(glm::vec4(0,0,0,0), glm::vec4(0,0.5,0,0), glm::vec4(0.5,1,0,0), glm::vec4(1,1,0,0)));
    for (int i = 1; i <= straightSegments; i++)
    {
        segments.push_back(glm::mat4(glm::vec4(i,1,0,0), glm::vec4(i+0.2,1,0,0), glm::vec4(i+0.8,1,0,0), glm::vec4(i+1,1,0,0)));
    }
    segments.push_back(glm::mat4(glm::vec4(straightSegments+1,1,0,0), glm::vec4(straightSegments+1.5,1,0,0), glm::vec4(straightSegments+2,0.5,0,0), glm::vec4(straightSegments+2,0,0,0)));
    return segments;
}

void Bezier1D::next(float stepSize)
{
    float nextLocation;
    if (direction) {
        nextLocation = currentLocation + stepSize;
        if (nextLocation >= 1) {
            nextLocation = 2 - nextLocation;
            direction = false;
        }
    }
    else {
        nextLocation = currentLocation - stepSize;
        if (nextLocation <= 0) {
            nextLocation = - nextLocation;
            direction = true;
        }
    }
    currentLocation = nextLocation;
}

Bezier1D::~Bezier1D(void)
{

}