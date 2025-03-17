#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/transform.hpp>
#include <cmath>
#include <string>

#include "types.h"
#include "util.h"
#include "clock.h"
#include "phys/rigid_body.h"

/*
class to manage the player's camera
*/
class Camera {
public:
	void init(float aspect);

    void update(float aspect);

	// TODO: add updateConfig()

	RigidBody* getBodyPtr();

	const std::string name_ = "CAMERA";
private:
	// camera config
	CameraConfig config_;

	// initial rotation
	float pitch_ = 0;
	float yaw_ = 0;

	// intial view matrix
	glm::mat4 view_ = glm::mat4(1);

	// physical repr.
	RigidBody body_;

	//glm::mat4 getRotM();

	//glm::mat4 getYawRotM();



};
