#pragma once

#include <chrono>
#include <string>

#include "util.h"

class Clock {
public:
	void init();

	float getProgramTime();

	const std::string name_ = "CLOCK";

private:
	std::chrono::time_point<std::chrono::high_resolution_clock> start_;

};