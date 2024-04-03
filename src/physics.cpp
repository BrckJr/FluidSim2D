#include "../header/physics.h"

// Helper-Function:
// Get as input x and y coordinates and translates it into the coordinates for the 1D array
// N is the size of the whole array, i.e. here SIZE*SIZE
int transform_coordinates(int x, int y, int N);

// Ensure that no smoke can exist the simulation box. 
// Set horizontal density to zero on vertical walls, analog on horizontal walls
void Physics::setBnd(int b, std::array<float, SIZE*SIZE>& x, int N) {
        // If b == 2, assign the negative value of the second but last element to the element closest to the boundary 
		// on top and bottom of the simulation box. This is called in the diffusion / advection for the y-axis.
		for(int i = 1; i < N - 1; i++) {
            	x[transform_coordinates(i, 0, N)] = (b == 2 ? -x[transform_coordinates(i, 1, N)] : x[transform_coordinates(i, 1, N)]);
            	x[transform_coordinates(i, N-1, N)] = (b == 2 ? -x[transform_coordinates(i, N-2, N)] : x[transform_coordinates(i, N-2, N)]);
        }

		// If b == 1, assign the negative value of the second but last element to the element closest to the boundary 
		// on left and right of the simulation box. This is called in the diffusion / advection for the x-axis.
        for(int j = 1; j < N - 1; j++) {
            	x[transform_coordinates(0, j, N)] = (b == 1 ? -x[transform_coordinates(1, j, N)] : x[transform_coordinates(1, j, N)]);
            	x[transform_coordinates(N-1, j, N)] = (b == 1 ? -x[transform_coordinates(N-2, j, N)] : x[transform_coordinates(N-2, j, N)]);
        }
    
		// Assign to the edges the average over itself and its both neighbors
    	x[transform_coordinates(0, 0, N)] = 0.33f * (x[transform_coordinates(1, 0, N)]
                                  + x[transform_coordinates(0, 1, N)]
                                  + x[transform_coordinates(0, 0, N)]);
    	x[transform_coordinates(0, N-1, N)] = 0.33f * (x[transform_coordinates(1, N-1, N)]
                                  + x[transform_coordinates(0, N-2, N)]
                                  + x[transform_coordinates(0, N-1, N)]);
    	x[transform_coordinates(N-1, 0, N)] = 0.33f * (x[transform_coordinates(N-2, 0, N)]
                                  + x[transform_coordinates(N-1, 1, N)]
                                  + x[transform_coordinates(N-1, 0, N)]);
    	x[transform_coordinates(N-1, N-1, N)] = 0.33f * (x[transform_coordinates(N-2, N-1, N)]
                                  + x[transform_coordinates(N-1, N-2, N)]
                                  + x[transform_coordinates(N-1, N-1, N)]);
}

// Viscous diffusion of the velocity field in x and y direction according to the Navier Stokes PDE
void Physics::diffuse_velocity(int b, std::array<float, SIZE*SIZE>& velocity, std::array<float, SIZE*SIZE>& previous_velocity, float diffusion_coefficient, float dt, int number_of_iterations, int N) {
	float a = dt * diffusion_coefficient * (N - 2) * (N - 2);
	// For the sake of inmproved speed, keep number of iterations low
	for (int k = 0; k < number_of_iterations; k++) {
		// Solve the implicit discretization of the density with the Gauss Seidel Solver
		for (int j = 1; j < N - 1; j++) {
				for (int i = 1; i < N - 1; i++) {
						velocity[transform_coordinates(i, j, N)] = (previous_velocity[transform_coordinates(i, j, N)] + a 
							*(velocity[transform_coordinates(i+1, j, N)]
								+velocity[transform_coordinates(i-1, j, N)]
								+velocity[transform_coordinates(i, j+1, N)]
								+velocity[transform_coordinates(i, j-1, N)]
							)) / (1 + 4 * a);
				}
		}
		// Set the boundary conditions for the velocities.
		setBnd(b, velocity, N);
	}
}

// Diffuse the smoke density according to the smoke density PDE (see Jos Stam, Figure 1, equation 2)
void Physics::diffuse_density(int b, std::array<float, SIZE*SIZE>& density, std::array<float, SIZE*SIZE>& previous_density, float diffusion_coefficient, float dt, int number_of_iterations, int N) {
	float a = dt * diffusion_coefficient * (N - 2) * (N - 2);
	// For the sake of inmproved speed, keep number of iterations low
	for (int k = 0; k < number_of_iterations; k++) {
		// Solve the implicit discretization of the density with the Gauss Seidel Solver
		for (int j = 1; j < N - 1; j++) {
				for (int i = 1; i < N - 1; i++) {
						density[transform_coordinates(i, j, N)] = (previous_density[transform_coordinates(i, j, N)] + a 
							*(density[transform_coordinates(i+1, j, N)]
								+density[transform_coordinates(i-1, j, N)]
								+density[transform_coordinates(i, j+1, N)]
								+density[transform_coordinates(i, j-1, N)]
							)) / (1 + 4 * a);
				}
		}
		// Set the boundary conditions for the velocities.
		setBnd(b, density, N);
	}
}

// Self-Advection of the velocity in x and y direction according to the Navier Stokes PDE
// Self-Advection is the movement of the velocity field along itself
void Physics::advect_velocity(int b, std::array<float, SIZE*SIZE>& velocity, std::array<float, SIZE*SIZE>& previous_velocity, std::array<float, SIZE*SIZE>& previous_velocity_x, std::array<float, SIZE*SIZE>& previous_velocity_y, float dt, int N) {
// Interpolation coordinates
	float i0, i1, j0, j1;
    
	// Calculate the time step size in x and y directions
	float dtx = dt * (N - 2);
	float dty = dt * (N - 2);

	// Interpolation weights
	float s0, s1, t0, t1;

	// Temporary variables
	float tmp1, tmp2, x, y;

	// Convert integer N, i and j to float for calculations purpose
	float Nfloat = N;
    
	for(int j = 1; j < N - 1; j++) { 
		for(int i = 1; i < N - 1; i++) {
				// Calculate the interpolated in x and y position
				x = i - dtx * previous_velocity_x[transform_coordinates(i, j, N)];
				y = j -  dty * previous_velocity_y[transform_coordinates(i, j, N)];
			
				// Check if interpoland is within grid boundaries
				if(x < 0.5f) x = 0.5f; 
				if(x > Nfloat + 0.5f) x = Nfloat + 0.5f; 
				i0 = ::floorf(x); 
				i1 = i0 + 1.0f;
				if(y < 0.5f) y = 0.5f; 
				if(y > Nfloat + 0.5f) y = Nfloat + 0.5f; 
				j0 = ::floorf(y);
				j1 = j0 + 1.0f; 

	            // Calculate the interpolation weights
				s1 = x - i0; 
				s0 = 1.0f - s1; 
				t1 = y - j0; 
				t0 = 1.0f - t1;
			
				// Convert the interpolated positions to integer indices
				int i0i = i0;
				int i1i = i1;
				int j0i = j0;
				int j1i = j1;
			
			// Update the density according to interpolation
			velocity[transform_coordinates(i, j, N)] = 
				s0 * (t0 * previous_velocity[transform_coordinates(i0i, j0i, N)] + t1 * previous_velocity[transform_coordinates(i0i, j1i, N)]) +
				s1 * (t0 * previous_velocity[transform_coordinates(i1i, j0i, N)] + t1 * previous_velocity[transform_coordinates(i1i, j1i, N)]);
		}
	}
	setBnd(b, velocity, N);
}

// Advection of the smoke in the velocity field according to the smoke density PDE (see Jos Stam, Figure 1, equation 2)
void Physics::advect_density(int b, std::array<float, SIZE*SIZE>& density, std::array<float, SIZE*SIZE>& previous_density, std::array<float, SIZE*SIZE>& velocity_x, std::array<float, SIZE*SIZE>& velocity_y, float dt, int N) {
	// Interpolation coordinates
	float i0, i1, j0, j1;
    
	// Calculate the time step size in x and y directions
	float dtx = dt * (N - 2);
	float dty = dt * (N - 2);

	// Interpolation weights
	float s0, s1, t0, t1;

	// Temporary variables
	float tmp1, tmp2, x, y;

	// Convert integer N, i and j to float for calculations purpose
	float Nfloat = N;
    
	for(int j = 1; j < N - 1; j++) { 
		for(int i = 1; i < N - 1; i++) {
				// Calculate the interpolated in x and y position
				x = i - dtx * velocity_x[transform_coordinates(i, j, N)];
				y = j-  dty * velocity_y[transform_coordinates(i, j, N)];
			
				// Check if interpoland is within grid boundaries
				if(x < 0.5f) x = 0.5f; 
				if(x > Nfloat + 0.5f) x = Nfloat + 0.5f; 
				i0 = ::floorf(x); 
				i1 = i0 + 1.0f;
				if(y < 0.5f) y = 0.5f; 
				if(y > Nfloat + 0.5f) y = Nfloat + 0.5f; 
				j0 = ::floorf(y);
				j1 = j0 + 1.0f; 

	            // Calculate the interpolation weights
				s1 = x - i0; 
				s0 = 1.0f - s1; 
				t1 = y - j0; 
				t0 = 1.0f - t1;
			
				// Convert the interpolated positions to integer indices
				int i0i = i0;
				int i1i = i1;
				int j0i = j0;
				int j1i = j1;
			
			// Update the density according to interpolation
			density[transform_coordinates(i, j, N)] = 
				s0 * (t0 * previous_density[transform_coordinates(i0i, j0i, N)] + t1 * previous_density[transform_coordinates(i0i, j1i, N)]) +
				s1 * (t0 * previous_density[transform_coordinates(i1i, j0i, N)] + t1 * previous_density[transform_coordinates(i1i, j1i, N)]);
		}
	}
	setBnd(b, density, N);
}

// Force mass conservation and ensure that the velocity field remains divergence-free.
void Physics::project(std::array<float, SIZE*SIZE>& vx, std::array<float, SIZE*SIZE>& vy, std::array<float, SIZE*SIZE>& p, std::array<float, SIZE*SIZE>& div, int N) {
	
	// Get the divergence via stencil matrix
	float h = 1 / N;
	for (int j = 1; j < N - 1; j++) {
		for (int i = 1; i < N - 1; i++) {
			div[transform_coordinates(i, j, N)] = -0.5f*h*(
					vx[transform_coordinates(i+1, j, N)]
					-vx[transform_coordinates(i-1, j, N)]
					+vy[transform_coordinates(i, j+1, N)]
					-vy[transform_coordinates(i, j-1, N)]
				)/N;
			
			// Initialize the pressure array
			p[transform_coordinates(i, j, N)] = 0;
		}
	}

	// Set boundary condition for divergence
	setBnd(0, div, N); 
	// Reset boundary condition
	setBnd(0, p, N);

	// Solve the PDE for the pressure distribution   
	for (int j = 1; j < N - 1; j++) {
		for (int i = 1; i < N - 1; i++) { 
				p[transform_coordinates(i, j, N)] = (div[transform_coordinates(i, j, N)] +
					(p[transform_coordinates(i+1, j, N)]
						+p[transform_coordinates(i-1, j, N)]
						+p[transform_coordinates(i, j+1, N)]
						+p[transform_coordinates(i, j-1, N)]
					))/4;
		}
		// Set boundary condition for pressure inside loop to be consistent with boundary conditions
		setBnd(0, p, N);
	}
	
	// Use the calculated results from the pressure discretization to get the velovities in x and y
	for (int j = 1; j < N - 1; j++) {
		for (int i = 1; i < N - 1; i++) {
			vx[transform_coordinates(i, j, N)] -= 0.5f * (p[transform_coordinates(i+1, j, N)] - p[transform_coordinates(i-1, j, N)]) * N;
			vy[transform_coordinates(i, j, N)] -= 0.5f * (p[transform_coordinates(i, j+1, N)] - p[transform_coordinates(i, j-1, N)]) * N;
		}
        }
		// Set boundary conditions for the density in x and y
    	setBnd(1, vx, N);
    	setBnd(2, vy, N);
}


