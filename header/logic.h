#ifndef LOGIC_H
#define LOGIC_H

#include<vector>
#include<SFML/Graphics.hpp>
#include "./physics.h"
#include "./const.h"
#include "./options.h"
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
	
	// Original Method
	void InitArr(float arr[], int size);
	float MapToRange(float value, float minIn, float maxIn, float minOut, float maxOut);
public:
	// Container();
	Logic(float dt, float diff, float visc);
	// ~Container();

	void AddDensity(float x, float y, float amount);
	void AddVelocity(float x, float y, float px, float py);
	void Step();
	void Render(sf::RenderWindow& win, Color color);
	void FadeDensity(int size);

	sf::Color Hsv(int hue, float sat, float val, float d);
};

#endif