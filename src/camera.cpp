#include "camera.h"

/*-----------------------------------------------------------------------------
------------------------------INITIALIZATION-----------------------------------
-----------------------------------------------------------------------------*/
void Camera::init(float aspect) {
	util::log(name_, "initializing camera");

	// set intial rotation
	pitch_ = 0;
	yaw_ = 0;

	// set camera config:
	config_.fovy = 90;
	config_.near = 0.1f;
	config_.far = 2000.f;

	config_.perspectiveM = glm::perspective(glm::radians(config_.fovy), aspect, config_.near, config_.far);
	// invert the Y direction on projection matrix ???
	config_.perspectiveM[1][1] *= -1;

	// init physical body
	body_.init();
	body_.setPosition(0.f, 0.f, 5.f);

	// init view matrix
	view_ = glm::translate(glm::mat4(1), body_.getPosition());

}


/*-----------------------------------------------------------------------------
-----------------------------UPDATE-STUFF--------------------------------------
-----------------------------------------------------------------------------*/
void Camera::update(float aspect) {
	glm::mat4 transM = glm::translate(glm::mat4(1), body_.getPosition());
	glm::mat4 rotM = getRotationMat();

	// apply updates to view matrix
	view_ = glm::inverse(transM * rotM);

	config_.perspectiveM = glm::perspective(glm::radians(config_.fovy), aspect, config_.near, config_.far);
	config_.perspectiveM[1][1] *= -1;
}

void Camera::incrementYaw(float increment) {
	yaw_ += increment;
}

void Camera::incrementPitch(float increment) {
	// limit pitch to +-90 degrees
	if (abs(pitch_ - increment) < glm::radians(90.0f)) {
		pitch_ -= increment;
	}
}



/*-----------------------------------------------------------------------------
-----------------------------GETTERS-------------------------------------------
-----------------------------------------------------------------------------*/
const glm::mat4& Camera::getViewMatrix() const {
	return view_;
}

const glm::mat4& Camera::getPerspectiveMatrix() const {
	return config_.perspectiveM;
}

RigidBody* Camera::getBodyPtr() { return &body_; }

/*-----------------------------------------------------------------------------
-----------------------------HELPERS-------------------------------------------
-----------------------------------------------------------------------------*/
glm::mat4 Camera::getRotationMat() {
	glm::quat pitchRotation = glm::angleAxis(pitch_, glm::vec3{ 1, 0, 0 });
	glm::quat yawRotation = glm::angleAxis(yaw_, glm::vec3{ 0, -1, 0 });

	return glm::toMat4(yawRotation) * glm::toMat4(pitchRotation);
}
