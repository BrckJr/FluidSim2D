#include "../header/simulation.h"
#include <iostream>

// Creating a window where we run the simulation
// Logic(_dt, _diff, _viscosity)
Simulation::Simulation() : options(Options()), logic(Logic(0.1f, 0.0f, 0.0005f)) {
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
				// Checks if key 'c' is pressed to change the color scheme of the visualization
				case sf::Event::KeyReleased:
					if (e.key.code == sf::Keyboard::Key::C) {
						Color c = (options.GetColor() == Color::Default) ?
							Color::Hsb : (options.GetColor() == Color::Hsb) ?
							Color::Velocity : Color::Default;
						options.SetColor(c);
					}
					break;
				default:
					break;
			}
		}

		// If left mouse button is pressed, the density at the respective location is increased by 200
		if (sf::Mouse::isButtonPressed(sf::Mouse::Left))			
			logic.AddDensity(currentMouse.y/SCALE, currentMouse.x/SCALE, 200);

		// Get's new mouse positions and subtracts the old mouse positions to get a ratio of velocity
		// the mouse is dragged over the windowdow
		currentMouse = sf::Mouse::getPosition(window);

		float amountX = currentMouse.x - previousMouse.x;
		float amountY = currentMouse.y - previousMouse.y;

		// Adds velocity to the single grid elements with the ratio of the mouse movement 
		// between currentMouse and previousMouse multiplied with the factor 0.1 
		logic.AddVelocity(currentMouse.y/SCALE, currentMouse.x/SCALE, amountY / 10, amountX / 10);
		
		// Resets the current mouse position
		previousMouse = currentMouse;

		// Makes a step in the fluid simulationulation and updates the rendering
		logic.Step();
		logic.Render(window, options.GetColor());

		// Let's the density in the windowdow reduce over time which is equivalent to flow of density outside 
		// of the windowdow. Without the fading, the density increases continously
		logic.FadeDensity(SIZE*SIZE);
		
		// Updates the visualization
		window.display();
	}
}

