#pragma once

#include <glm/glm.hpp>
#include <string>

#include "../util.h"

/*
class for objects that will participate in the dynamics world
*/
class RigidBody {
public:
	void init();

	void update();

	// position
	glm::vec3 getPosition();
	void setPosition(float x, float y, float z);

	// orientation
	glm::vec3 getOrientation();
	void setOrientation(float x, float y, float z);

	const std::string name_ = "RIGID BODY";

private:
	glm::vec3 position_ = { 0.f, 0.f, 0.f };
	glm::vec3 linearVel_ = { 0.f, 0.f, 0.f };
	glm::vec3 orientation_ = { 0.f, 0.f, 0.f }; // in degrees, Euler angle
	glm::vec3 angleVel_ = { 0.f, 0.f, 0.f };

};
