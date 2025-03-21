#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <string>

#include "../util.h"

/*
class for objects that will participate in the dynamics world
*/
class RigidBody {
public:
	void init();

	void update(float deltaT);

	void setYaw(float yaw);

	// position
	glm::vec3 getPosition();
	void setPosition(glm::vec3 position);

	// orientation
	glm::vec3 getOrientation();
	void setOrientation(glm::vec3 orientation);

	// velocity
	glm::vec3 getVelocity();
	void setVelocity(glm::vec3 velocity);

	const std::string name_ = "RIGID BODY";

private:
	float yaw_ = 0.f;
	glm::vec3 position_ = { 0.f, 0.f, 0.f };
	glm::vec3 linearVel_ = { 0.f, 0.f, 0.f };
	glm::vec3 orientation_ = { 0.f, 0.f, 0.f }; // in degrees, Euler angle
	glm::vec3 angleVel_ = { 0.f, 0.f, 0.f };

};
