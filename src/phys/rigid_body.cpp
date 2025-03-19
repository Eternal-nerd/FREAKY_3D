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
void RigidBody::update() {

}

void RigidBody::setPosition(float x, float y, float z) {
	position_.x = x;
	position_.y = y;
	position_.z = z;
}

void RigidBody::setOrientation(float x, float y, float z) {
	orientation_.x = x;
	orientation_.y = y;
	orientation_.z = z;
}

/*-----------------------------------------------------------------------------
-----------------------------GETTERS-------------------------------------------
-----------------------------------------------------------------------------*/
glm::vec3 RigidBody::getPosition() {
	return position_;
}

glm::vec3 RigidBody::getOrientation() {
	return orientation_;
}