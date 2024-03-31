#include "game.h"
#include "Textbox.h"
#include "Button.h"
#include <vector>
#include <fstream>

static sf::Image    currentImage;		//	read color pixel
static sf::Texture	currentTexture;		//	show on screen
static sf::Sprite	currentSprite;		//	show on screen

static int imageOriginX;
static int imageOriginY;

static sf::Font mainfont;

static Textbox textbox1(150, 300, 300, 50);
static Textbox textbox2(150, 400, 300, 50);
static Textbox textbox3(150, 500, 300, 50);
static std::vector<Textbox> textboxes;
static std::vector<sf::Text*> texts;
static std::vector<Button*> buttons;
static std::vector<sf::CircleShape*> pins;
static std::vector<sf::Vector2i> fills;
static std::vector <sf::RectangleShape*> pinnedBorders;
static sf::String currentString;
static bool isSelected = false;

static float spritemultiplyer;

static sf::Vector2f mousePos;
static sf::Color backgrondColor(255,255,255);
enum class EditMode { Standby, Editing };
enum class MouseState { Hold, Release, Clicked };

static EditMode editmode = EditMode::Standby;
static MouseState mousestate = MouseState::Release;

typedef struct{
	sf::Vector2i topleft;
	sf::Vector2i topright;
	sf::Vector2i bottomleft;
	sf::Vector2i bottomright;
} slice;

static std::vector<slice> sliceFrames;
// View
sf::View view;

std::function<void()> ToggleEditMode([] {
	if (editmode == EditMode::Editing && mousestate == MouseState::Release) {
		editmode = EditMode::Standby;
		std::cout << "Standby" << std::endl;
	}
	else if (editmode == EditMode::Standby && mousestate == MouseState::Release) {
		editmode = EditMode::Editing;
		std::cout << "Edit" << std::endl;
	}
	});

std::function<void()> ExportCoordinates([] {
	std::ofstream outputfile("output.txt");
	for (int i = 0; i < sliceFrames.size(); i++) {
		outputfile << i << std::endl;
		outputfile << sliceFrames[i].topleft.x << " " << sliceFrames[i].topleft.y << std::endl;
		outputfile << sliceFrames[i].topright.x << " " << sliceFrames[i].topright.y << std::endl;
		outputfile << sliceFrames[i].bottomleft.x << " " << sliceFrames[i].bottomleft.y << std::endl;
		outputfile << sliceFrames[i].bottomright.x << " " << sliceFrames[i].bottomright.y << std::endl;
		outputfile << std::endl;
	}
	});
void GameInit() {

	mainfont.loadFromFile("asset\\THSarabunNew.ttf");

	/*textbox1.text.setFont(mainfont);
	textbox2.text.setFont(mainfont);
	textbox3.text.setFont(mainfont);

	textboxes.push_back(textbox1);
	textboxes.push_back(textbox2);
	textboxes.push_back(textbox3);*/

	texts.push_back(new sf::Text("Background Color = 255, 255, 255", mainfont, 32));
	texts[0]->setPosition(150, 200);
	/*texts.push_back(new sf::Text("R", mainfont, 32));
	texts[1]->setPosition(120, 300);
	texts.push_back(new sf::Text("G", mainfont, 32));
	texts[2]->setPosition(120, 400);
	texts.push_back(new sf::Text("B", mainfont, 32));
	texts[3]->setPosition(120, 500);*/

	for (int i = 0; i < texts.size(); i++) {
		texts[i]->setStyle(sf::Text::Bold);
		texts[i]->setFillColor(sf::Color::White);
	}

	buttons.push_back(new Button("Edit Mode/Stand by", sf::Vector2f(150, 600), sf::Vector2f(250, 80), 
		sf::Color::White, sf::Color::Cyan, sf::Color::Blue, 32, mainfont));
	buttons[0]->setOnClickCallback(ToggleEditMode);

	/*buttons.push_back(new Button("Slice", sf::Vector2f(150, 700), sf::Vector2f(250, 80),
		sf::Color::White, sf::Color::Cyan, sf::Color::Blue, 32, mainfont));*/

	buttons.push_back(new Button("Export", sf::Vector2f(150, 800), sf::Vector2f(250, 80),
		sf::Color::White, sf::Color::Cyan, sf::Color::Blue, 32, mainfont));
	buttons[1]->setOnClickCallback(ExportCoordinates);

	buttons.push_back(new Button("Redo", sf::Vector2f(600, 800), sf::Vector2f(100, 50),
		sf::Color::White, sf::Color::Cyan, sf::Color::Blue, 32, mainfont));
	buttons.push_back(new Button("Clear", sf::Vector2f(750, 800), sf::Vector2f(100, 50),
		sf::Color::White, sf::Color::Cyan, sf::Color::Blue, 32, mainfont));


	currentImage.loadFromFile("asset\\testpic2.png");
	currentTexture.loadFromFile("asset\\testpic2.png");
	currentSprite.setTexture(currentTexture);
	currentSprite.setOrigin(currentTexture.getSize().x / 2, currentTexture.getSize().y / 2);
	
	spritemultiplyer = 2;
	imageOriginX = 700 - ((currentSprite.getTextureRect().width * spritemultiplyer) / 2);
	imageOriginY = 500 - ((currentSprite.getTextureRect().height * spritemultiplyer) / 2);
	//std::cout << "imageOriginX + (currentSprite.getTextureRect().width * spritemultiplyer)  = " << imageOriginX + (currentSprite.getTextureRect().width * spritemultiplyer) << std::endl;
	//std::cout << "imageOriginY + (currentSprite.getTextureRect().height * spritemultiplyer) = " << imageOriginY + (currentSprite.getTextureRect().height * spritemultiplyer) << std::endl;
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

void FloodFill(int x, int y, sf::Color currentcolor) {

	int w = currentImage.getSize().x;
	int h = currentImage.getSize().y;

	fills.clear();
	std::vector<sf::Vector2i> queue;
	if (currentcolor == backgrondColor) { // work normally
		//std::cout << "current color = background color" << std::endl;
		return;
	}

	bool isAlreadyVisit = false;
	
	queue.push_back(sf::Vector2i(x, y));
	while (queue.empty() == false) {
		sf::Vector2i currentQueue = queue.back();
		//std::cout << "currentQueue.x : " << currentQueue.x << "currentQueue.y : " << currentQueue.y << std::endl;
		// check already visit position
		isAlreadyVisit = false;

		//// check out of bounds and background color matching
		if ((currentQueue.x < 0) ||
			(currentQueue.x >= w) ||
			(currentQueue.y < 0) ||
			(currentQueue.y >= h || 
			(currentImage.getPixel(currentQueue.x, currentQueue.y) == backgrondColor))) {

			//std::cout << "Background color" << std::endl;
			queue.pop_back();
			continue;
		}
		else { // if not, push into fills, popback queue, and add surrounded pixels.
			//std::cout << "else color" << currentQueue.x << ", " << currentQueue.y << std::endl;
			for (int i = 0; i < fills.size(); i++) {
				if (fills[i].x == currentQueue.x && fills[i].y == currentQueue.y) {
					//std::cout << "isAlreadyVisit = true" << std::endl;
					isAlreadyVisit = true;
					break;
				}
			}
			if (isAlreadyVisit) {
				//std::cout << currentQueue.x << ", " << currentQueue.y << " Already visit" << std::endl;
				queue.pop_back();
				continue;
			}
			fills.push_back(sf::Vector2i(currentQueue.x, currentQueue.y));
			queue.pop_back();
			queue.push_back(sf::Vector2i(currentQueue.x + 1, currentQueue.y));
			queue.push_back(sf::Vector2i(currentQueue.x - 1, currentQueue.y));
			queue.push_back(sf::Vector2i(currentQueue.x, currentQueue.y + 1));
			queue.push_back(sf::Vector2i(currentQueue.x, currentQueue.y - 1));
			//std::cout << "Fill push_back complete" << std::endl;
		}
	}
}

void BorderSetup(sf::Vector2i mousPos) {
	//std::cout << "Hello" << std::endl;
	if (sf::Mouse::getPosition(window).x < imageOriginX ||
		sf::Mouse::getPosition(window).x > imageOriginX + (currentSprite.getTextureRect().width * spritemultiplyer) ||
		sf::Mouse::getPosition(window).y < imageOriginY ||
		sf::Mouse::getPosition(window).y > imageOriginY + (currentSprite.getTextureRect().height * spritemultiplyer)) {

		//std::cout << "Error: out of image bound" << std::endl;
	}
	else {
		sf::Color currentColor = currentImage.getPixel((sf::Mouse::getPosition(window).x - imageOriginX) / spritemultiplyer, (sf::Mouse::getPosition(window).y - imageOriginY) / spritemultiplyer);
		//std::cout << (int)currentColor.r << ", " << (int)currentColor.g << ", " << (int)currentColor.b << std::endl;
		//std::cout << "Mouse position: x " << sf::Mouse::getPosition(window).x << " y " << sf::Mouse::getPosition(window).y << std::endl;
		int startPosX = (sf::Mouse::getPosition(window).x - imageOriginX) / spritemultiplyer;
		int startPosY = (sf::Mouse::getPosition(window).y - imageOriginY) / spritemultiplyer;
		//std::cout << "startPosX " << startPosX << " startPosY " << startPosY << std::endl;
		FloodFill(startPosX, startPosY, currentColor);
		int minX, maxX, minY, maxY;
		for (int i = 0; i < fills.size(); i++) {
			//std::cout << "fills no" << i << ": " << fills[i].x << " " << fills[i].y << std::endl;
			if (i == 0) {
				minX = fills[i].x;
				minY = fills[i].y;
				maxX = fills[i].x;
				maxY = fills[i].y;
			}
			else {
				if (fills[i].x < minX) {
					minX = fills[i].x;
				}
				if (fills[i].x > maxX) {
					maxX = fills[i].x;
				}
				if (fills[i].y < minY) {
					minY = fills[i].y;
				}
				if (fills[i].y > maxY) {
					maxY = fills[i].y;
				}
			}
		}
		slice newslice = { sf::Vector2i(minX, minY),
						  sf::Vector2i(maxX, minY),
						  sf::Vector2i(minX, maxY),
						  sf::Vector2i(maxX, maxY) };
		//std::cout << "(" << newslice.topleft.x << "," << newslice.topleft.y << ")" << std::endl;
		//std::cout << "(" << newslice.topright.x << "," << newslice.topright.y << ")" << std::endl;
		//std::cout << "(" << newslice.bottomleft.x << "," << newslice.bottomleft.y << ")" << std::endl;
		//std::cout << "(" << newslice.bottomright.x << "," << newslice.bottomright.y << ")" << std::endl;
		sf::RectangleShape *border = new sf::RectangleShape();
		sf::Vector2i topleft_resize = (sf::Vector2i(imageOriginX + (newslice.topleft.x * spritemultiplyer), imageOriginY + (newslice.topleft.y * spritemultiplyer)));
		sf::Vector2i bottomright_resize = (sf::Vector2i(imageOriginX + (newslice.bottomright.x * spritemultiplyer), imageOriginY + (newslice.bottomright.y * spritemultiplyer)));
		border->setFillColor(sf::Color::Transparent);
		border->setOutlineColor(sf::Color::Red);
		border->setOutlineThickness(2.0f);
		border->setPosition(sf::Vector2f(topleft_resize));
		border->setSize(sf::Vector2f(bottomright_resize.x - topleft_resize.x + 1, bottomright_resize.y - topleft_resize.y + 1));
		//std::cout << "borderPosition: " << border->getPosition().x + bottomright_resize.x - topleft_resize.x << ", " << border->getPosition().y + bottomright_resize.y - topleft_resize.y << std::endl;
		pinnedBorders.push_back(border);
		sliceFrames.push_back(newslice);
	}
}

void GameUpdate(double dt, long frame, int &state) {

	mousePos = (sf::Vector2f)sf::Mouse::getPosition(window);

	for (int i = 0; i < buttons.size(); i++) {
		buttons[i]->handleClick(mousePos);
	}

	if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
		if (editmode == EditMode::Editing && mousestate == MouseState::Release && buttons[0]->isHovered(mousePos) == false) {
			if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
				BorderSetup(sf::Mouse::getPosition(window));
			}
		}
		else {
			if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
				
			}
		}
		mousestate = MouseState::Hold;
	}
	else if (sf::Event::MouseButtonReleased) {
		mousestate = MouseState::Release;
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
	currentSprite.setScale(spritemultiplyer, spritemultiplyer);
	
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
	for (int i = 0; i < pinnedBorders.size(); i++) {
		//std::cout << "draw border" << std::endl;
		window.draw(*pinnedBorders[i]);
	}

	// swap framebuffer
	window.display();
}

void GameClean() {

}