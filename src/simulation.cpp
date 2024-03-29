#include "../header/simulation.h"

// Creating a window where we run the simulation
// Logic(_dt, _diff, _viscosity)
Simulation::Simulation() : logic(Logic(0.1f, 0.0f, 0.0005f)) {
	window.create(sf::VideoMode(SIZE*SCALE, SIZE*SCALE), "Euler fluid simulation", sf::Style::Titlebar | sf::Style::Close);
}

// Running the simulation
void Simulation::run() {
	// Checks the current mouse position and safes it in previousMouse and currentMouse
	// for comparison with mouse movement later on
	sf::Vector2i previousMouse = sf::Mouse::getPosition(window);
	sf::Vector2i currentMouse = sf::Mouse::getPosition(window);

	while (window.isOpen()) {
		sf::Event e;
		while (window.pollEvent(e)) {
			switch (e.type) {
				// Makes sure the windowdow is closed when the respective button is hit
				case sf::Event::Closed:
					window.close();
					break;
				default:
					break;
			}
		}

		// If left mouse button is pressed, the density at the respective location is increased by 200
		if (sf::Mouse::isButtonPressed(sf::Mouse::Left))			
			logic.addDensity(currentMouse.y/SCALE, currentMouse.x/SCALE, 200);

		// Get's new mouse positions and subtracts the old mouse positions to get a ratio of velocity
		// the mouse is dragged over the windowdow
		currentMouse = sf::Mouse::getPosition(window);

		float amountX = currentMouse.x - previousMouse.x;
		float amountY = currentMouse.y - previousMouse.y;

		// Adds velocity to spots where the mouse is dragged over with the velocity of the mouse movement 
		logic.addVelocity(currentMouse.y/SCALE, currentMouse.x/SCALE, amountY / 10, amountX / 10);
		
		// Resets the current mouse position
		previousMouse = currentMouse;

		// Makes a step in the fluid simulationulation and updates the rendering
		logic.step();
		logic.render(window);

		// Let's the density in the windowdow reduce over time which is equivalent to flow of density outside 
		// of the windowdow. Without the fading, the density increases continously
		logic.fadeDensity(SIZE*SIZE);
		
		// Updates the visualization
		window.display();
	}
}

