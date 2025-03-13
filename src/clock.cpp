#include "clock.h"

void Clock::init() {
	util::log(name_, "initializing clock");

	// set a start timepoint
	start_ = std::chrono::high_resolution_clock::now();
}

float Clock::getProgramTime() {
	auto timepoint = std::chrono::high_resolution_clock::now();
	return std::chrono::duration<float, std::chrono::seconds::period>(timepoint - start_).count();
}