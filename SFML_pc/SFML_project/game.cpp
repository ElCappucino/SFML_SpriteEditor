#include "game.h"
#include "Textbox.h"
#include "Button.h"
#include <vector>

static sf::Image    currentImage;		//	read color pixel
static sf::Texture	currentTexture;		//	show on screen
static sf::Sprite	currentSprite;		//	show on screen

static sf::Font mainfont;

static Textbox textbox1(150, 300, 300, 50);
static Textbox textbox2(150, 400, 300, 50);
static Textbox textbox3(150, 500, 300, 50);
static std::vector<Textbox> textboxes;
static std::vector<sf::Text*> texts;
static std::vector<Button*> buttons;
static sf::String currentString;
static bool isSelected = false;

static sf::Vector2f mousePos;

// View
sf::View view;

void GameInit() {

	mainfont.loadFromFile("asset\\THSarabunNew.ttf");

	textbox1.text.setFont(mainfont);
	textbox2.text.setFont(mainfont);
	textbox3.text.setFont(mainfont);

	textboxes.push_back(textbox1);
	textboxes.push_back(textbox2);
	textboxes.push_back(textbox3);

	texts.push_back(new sf::Text("Background Color", mainfont, 32));
	texts[0]->setPosition(150, 200);
	texts.push_back(new sf::Text("R", mainfont, 32));
	texts[1]->setPosition(120, 300);
	texts.push_back(new sf::Text("G", mainfont, 32));
	texts[2]->setPosition(120, 400);
	texts.push_back(new sf::Text("B", mainfont, 32));
	texts[3]->setPosition(120, 500);

	for (int i = 0; i < texts.size(); i++) {
		texts[i]->setStyle(sf::Text::Bold);
		texts[i]->setFillColor(sf::Color::White);
	}

	buttons.push_back(new Button("Edit Mode/Stand by", sf::Vector2f(150, 600), sf::Vector2f(250, 80), 
		sf::Color::White, sf::Color::Cyan, sf::Color::Blue, 32, mainfont));
	std::cout << buttons[0]->buttonText.getPosition().x << std::endl;
	buttons.push_back(new Button("Slice", sf::Vector2f(150, 700), sf::Vector2f(250, 80),
		sf::Color::White, sf::Color::Cyan, sf::Color::Blue, 32, mainfont));
	std::cout << buttons[1]->buttonText.getPosition().x << std::endl;
	buttons.push_back(new Button("Export", sf::Vector2f(150, 800), sf::Vector2f(250, 80),
		sf::Color::White, sf::Color::Cyan, sf::Color::Blue, 32, mainfont));
	std::cout << buttons[2]->buttonText.getPosition().x << std::endl;

	buttons.push_back(new Button("Redo", sf::Vector2f(600, 800), sf::Vector2f(100, 50),
		sf::Color::White, sf::Color::Cyan, sf::Color::Blue, 32, mainfont));
	buttons.push_back(new Button("Clear", sf::Vector2f(750, 800), sf::Vector2f(100, 50),
		sf::Color::White, sf::Color::Cyan, sf::Color::Blue, 32, mainfont));


	currentImage.loadFromFile("asset\\testpic1.png");
	currentTexture.loadFromFile("asset\\testpic1.png");
	currentSprite.setTexture(currentTexture);
	currentSprite.setOrigin(currentTexture.getSize().x / 2, currentTexture.getSize().y / 2);

	// set view
	view.setCenter(window.getSize().x/2, window.getSize().y/2);
	view.setSize(sf::Vector2f(window.getSize().x, window.getSize().y));
	view.setRotation(0.0f);

	std::cout << "game init" << std::endl;

}

void TextBoxHandler(const sf::Event& event, sf::RenderWindow& window) {
	for (int i = 0; i < textboxes.size(); i++) {
		textboxes[i].handleSelection(event, window);
		textboxes[i].handleTextInput(event);
	}
}

void GameUpdate(double dt, long frame, int &state) {

	mousePos = (sf::Vector2f)sf::Mouse::getPosition(window);

	for (int i = 0; i < buttons.size(); i++) {
		buttons[i]->handleClick(mousePos);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)){

	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {

	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
		
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {

	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::K)) {

	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::J)) {
		
	}
	//zoom-UO
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::U)) {
		view.zoom(0.99f); 
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::O)) {
		view.zoom(1 / 0.99f);
	}

}

void GameDraw(double dt) {

	window.setView(view);

	// clear framebuffer
	window.clear(sf::Color::Black);

	int w = currentSprite.getTextureRect().width;
	int h = currentSprite.getTextureRect().height;
	currentSprite.setTextureRect(sf::IntRect(0, 0, w, h));

	currentSprite.setPosition(700, 500);
	currentSprite.setScale(0.2, 0.2);
	
	for (int i = 0; i < textboxes.size(); i++) {
		textboxes[i].drawTextbox(window);
	}

	window.draw(currentSprite);

	for (int i = 0; i < texts.size(); i++) {
		window.draw(*texts[i]);
	}

	for (int i = 0; i < buttons.size(); i++) {
		buttons[i]->draw(window);
	}

	// swap framebuffer
	window.display();
}

void GameClean() {

}