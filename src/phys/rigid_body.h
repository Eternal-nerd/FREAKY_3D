#pragma once

#include <glm/glm.hpp>
#include <string>

#include "../types.h"
#include "../util.h"

/*
class for objects that will participate in the dynamics world
*/
class RigidBody {
public:
	void init(RigidBodyProperties p);

	void update();

	glm::vec3 getPosition();

    glm::mat4 getModelMatrix();

	const std::string name_ = "RIGID BODY";

private:
	glm::vec3 position_ = { 0.f, 0.f, 0.f };
	glm::vec3 linearVel_ = { 0.f, 0.f, 0.f };
	glm::mat3 orientation_ = glm::mat3(1);
	glm::vec3 angleVel_ = { 0.f, 0.f, 0.f };

};
