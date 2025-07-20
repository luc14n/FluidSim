#pragma once
#include "FluidGrid.h"
#include "FluidParticle.h"

class FluidSolver {
public:
	FluidSolver(FluidGrid& grid, std::vector<FluidParticle>& particles);
	void step(float dt);

	// Add more methods for boundary conditions, etc.
private:
	FluidGrid& grid;
	std::vector<FluidParticle>& particles;
};