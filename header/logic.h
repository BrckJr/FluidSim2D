#ifndef LOGIC_H
#define LOGIC_H

#include<vector>
#include<SFML/Graphics.hpp>
#include "./physics.h"
#include "./const.h"
#include <array>

// This class makes the computation of the fluid behaviour
class Logic {
	private:
		// Creating a physics attribute to get the correct physical behaviour
		Physics _physics;

		// Size attribute which defines the number of grid elements in each direction (x and y are both equal)
		int _size;

		// Time step
		float _dt;

		// Diffusion coefficient
		float _diff;
		
		// Viscosity attribute
		float _viscosity;

		// Velocity in x and y
		std::array<float, SIZE*SIZE> _velocity_x;
		std::array<float, SIZE*SIZE> _velocity_y;

		// Change of velocity in x and y direction
		std::array<float, SIZE*SIZE> _d_velocity_x;
		std::array<float, SIZE*SIZE> _d_velocity_y;

		// Density and previous density at a given spot
		std::array<float, SIZE*SIZE> _previousDensity;
		// float _density[SIZE*SIZE];
		std::array<float, SIZE*SIZE> _density;
			
	public:
		// Constructor
		Logic(float dt, float diff, float visc);

		// Adds density at the spot where the mouse is clicked
		void addDensity(float x, float y, float amount);
		
		// Adds velocity to the fluid elements where the mouse is dragged over
		void addVelocity(float x, float y, float px, float py);
		
		// Makes one simulation step by solving the physical behaviour 
		void step();
		
		// Refreshe the image displayed during simulation
		void render(sf::RenderWindow& win);
		
		// Let the density fade over time so to not fill the complete image
		// UNPHYSICAL BEHAVIOR!!
		void fadeDensity(int size);
};

#endif