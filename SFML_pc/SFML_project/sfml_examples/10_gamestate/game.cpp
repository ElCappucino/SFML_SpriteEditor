#include "game.h"



void GameInit() {

	std::cout << "game init" << std::endl;

}

void GameClean() {

	std::cout << "game clean" << std::endl;
}

void GameUpdate(double dt) {

	std::cout << "fps: " << 1 / dt << std::endl;

	// Process keyboard and mouse
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)){

	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {

	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {

	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {

	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::J)) {
	
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::K)) {
		
	}

	

}


void GameDraw(double dt) {

	// clear framebuffer
	window.clear(sf::Color::Black);

	// draw here
	// ...


	// swap framebuffer
	window.display();


}