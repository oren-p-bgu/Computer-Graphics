#pragma once
#include "../res/includes/glm/glm.hpp"

class MovableGLM 
{
public:
	MovableGLM();
	glm::mat4 MakeTrans(const glm::mat4 &prevTransformations) const;
	glm::mat4 MakeTrans() const;
	void MyTranslate(glm::vec3 delta, int mode);
	void MyRotate(float angle, const glm::vec3 &vec, int mode);
	void MyRotate(glm::vec3 velocity, glm::vec3 axisToShift);
	void RotateRelative(float angle, const glm::vec3& vec, MovableGLM other);
	void RotateRelativeOrigin(float angle, const glm::vec3& vec, MovableGLM other);
	void RotateRelativeGlobal(float angle, const glm::vec3& vec);
	void MyScale(glm::vec3 scale);
	void ZeroTrans();
	glm::vec3 GetOrigin();
	float GetScale();
private:
	glm::mat4 trans;
	glm::mat4 rot;
	glm::mat4 scl;
};