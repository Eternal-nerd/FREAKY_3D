#include "simulation.h"

/*-----------------------------------------------------------------------------
------------------------------INITIALIZATION-----------------------------------
-----------------------------------------------------------------------------*/
void Simulation::init(Clock& clock) {
	util::log(name_, "initializing simulation");

	clock_ = &clock;
}

void Simulation::registerBody(RigidBody* body) {
	util::log(name_, "registering rigid body");
	bodies_.push_back(body);
	bodyCount_++;
}

/*-----------------------------------------------------------------------------
------------------------------UPDATE-STUFF-------------------------------------
-----------------------------------------------------------------------------*/
void Simulation::stepSimulation() {
	// get time difference from last update
	float delta = clock_->getProgramTime() - lastUpdate_;
	// TODO FIND COLLISIONS, apply gravity, etc. etc. etc.

	// for now, just calculate new positions based on linear velocity
	for (int i = 0; i < bodyCount_; i++) {
		bodies_[i]->update(delta);
	}


	lastUpdate_ = clock_->getProgramTime();
}