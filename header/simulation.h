#include<vector>

#include "./logic.h"

class Simulation {
	private:
		// Window for displaying the animation
		sf::RenderWindow window;
		
		// Creates the logic behind the animation
		Logic logic;

		// Creates option for color selection with keystroke 'c' during runtime
		Options options;
	
	public:
		// Constructor for initialization of window, container, options 
		Simulation();
		
		// Method for running the simulation
		void run();
};
