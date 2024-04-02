#include "../header/physics.h"

// Helper-Function:
// Get as input x and y coordinates and checks if the coordinates are outside of the window
// N is the max size, i.e. here SIZE*SIZE
int check_bounds(int x, int y, int N);

void Physics::setBnd(int b, std::array<float, SIZE*SIZE>& x, int N) {
        // If b == 2, assign the negative value of the second but last element to the element closest to the boundary 
		// on top and bottom of the simulation box. If b == 1, assign the positive value.
		for(int i = 1; i < N - 1; i++) {
            	x[check_bounds(i, 0, N)] = (b == 2 ? -x[check_bounds(i, 1, N)] : x[check_bounds(i, 1, N)]);
            	x[check_bounds(i, N-1, N)] = (b == 2 ? -x[check_bounds(i, N-2, N)] : x[check_bounds(i, N-2, N)]);
        }

		// If b == 1, assign the negative value of the second but last element to the element closest to the boundary 
		// on left and right of the simulation box. If b == 1, assign the positive value.
        for(int j = 1; j < N - 1; j++) {
            	x[check_bounds(0, j, N)] = (b == 1 ? -x[check_bounds(1, j, N)] : x[check_bounds(1, j, N)]);
            	x[check_bounds(N-1, j, N)] = (b == 1 ? -x[check_bounds(N-2, j, N)] : x[check_bounds(N-2, j, N)]);
        }
    
		// Assign to the edges the average over itself and its both neighbors
    	x[check_bounds(0, 0, N)] = 0.33f * (x[check_bounds(1, 0, N)]
                                  + x[check_bounds(0, 1, N)]
                                  + x[check_bounds(0, 0, N)]);
    	x[check_bounds(0, N-1, N)] = 0.33f * (x[check_bounds(1, N-1, N)]
                                  + x[check_bounds(0, N-2, N)]
                                  + x[check_bounds(0, N-1, N)]);
    	x[check_bounds(N-1, 0, N)] = 0.33f * (x[check_bounds(N-2, 0, N)]
                                  + x[check_bounds(N-1, 1, N)]
                                  + x[check_bounds(N-1, 0, N)]);
    	x[check_bounds(N-1, N-1, N)] = 0.33f * (x[check_bounds(N-2, N-1, N)]
                                  + x[check_bounds(N-1, N-2, N)]
                                  + x[check_bounds(N-1, N-1, N)]);
}

// Solve the differential euqation with the second order central difference method
void Physics::linSolve(int b, std::array<float, SIZE*SIZE>& x, std::array<float, SIZE*SIZE>& x0, float a, float c, int iter, int N) {
	float cRecip = 1.0 / c;
    	for (int k = 0; k < iter; k++) {
			// Iterate over inner area of the whole simulation box. 
			// Values of boundaries are assigned in method setBnd().
        	for (int j = 1; j < N - 1; j++) {
                	for (int i = 1; i < N - 1; i++) {
                    		x[check_bounds(i, j, N)] = (x0[check_bounds(i, j, N)] + a 
								*(x[check_bounds(i+1, j, N)]
                                	+x[check_bounds(i-1, j, N)]
                                	+x[check_bounds(i, j+1, N)]
                                	+x[check_bounds(i, j-1, N)]
                                	+x[check_bounds(i, j, N)]
                                	+x[check_bounds(i, j, N)]
                           		)) * cRecip;
                	}
		}
		setBnd(b, x, N);
	}
}

// Solve the diffusion process.
// Diffusion is the random movement of particles in the smoke
// The PDE is solved via the second order finite difference method
// a is the coefficient for x[i,j] in the discretization scheme
void Physics::diffuse(int b, std::array<float, SIZE*SIZE>& x, std::array<float, SIZE*SIZE>& x0, float diff, float dt, int iter, int N) {
	float a = dt * diff * (N - 2) * (N - 2);
	linSolve(b, x, x0, a, 1 + 6 * a, iter, N);	
}

void Physics::project(std::array<float, SIZE*SIZE>& vx, std::array<float, SIZE*SIZE>& vy, std::array<float, SIZE*SIZE>& p, std::array<float, SIZE*SIZE>& div, int iter, int N) {
	// Get the divergence via stencil matrix
	for (int j = 1; j < N - 1; j++) {
		for (int i = 1; i < N - 1; i++) {
			div[check_bounds(i, j, N)] = -0.5f*(
					vx[check_bounds(i+1, j, N)]
					-vx[check_bounds(i-1, j, N)]
					+vy[check_bounds(i, j+1, N)]
					-vy[check_bounds(i, j-1, N)]
				)/N;
			p[check_bounds(i, j, N)] = 0;
		}
	}

	// Set boundary condition for divergence
	setBnd(0, div, N); 
	// Set boundary condition for pressure
	setBnd(0, p, N);
	// Solve the PDE for the pressure distribution
	linSolve(0, p, div, 1, 6, iter, N);
    
	// Use the calculated results from the linSolve to get the velovities in x and y
	for (int j = 1; j < N - 1; j++) {
		for (int i = 1; i < N - 1; i++) {
			vx[check_bounds(i, j, N)] -= 0.5f * (p[check_bounds(i+1, j, N)] - p[check_bounds(i-1, j, N)]) * N;
			vy[check_bounds(i, j, N)] -= 0.5f * (p[check_bounds(i, j+1, N)] -p[check_bounds(i, j-1, N)]) * N;
		}
        }
		// Set boundary conditions for the velocity in x and y
    	setBnd(1, vx, N);
    	setBnd(2, vy, N);
}

void Physics::advect(int b, std::array<float, SIZE*SIZE>& d, std::array<float, SIZE*SIZE>& d0, std::array<float, SIZE*SIZE>& vx, std::array<float, SIZE*SIZE>& vy, float dt, int N) {
float i0, i1, j0, j1;
    
    	float dtx = dt * (N - 2);
    	float dty = dt * (N - 2);
    
    	float s0, s1, t0, t1;
    	float tmp1, tmp2, x, y;
    
    	float Nfloat = N;
    	float ifloat, jfloat;

    	int i, j;
    
	for(j = 1, jfloat = 1; j < N - 1; j++, jfloat++) { 
		for(i = 1, ifloat = 1; i < N - 1; i++, ifloat++) {
                	tmp1 = dtx * vx[check_bounds(i, j, N)];
                	tmp2 = dty * vy[check_bounds(i, j, N)];
                	x = ifloat - tmp1; 
                	y = jfloat - tmp2;
                
                	if(x < 0.5f) x = 0.5f; 
                	if(x > Nfloat + 0.5f) x = Nfloat + 0.5f; 
                	i0 = ::floorf(x); 
                	i1 = i0 + 1.0f;
                	if(y < 0.5f) y = 0.5f; 
                	if(y > Nfloat + 0.5f) y = Nfloat + 0.5f; 
                	j0 = ::floorf(y);
                	j1 = j0 + 1.0f; 

                	s1 = x - i0; 
                	s0 = 1.0f - s1; 
               		t1 = y - j0; 
                	t0 = 1.0f - t1;
                
                	int i0i = i0;
                	int i1i = i1;
                	int j0i = j0;
                	int j1i = j1;
                
			d[check_bounds(i, j, N)] = 
				s0 * (t0 * d0[check_bounds(i0i, j0i, N)] + t1 * d0[check_bounds(i0i, j1i, N)]) +
				s1 * (t0 * d0[check_bounds(i1i, j0i, N)] + t1 * d0[check_bounds(i1i, j1i, N)]);
            	}
        }
	setBnd(b, d, N);
}

