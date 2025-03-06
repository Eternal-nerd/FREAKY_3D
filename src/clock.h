#pragma once

#include <chrono>

class Clock {
public:
	void init();

private:
	std::chrono::time_point<std::chrono::high_resolution_clock> start_;

};