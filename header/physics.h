#include "./const.h"
#include <array>
#include <cmath>


class Physics {
private:
	// Linear solver for the 2D Navier Stockes Equation
	void LinSolve(int b, std::array<float, SIZE*SIZE> x, std::array<float, SIZE*SIZE> x0, float a, float c, int iter, int N);
	void SetBnd(int b, std::array<float, SIZE*SIZE> x, int N);
public:
	void Diffuse(int b, std::array<float, SIZE*SIZE> x, std::array<float, SIZE*SIZE> x0, float diff, float dt, int iter, int N);
	void Project(std::array<float, SIZE*SIZE> vx, std::array<float, SIZE*SIZE> vy, std::array<float, SIZE*SIZE> p, std::array<float, SIZE*SIZE> div, int iter, int N);	
	void Advect(int b, std::array<float, SIZE*SIZE> d, std::array<float, SIZE*SIZE> d0, std::array<float, SIZE*SIZE> vx, std::array<float, SIZE*SIZE> vy, float dt, int N); 
};
