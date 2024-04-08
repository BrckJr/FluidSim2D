#include "../header/logic.h"
#include <iostream>

// Helper-Function:
// Get as input x and y coordinates and checks if the coordinates are outside of the window
// N is the max size, i.e. here SIZE*SIZE
int transform_coordinates(int x, int y, int N);

Logic::Logic(float dt, float diff, float visc) {
	// Get size from global variable SIZE to set correct array sizes
	_size = SIZE;
	
	// Time step of recalulcation of the fluid movement
	// The smaller _dt, the "slower" the rendering
	_dt = dt;

	// Speed of diffusion 
	_diffusion_coefficient = diff;
	
	// Set viscosity of fluid
	_viscosity = visc;

	// Setting the initial values of the arrays to zero
	std::fill(_velocity_x.begin(), _velocity_x.end(), 0);
	std::fill(_velocity_y.begin(), _velocity_y.end(), 0);
	std::fill(_previous_velocity_x.begin(), _previous_velocity_x.end(), 0);
	std::fill(_previous_velocity_y.begin(), _previous_velocity_y.end(), 0);
	std::fill(_previous_density.begin(), _previous_density.end(), 0);
	std::fill(_density.begin(), _density.end(), 0);
}
	
// Addition of density in the density field at the spot where the mouse is clicked
// This represents the third term in the smoke density PDE (see Jos Stam, Figure 1, equation 2)
void Logic::addDensity(float x, float y, float amount) {
	_density[transform_coordinates(x,y,_size)] += amount;
}

// Increase of velocity in the velocity field in x and y direction due to external force (mouse movement)
void Logic::addVelocity(float x, float y, float px, float py) {
	int index = transform_coordinates(x,y,_size);
	_velocity_x[index] += px;
	_velocity_y[index] += py;
}

// Make one simulation step by solving the differential equation
// Diffuse and advect always in both directions
void Logic::step() {
	// Viscous diffusion of the velocity field in x and y direction according to the Navier Stokes PDE
	_physics.diffuse_velocity(1, _previous_velocity_x, _velocity_x, _viscosity, _dt, 16, _size);	
	_physics.diffuse_velocity(2, _previous_velocity_y, _velocity_y, _viscosity, _dt, 16, _size);	

	// Force mass conservation and ensure that the velocity field remains divergence-free.
	_physics.project(_previous_velocity_x, _previous_velocity_y, _velocity_x, _velocity_y, _size);
	
	// Self-Advection of the velocity in x and y direction according to the Navier Stokes PDE
	// Self-Advection is the movement of the velocity field along itself
	_physics.advect_velocity(1, _velocity_x, _previous_velocity_x, _previous_velocity_x, _previous_velocity_y, _dt, _size);
	_physics.advect_velocity(2, _velocity_y, _previous_velocity_y, _previous_velocity_x, _previous_velocity_y, _dt, _size);

	// Force mass conservation and ensure that the velocity field remains divergence-free.
	_physics.project(_velocity_x, _velocity_y, _previous_velocity_x, _previous_velocity_y, _size);

	// Diffuse the smoke density according to the smoke density PDE (see Jos Stam, Figure 1, equation 2)
	_physics.diffuse_density(0, _previous_density, _density, _diffusion_coefficient, _dt, 16, _size);	
	
	// Advection of the smoke in the velocity field according to the smoke density PDE (see Jos Stam, Figure 1, equation 2)
	_physics.advect_density(0, _density, _previous_density, _velocity_x, _velocity_y, _dt, _size);
}

// Refresh the image displayed during simulation
void Logic::render(sf::RenderWindow& win) {
	// Create a rectangle shape for the block where no smoke can pass with a white frame
    sf::RectangleShape square(sf::Vector2f(SCALE*BLOCK_WIDTH, SCALE*BLOCK_HEIGHT));
    square.setPosition(SCALE*(SIZE - BLOCK_WIDTH)/2 - SCALE*0.05, SCALE*(SIZE - BLOCK_HEIGHT)/2 - SCALE*0.05);
    square.setFillColor(sf::Color(192, 192, 192));
    square.setOutlineThickness(2.f);
    square.setOutlineColor(sf::Color::White);
	


	win.clear();
	for (int i = 0; i < _size; i++) {
		for(int j = 0; j < _size; j++) {
			sf::RectangleShape rect;
			rect.setSize(sf::Vector2f(SCALE, SCALE));
			rect.setPosition(j * SCALE, i * SCALE);
			rect.setFillColor(sf::Color(0, 255, 0, (_density[transform_coordinates(i,j,_size)] > 255) ? 255 : _density[transform_coordinates(i,j,_size)]));
			win.draw(rect);
			win.draw(square);
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
