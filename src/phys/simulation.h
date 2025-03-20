#pragma once

#include <vector>
#include <string>

#include "../clock.h"
#include "../util.h"
#include "rigid_body.h"

/*
a class where the collision detection stuff resides
operates on rigid bodies and joints?
*/
class Simulation {
public:
	void init(Clock& clock);

	void registerBody(RigidBody* body);

	void stepSimulation();

	const std::string name_ = "SIMULATION";
private:
	Clock* clock_ = nullptr;
	float lastUpdate_ = 0.f;

	int bodyCount_ = 0;
	std::vector<RigidBody*> bodies_{};

};
