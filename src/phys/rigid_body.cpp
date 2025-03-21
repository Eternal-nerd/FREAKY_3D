#include "rigid_body.h"

/*-----------------------------------------------------------------------------
------------------------------INITIALIZATION-----------------------------------
-----------------------------------------------------------------------------*/
void RigidBody::init() {
	util::log(name_, "initializing rigid body");

	yaw_ = 0.f;
	position_ = {0.f, 0.f, 0.f};
	linearVel_ = { 0.f, 0.f, 0.f };
	orientation_ = { 0.f,0.f,0.f };
	angleVel_ = { 0.f, 0.f, 0.f };
}

/*-----------------------------------------------------------------------------
-----------------------------UPDATE-STUFF--------------------------------------
-----------------------------------------------------------------------------*/
void RigidBody::update(float deltaT) {
	// first, update position (do more later?)
	glm::quat yawRotation = glm::angleAxis(yaw_, glm::vec3{ 0, -1, 0 });
	glm::mat4 yawRotM = glm::toMat4(yawRotation);
	glm::vec3 positionIncr = glm::vec3(yawRotM * glm::vec4(linearVel_ * deltaT, 0));
	position_ += positionIncr;
}

/*-----------------------------------------------------------------------------
-----------------------------SETTERS-------------------------------------------
-----------------------------------------------------------------------------*/
void RigidBody::setPosition(glm::vec3 position) {
	position_ = position;
}

void RigidBody::setOrientation(glm::vec3 orientation) {
	orientation_ = orientation;
}

void RigidBody::setVelocity(glm::vec3 velocity) {
	linearVel_ = velocity;
}

void RigidBody::setYaw(float yaw) {
	yaw_ = yaw;
}

/*-----------------------------------------------------------------------------
-----------------------------GETTERS-------------------------------------------
-----------------------------------------------------------------------------*/
glm::vec3 RigidBody::getPosition() {
	return position_;
}

glm::vec3 RigidBody::getVelocity() {
	return linearVel_;
}

glm::vec3 RigidBody::getOrientation() {
	return orientation_;
}