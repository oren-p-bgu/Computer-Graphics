#include "MovableGLM.h"
#include <stdio.h>
#include "../res/includes/glm/gtc/matrix_transform.hpp"
#include "../res/includes/glm/gtc/quaternion.hpp"


static void printMat(const glm::mat4 mat)
{
	printf(" matrix: \n");
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
			printf("%f ", mat[j][i]);
		printf("\n");
	}
}

MovableGLM::MovableGLM()
{
	ZeroTrans();
}

glm::mat4 MovableGLM::MakeTrans(const glm::mat4 &prevTransformations) const
{
	return prevTransformations * MakeTrans();
}

glm::mat4 MovableGLM::MakeTrans() const
{
	return   trans* rot *scl ;
}

void MovableGLM::MyTranslate(glm::vec3 delta,int mode)
{
	if (mode == 1) { // not relational
		trans = glm::translate(glm::mat4(1), delta);
	}
	else {
		trans = glm::translate(trans,delta);
	}
}

void  MovableGLM::MyRotate(float angle,const glm::vec3 &vec,int mode)
{
	rot = glm::rotate(rot,angle,vec);
}

void MovableGLM::RotateRelative(float angle, const glm::vec3& vec, MovableGLM other) {
	glm::vec3 thisOrigin = glm::vec3(trans[3][0], trans[3][1], trans[3][2]);
	glm::vec3 otherOrigin = glm::vec3(other.trans[3][0], other.trans[3][1], other.trans[3][2]);
	glm::vec3 delta = otherOrigin - thisOrigin;

	glm::vec3 relativeVec = glm::vec3(  other.MakeTrans() *glm::vec4(vec, 1));
	glm::vec3 newDelta = glm::vec3((glm::rotate(glm::mat4(1), angle, relativeVec) * (other.trans * glm::vec4(thisOrigin, 1)))) - thisOrigin;
	
	glm::vec3 vec2 = glm::vec3( glm::inverse(rot) * ( other.rot * glm::vec4(vec, 1)));
	MyTranslate(newDelta, 0);
	MyRotate(angle, vec2, 0);
}

void MovableGLM::RotateRelativeOrigin(float angle, const glm::vec3& vec, MovableGLM other) {
	glm::vec3 thisOrigin = glm::vec3(trans[3][0], trans[3][1], trans[3][2]);
	glm::vec3 otherOrigin = glm::vec3(other.trans[3][0], other.trans[3][1], other.trans[3][2]);
	glm::vec3 delta = otherOrigin - thisOrigin;

	glm::vec3 newDelta = glm::vec3((glm::rotate(glm::mat4(1), angle, vec) * (other.trans * glm::vec4(thisOrigin, 1)))) - thisOrigin;
	
	glm::vec3 relativeVec = glm::vec3( glm::vec4(vec, 1) *  MakeTrans() );
	MyTranslate(newDelta, 0);
	MyRotate(angle, relativeVec, 0);
}
void MovableGLM::RotateRelativeGlobal(float angle, const glm::vec3& vec) {
	glm::vec3 relativeVec = glm::vec3(glm::vec4(vec, 1) * MakeTrans() );
	rot = glm::rotate(rot, angle, relativeVec);
}
	
void  MovableGLM::MyScale(glm::vec3 scale)
{
	scl = glm::scale(scl,scale);
}

void MovableGLM::ZeroTrans()
{
	trans = glm::mat4(1);
	rot = glm::mat4(1);
	scl = glm::mat4(1);
}