#include "../header/physics.h"

// Helper-Function:
// Get as input x and y coordinates and checks if the coordinates are outside of the window
// N is the max size, i.e. here SIZE*SIZE
int check_bounds(int x, int y, int N);

void Physics::SetBnd(int b, std::array<float, SIZE*SIZE> x, int N) {
        for(int i = 1; i < N - 1; i++) {
            	x.at(check_bounds(i, 0, N)) = b == 2 ? -x.at(check_bounds(i, 1, N)) : x.at(check_bounds(i, 1, N));
            	x.at(check_bounds(i, N-1, N)) = b == 2 ? -x.at(check_bounds(i, N-2, N)) : x.at(check_bounds(i, N-2, N));
        }

        for(int j = 1; j < N - 1; j++) {
            	x.at(check_bounds(0, j, N)) = b == 1 ? -x.at(check_bounds(1, j, N)) : x.at(check_bounds(1, j, N));
            	x.at(check_bounds(N-1, j, N)) = b == 1 ? -x.at(check_bounds(N-2, j, N)) : x.at(check_bounds(N-2, j, N));
        }
    
    	x.at(check_bounds(0, 0, N)) = 0.33f * (x.at(check_bounds(1, 0, N))
                                  + x.at(check_bounds(0, 1, N))
                                  + x.at(check_bounds(0, 0, N)));
    	x.at(check_bounds(0, N-1, N)) = 0.33f * (x.at(check_bounds(1, N-1, N))
                                  + x.at(check_bounds(0, N-2, N))
                                  + x.at(check_bounds(0, N-1, N)));
    	x.at(check_bounds(N-1, 0, N)) = 0.33f * (x.at(check_bounds(N-2, 0, N))
                                  + x.at(check_bounds(N-1, 1, N))
                                  + x.at(check_bounds(N-1, 0, N)));
    	x.at(check_bounds(N-1, N-1, N)) = 0.33f * (x.at(check_bounds(N-2, N-1, N))
                                  + x.at(check_bounds(N-1, N-2, N))
                                  + x.at(check_bounds(N-1, N-1, N)));
}

void Physics::LinSolve(int b, std::array<float, SIZE*SIZE> x, std::array<float, SIZE*SIZE> x0, float a, float c, int iter, int N) {
	float cRecip = 1.0 / c;
    	for (int k = 0; k < iter; k++) {
        	for (int j = 1; j < N - 1; j++) {
				for (int i = 1; i < N - 1; i++) {
					x.at(check_bounds(i, j, N)) = (x0.at(check_bounds(i, j, N)) + a 
						*(x.at(check_bounds(i+1, j, N))
						+x.at(check_bounds(i-1, j, N))
						+x.at(check_bounds(i, j+1, N))
						+x.at(check_bounds(i, j-1, N))
						+x.at(check_bounds(i, j, N))
						+x.at(check_bounds(i, j, N))
                           		)) * cRecip;
                	}
		}
		SetBnd(b, x, N);
	}
}

void Physics::Diffuse(int b, std::array<float, SIZE*SIZE> x, std::array<float, SIZE*SIZE> x0, float diff, float dt, int iter, int N) {
	float a = dt * diff * (N - 2) * (N - 2);
	LinSolve(b, x, x0, a, 1 + 6 * a, iter, N);	
}

void Physics::Project(std::array<float, SIZE*SIZE> vx, std::array<float, SIZE*SIZE> vy, std::array<float, SIZE*SIZE> p, std::array<float, SIZE*SIZE> div, int iter, int N) {
        for (int j = 1; j < N - 1; j++) {
            	for (int i = 1; i < N - 1; i++) {
                	div.at(check_bounds(i, j, N)) = -0.5f*(
						vx.at(check_bounds(i+1, j, N))
						-vx.at(check_bounds(i-1, j, N))
	                	+vy.at(check_bounds(i, j+1, N))
						-vy.at(check_bounds(i, j-1, N))
                    	)/N;
                	p.at(check_bounds(i, j, N)) = 0;
            	}
        }

	SetBnd(0, div, N); 
	SetBnd(0, p, N);
	LinSolve(0, p, div, 1, 6, iter, N);
    
	for (int j = 1; j < N - 1; j++) {
		for (int i = 1; i < N - 1; i++) {
			vx.at(check_bounds(i, j, N)) -= 0.5f * (p.at(check_bounds(i+1, j, N)) - p.at(check_bounds(i-1, j, N))) * N;
			vy.at(check_bounds(i, j, N)) -= 0.5f * (p.at(check_bounds(i, j+1, N)) -p.at(check_bounds(i, j-1, N))) * N;
		}
	}
	SetBnd(1, vx, N);
	SetBnd(2, vy, N);
}

void Physics::Advect(int b, std::array<float, SIZE*SIZE> d, std::array<float, SIZE*SIZE> d0, std::array<float, SIZE*SIZE> vx, std::array<float, SIZE*SIZE> vy, float dt, int N) {
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
                	tmp1 = dtx * vx.at(check_bounds(i, j, N));
                	tmp2 = dty * vy.at(check_bounds(i, j, N));
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
                
			d.at(check_bounds(i, j, N)) = 
				s0 * (t0 * d0.at(check_bounds(i0i, j0i, N)) + t1 * d0.at(check_bounds(i0i, j1i, N))) +
				s1 * (t0 * d0.at(check_bounds(i1i, j0i, N)) + t1 * d0.at(check_bounds(i1i, j1i, N)));
            	}
        }
	SetBnd(b, d, N);
}

