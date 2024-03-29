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

void Logic::AddDensity(float x, float y, float amount) {
	_density.at(check_bounds(x,y,_size)) += amount;
}

void Logic::AddVelocity(float x, float y, float px, float py) {
	int index = check_bounds(x,y,_size);

	_velocity_x.at(index) += px;
	_velocity_y.at(index) += py;
}

void Logic::Step() {
	_physics.Diffuse(1, _d_velocity_x, _velocity_x, _viscosity, _dt, 16, _size);	
	_physics.Diffuse(2, _d_velocity_y, _velocity_y, _viscosity, _dt, 16, _size);	

	_physics.Project(_d_velocity_x, _d_velocity_y, _velocity_x, _velocity_y, 16, _size);
	
	_physics.Advect(1, _velocity_x, _d_velocity_x, _d_velocity_x, _d_velocity_y, _dt, _size);
	_physics.Advect(2, _velocity_y, _d_velocity_y, _d_velocity_x, _d_velocity_y, _dt, _size);

	_physics.Project(_velocity_x, _velocity_y, _d_velocity_x, _d_velocity_y, 16, _size);

	_physics.Diffuse(0, _previousDensity, _density, _diff, _dt, 16, _size);	
	_physics.Advect(0, _density, _previousDensity, _velocity_x, _velocity_y, _dt, _size);
}

sf::Color Logic::Hsv(int hue, float sat, float val, float d) {
	hue %= 360;
  	while(hue<0) hue += 360;

  	if(sat<0.f) sat = 0.f;
  	if(sat>1.f) sat = 1.f;

  	if(val<0.f) val = 0.f;
  	if(val>1.f) val = 1.f;

  	int h = hue/60;
  	float f = float(hue)/60-h;
  	float p = val*(1.f-sat);
  	float q = val*(1.f-sat*f);
  	float t = val*(1.f-sat*(1-f));

  	switch(h) {
    		default:
   	 	case 0:
    		case 6: return sf::Color(val*255, t*255, p*255, d);
    		case 1: return sf::Color(q*255, val*255, p*255, d);
    		case 2: return sf::Color(p*255, val*255, t*255, d);
    		case 3: return sf::Color(p*255, q*255, val*255, d);
    		case 4: return sf::Color(t*255, p*255, val*255, d);
    		case 5: return sf::Color(val*255, p*255, q*255, d);
  	}
}

float Logic::MapToRange(float val, float minIn, float maxIn, float minOut, float maxOut) {
	float x = (val - minIn) / (maxIn - minIn);
	float result = minOut + (maxOut - minOut) * x;
	return (result < minOut) ? minOut : (result > maxOut) ? maxOut : result;
}

void Logic::Render(sf::RenderWindow& win, Color color) {
	win.clear();
	for (int i = 0; i < _size; i++) {
		for(int j = 0; j < _size; j++) {
			sf::RectangleShape rect;
			rect.setSize(sf::Vector2f(SCALE, SCALE));
			rect.setPosition(j * SCALE, i * SCALE);
			
			switch (color) {
				case Color::Default:
					rect.setFillColor(sf::Color(255, 255, 255, (_density.at(check_bounds(i,j,_size)) > 255) ? 255 : _density.at(check_bounds(i,j,_size))));
					break;
				case Color::Hsb:
					rect.setFillColor(Hsv((_density.at(check_bounds(i,j,_size))), 1, 1, 255));
					break;
				case Color::Velocity: {
						int r = (int)MapToRange(_velocity_x.at(check_bounds(i,j,_size)), -0.05f, 0.05f, 0, 255);
						int g = (int)MapToRange(_velocity_y.at(check_bounds(i,j,_size)), -0.05f, 0.05f, 0, 255);
						rect.setFillColor(sf::Color(r, g, 255));
						break;
					}
				default:
					break;
			};	

			win.draw(rect);
		}
	}
}

void Logic::FadeDensity(int size) {
	for (int i = 0; i < size; i++) {
		float d = _density.at(i);
		_density.at(i) = (d - 0.05f < 0) ? 0 : d - 0.05f; 
		std::cout << _density.at(i) << '\n';
	}	
}
