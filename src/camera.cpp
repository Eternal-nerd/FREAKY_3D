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
	RigidBodyProperties p;
	p.position = { 0.f, 2.f, 2.f };
	body_.init(p);

	// init view matrix
	view_ = glm::translate(glm::mat4(1), body_.getPosition());

}


/*-----------------------------------------------------------------------------
-----------------------------UPDATE-STUFF--------------------------------------
-----------------------------------------------------------------------------*/
void Camera::update(float aspect) {


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
