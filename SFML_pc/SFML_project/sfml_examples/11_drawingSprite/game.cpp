#include "game.h"

// -------------------------------------------
// Level variable
// -------------------------------------------

// texture to use
sf::Texture		penTex;
sf::Texture		faceTex;
sf::Texture		marioTex;
sf::Texture		bgTex;
sf::Texture		explosionTex;

// Sprite to use
sf::Sprite		bgSprite;
sf::Sprite		penSprite;
sf::Sprite		marioSprite;
sf::Sprite		faceSprite;
sf::Sprite		explosionSprite;

int		explodeX, explodeY;

// View
sf::View view;


void GameInit() {

	// Load texture
	penTex.loadFromFile("asset\\pen.png");
	faceTex.loadFromFile("asset\\face.png");
	marioTex.loadFromFile("asset\\mario.png");
	bgTex.loadFromFile("asset\\dracula_bg2.png");
	explosionTex.loadFromFile("asset\\explosion2.png");

	// Set texture to sprite
	bgSprite.setTexture(bgTex);
	penSprite.setTexture(penTex);
	marioSprite.setTexture(marioTex);
	explosionSprite.setTexture(explosionTex);
	faceSprite.setTexture(faceTex);


	// Set sprite-texture mapping 
	//	- for explosionSprite, each frame 100x100 pixel, 9x9 frame in a texture
	explodeX = 0;
	explodeY = 0;
	explosionSprite.setTextureRect(sf::IntRect(explodeX, explodeY, 100, 100));
	

	// Set sprite transformation
	bgSprite.setPosition(sf::Vector2f(0.f, 0.f));
	bgSprite.setScale(sf::Vector2f(2.f, 2.f));

	marioSprite.setPosition(sf::Vector2f(0.f, 0.f));
	marioSprite.setScale(sf::Vector2f(0.2f, 0.2f));

	penSprite.setPosition(sf::Vector2f(200.f, 0.f));
	penSprite.setScale(sf::Vector2f(0.5f, 0.5f));

	faceSprite.setPosition(sf::Vector2f(400.f, 0.f));
	faceSprite.setScale(sf::Vector2f(0.2f, 0.2f));

	explosionSprite.setPosition(sf::Vector2f(200.f, 400.f));
	explosionSprite.setScale(sf::Vector2f(5.f, 5.f));
	
	// set view
	view.setCenter(window.getSize().x/2, window.getSize().y/2);
	view.setSize(sf::Vector2f(window.getSize().x, window.getSize().y));
	view.setRotation(0.0f);

}



void GameUpdate(double dt, long frame) {

	//-----------------------------------------
	// Get user input
	//-----------------------------------------
	// Move Player sprite WSAD
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)){
		marioSprite.move(sf::Vector2f(0.f, -10.f));
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
		marioSprite.move(sf::Vector2f(10.f, 0.f));
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
		marioSprite.move(sf::Vector2f(0.f, 10.f));
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
		marioSprite.move(sf::Vector2f(-10.f, 0.f));
	}

	// Move cam: pan-IJKL, zoom-UO, rotate-89
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::I)) {
		view.move(0.f, -5.f);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::K)) {
		view.move(0.f, 5.f);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::J)) {
		view.move(-5.f, 0.f);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::L)) {
		view.move(5.f, 0.f);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::U)) {
		view.zoom(0.99f); 
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::O)) {
		view.zoom(1 / 0.99f);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num8)) {
		view.rotate(-2.f);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num9)) {
		view.rotate(2.f);
	}

	
	//-----------------------------------------
	// Update animation
	//-----------------------------------------
	if ((frame % 1) == 0) {			// do once for each n frames, to slow the animation speed
		explodeX += 100;			// move 1 frame to the right
		if (explodeX > 801) {		// if reaching the end of this row
			explodeX = 0;			// set x to left most
			explodeY += 100;		// set to the row below
			if (explodeY > 801) {	// if reaching the last row
				explodeY = 0;		// go back to the top row
			}
		}
	}
	explosionSprite.setTextureRect(sf::IntRect(explodeX, explodeY, 100, 100));


}


void GameDraw(double dt) {

	window.setView(view);

	// clear framebuffer
	window.clear(sf::Color::Black);

	// draw here
	// ...
	window.draw(bgSprite);
	window.draw(penSprite);
	window.draw(marioSprite);
	window.draw(faceSprite);
	window.draw(explosionSprite);


	// swap framebuffer
	window.display();


}


void GameClean() {


}