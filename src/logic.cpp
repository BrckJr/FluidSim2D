#include "../header/logic.h"
#include <iostream>

// Helper-Function:
// Get as input x and y coordinates and checks if the coordinates are outside of the window
// N is the max size, i.e. here SIZE*SIZE
int check_bounds(int x, int y, int N);


Logic::Logic(float dt, float diff, float visc) {
	// Get size from global variable SIZE to set correct array sizes
	_size = SIZE;
	
	// Time step of recalulcation of the fluid movement
	// The smaller _dt, the "slower" the rendering
	_dt = dt;

	// Speed of diffusion 
	_diff = diff;
	
	// Set viscosity of fluid
	_viscosity = visc;

	// Setting the initial values of the arrays to zero
	std::fill(_velocity_x.begin(), _velocity_x.end(), 0);
	std::fill(_velocity_y.begin(), _velocity_y.end(), 0);
	std::fill(_d_velocity_x.begin(), _d_velocity_x.end(), 0);
	std::fill(_d_velocity_y.begin(), _d_velocity_y.end(), 0);
	std::fill(_previousDensity.begin(), _previousDensity.end(), 0);
	std::fill(_density.begin(), _density.end(), 0);
}
	
// Add density at the spot where the mouse is clicked
void Logic::addDensity(float x, float y, float amount) {
	_density[check_bounds(x,y,_size)] += amount;
}

// Add velocity to the fluid elements where the mouse is dragged over
void Logic::addVelocity(float x, float y, float px, float py) {
	int index = check_bounds(x,y,_size);
	_velocity_x[index] += px;
	_velocity_y[index] += py;
}

// Make one simulation step by solving the differential equation
// Diffuse and advect always in both directions
void Logic::step() {
	_physics.diffuse(1, _d_velocity_x, _velocity_x, _viscosity, _dt, 16, _size);	
	_physics.diffuse(2, _d_velocity_y, _velocity_y, _viscosity, _dt, 16, _size);	

	_physics.project(_d_velocity_x, _d_velocity_y, _velocity_x, _velocity_y, 16, _size);
	
	_physics.advect(1, _velocity_x, _d_velocity_x, _d_velocity_x, _d_velocity_y, _dt, _size);
	_physics.advect(2, _velocity_y, _d_velocity_y, _d_velocity_x, _d_velocity_y, _dt, _size);

	_physics.project(_velocity_x, _velocity_y, _d_velocity_x, _d_velocity_y, 16, _size);

	_physics.diffuse(0, _previousDensity, _density, _diff, _dt, 16, _size);	
	_physics.advect(0, _density, _previousDensity, _velocity_x, _velocity_y, _dt, _size);
}

// Refreshe the image displayed during simulation
void Logic::render(sf::RenderWindow& win) {
	win.clear();
	for (int i = 0; i < _size; i++) {
		for(int j = 0; j < _size; j++) {
			sf::RectangleShape rect;
			rect.setSize(sf::Vector2f(SCALE, SCALE));
			rect.setPosition(j * SCALE, i * SCALE);
			rect.setFillColor(sf::Color(0, 255, 0, (_density[check_bounds(i,j,_size)] > 255) ? 255 : _density[check_bounds(i,j,_size)]));
			win.draw(rect);
		}
	}
}

// Let's the density fade over time so to not fill the complete image
// UNPHYSICAL BEHAVIOR!!
void Logic::fadeDensity(int size) {
	for (int i = 0; i < size; i++) {
		float d = _density[i];
		_density[i] = (d - 0.05f < 0) ? 0 : d - 0.05f; 
	}	
}
