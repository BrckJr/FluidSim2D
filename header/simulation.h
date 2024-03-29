#include<vector>
#include<iostream>
#include "./logic.h"

class Simulation {
	private:
		// Window for displaying the animation
		sf::RenderWindow window;
		
		// Creates the logic behind the animation
		Logic logic;
	
	public:
		// Constructor for initialization of window, container, options 
		Simulation();
		
		// Method for running the simulation
		void run();
};
