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

/*
class to manage the player's camera
*/
class Camera {
public:
	void init();


	const std::string name_ = "CAMERA";
private:


};