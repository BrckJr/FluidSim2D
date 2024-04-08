#include "./const.h"
#include <array>
#include <cmath>
#include <iostream>


class Physics {
private:
	// Ensure that no smoke can exist the simulation box. 
	// Set horizontal density to zero on vertical walls, analog on horizontal walls
	void setBnd(int b, std::array<float, SIZE*SIZE>& x, int N, bool block_present);
public:
	// Viscous diffusion of the velocity field in x and y direction according to the Navier Stokes PDE
	void diffuse_velocity(int b, std::array<float, SIZE*SIZE>& x, std::array<float, SIZE*SIZE>& x0, float diff, float dt, int iter, int N);
	
	// Diffuse the smoke density according to the smoke density PDE (see Jos Stam, Figure 1, equation 2)
	void diffuse_density(int b, std::array<float, SIZE*SIZE>& x, std::array<float, SIZE*SIZE>& x0, float diff, float dt, int iter, int N);
	
	// Force mass conservation and ensure that the velocity field remains divergence-free.
	void project(std::array<float, SIZE*SIZE>& vx, std::array<float, SIZE*SIZE>& vy, std::array<float, SIZE*SIZE>& p, std::array<float, SIZE*SIZE>& div, int N);
	
	// Self-Advection of the velocity in x and y direction according to the Navier Stokes PDE
	// Self-Advection is the movement of the velocity field along itself
	void advect_velocity(int b, std::array<float, SIZE*SIZE>& d, std::array<float, SIZE*SIZE>& d0, std::array<float, SIZE*SIZE>& vx, std::array<float, SIZE*SIZE>& vy, float dt, int N); 
	
	// Advection of the smoke in the velocity field according to the smoke density PDE (see Jos Stam, Figure 1, equation 2)
	void advect_density(int b, std::array<float, SIZE*SIZE>& d, std::array<float, SIZE*SIZE>& d0, std::array<float, SIZE*SIZE>& vx, std::array<float, SIZE*SIZE>& vy, float dt, int N); 
};
