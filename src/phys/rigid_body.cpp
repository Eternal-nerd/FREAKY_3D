#include "rigid_body.h"

/*-----------------------------------------------------------------------------
------------------------------INITIALIZATION-----------------------------------
-----------------------------------------------------------------------------*/
void RigidBody::init() {
	util::log(name_, "initializing rigid body");

	glm::vec3 position_ = {0.f, 0.f, 0.f};
	glm::vec3 linearVel_ = { 0.f, 0.f, 0.f };
	glm::vec3 orientation_ = { 0.f,0.f,0.f };
	glm::vec3 angleVel_ = { 0.f, 0.f, 0.f };
}

/*-----------------------------------------------------------------------------
-----------------------------UPDATE-STUFF--------------------------------------
-----------------------------------------------------------------------------*/
void RigidBody::update(float deltaT) {
	// first, update position (do more later?)
	glm::vec3 positionIncr = linearVel_ * deltaT;

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