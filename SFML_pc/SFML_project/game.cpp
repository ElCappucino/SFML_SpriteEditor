#include "game.h"
#include "Textbox.h"
#include "Button.h"
#include <vector>
#include <queue>
#include <fstream>
#include <sstream>

static sf::Image    currentImage;		//	read color pixel
static sf::Texture	currentTexture;		//	show on screen
static sf::Sprite	currentSprite;		//	show on screen

static int imageOriginX;				// Set the x origin of image that show on screen
static int imageOriginY;				// Set the y origin of image that show on screen

static sf::Font mainfont;				// Set the main font for text object

static std::vector<Textbox> textboxes;						// Vector contains textbox objects
static std::vector<sf::Text*> texts;						// Vector contains text objects
static std::vector<Button*> buttons;						// Vector contains button objects					
static std::vector<sf::Vector2i> fills;						// Vector contains fill coordinates for flood fill
static std::vector<sf::RectangleShape*> pinnedBorders;		// Vector contains rectangle border of sliced sprite show on screen
static std::vector<slice> sliceFrames;						// Vector contains sliced sprite coordinates

static int visitGrid[1000][1000] = { 0 };						// array contains visit grid status for flood fill

static float spritemultiplyer;								// multiplyer parameter for resize image on screen

static sf::Vector2f mousePos;								// contains coordinate of mouse position
static sf::Color backgrondColor(0, 0, 0, 0);				// set background color

static EditMode editmode = EditMode::Standby;				// edit state to check if it is during the sprite slicing
static MouseState mousestate = MouseState::Release;			// mouse state to check if it is click or hold. detain the overclicking per frame
static EditMode eyedropper = EditMode::Standby;

// View
sf::View view;

// toggle between Edit mode.
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

// redo the sliced sprite. attach with Redo button
std::function<void()> RedoSlicing([] {
	if (fills.size() > 0) {
		sliceFrames.pop_back();
		pinnedBorders.pop_back();
	}
	});

// clear all the sliced sprite. attach with Clear button
std::function<void()> ClearSlicing([] {
	if (fills.size() > 0) {
		sliceFrames.clear();
		pinnedBorders.clear();
	}
	});

// Export coordinates of sliced sprite into .txt file. 
std::function<void()> ExportCoordinates([] {
	std::ofstream outputfile;
	outputfile.open("output\\output.txt", std::ofstream::out | std::ofstream::trunc);
	outputfile << sliceFrames.size() << std::endl << std::endl;
	for (int i = 0; i < sliceFrames.size(); i++) {
		outputfile << i << std::endl;
		outputfile << sliceFrames[i].topleft.x << " " << sliceFrames[i].topleft.y << std::endl;
		outputfile << sliceFrames[i].topright.x << " " << sliceFrames[i].topright.y << std::endl;
		outputfile << sliceFrames[i].bottomleft.x << " " << sliceFrames[i].bottomleft.y << std::endl;
		outputfile << sliceFrames[i].bottomright.x << " " << sliceFrames[i].bottomright.y << std::endl;
		outputfile << std::endl;
	}
	outputfile.close();
	});

// Export coordinates of sliced sprite into .txt file. 
std::function<void()> Eyedropper([] {
	if (eyedropper == EditMode::Editing && mousestate == MouseState::Release) {
		eyedropper = EditMode::Standby;
		std::cout << "Standby" << std::endl;
	}
	else if (eyedropper == EditMode::Standby && mousestate == MouseState::Release) {
		eyedropper = EditMode::Editing;
		std::cout << "Edit" << std::endl;
	}
	});


void GameInit() {

	mainfont.loadFromFile("asset\\THSarabunNew.ttf");

	texts.push_back(new sf::Text("Background Color = 0, 0, 0", mainfont, 32));
	texts[0]->setPosition(20, 20);

	for (int i = 0; i < texts.size(); i++) {
		texts[i]->setStyle(sf::Text::Bold);
		texts[i]->setFillColor(sf::Color::White);
	}

	buttons.push_back(new Button("Edit Mode/Stand by", sf::Vector2f(50, 700), sf::Vector2f(250, 80),
		sf::Color::White, sf::Color::Cyan, sf::Color::Blue, 32, mainfont));
	buttons[0]->setOnClickCallback(ToggleEditMode);

	buttons.push_back(new Button("Export", sf::Vector2f(50, 800), sf::Vector2f(250, 80),
		sf::Color::White, sf::Color::Cyan, sf::Color::Blue, 32, mainfont));
	buttons[1]->setOnClickCallback(ExportCoordinates);

	buttons.push_back(new Button("Redo", sf::Vector2f(50, 900), sf::Vector2f(100, 50),
		sf::Color::White, sf::Color::Cyan, sf::Color::Blue, 32, mainfont));
	buttons[2]->setOnClickCallback(RedoSlicing);

	buttons.push_back(new Button("Clear", sf::Vector2f(200, 900), sf::Vector2f(100, 50),
		sf::Color::White, sf::Color::Cyan, sf::Color::Blue, 32, mainfont));
	buttons[3]->setOnClickCallback(ClearSlicing);

	buttons.push_back(new Button("Eyedropper", sf::Vector2f(350, 900), sf::Vector2f(100, 50),
		sf::Color::White, sf::Color::Cyan, sf::Color::Blue, 32, mainfont));
	buttons[4]->setOnClickCallback(Eyedropper);

	currentImage.loadFromFile("asset\\ken-sprite-sheet.png");
	currentTexture.loadFromFile("asset\\ken-sprite-sheet.png");
	currentSprite.setTexture(currentTexture);
	currentSprite.setOrigin(currentTexture.getSize().x / 2, currentTexture.getSize().y / 2);

	spritemultiplyer = 0.5;
	imageOriginX = IMAGE_POSITION_X - ((currentSprite.getTextureRect().width * spritemultiplyer) / 2);
	imageOriginY = IMAGE_POSITION_Y - ((currentSprite.getTextureRect().height * spritemultiplyer) / 2);

	// set view
	view.setCenter(window.getSize().x / 2, window.getSize().y / 2);
	view.setSize(sf::Vector2f(window.getSize().x, window.getSize().y));
	view.setRotation(0.0f);

	std::cout << "game init" << std::endl;

}

// Function to check the current active textbox.
void TextBoxHandler(const sf::Event& event, sf::RenderWindow& window) {
	for (int i = 0; i < textboxes.size(); i++) {
		textboxes[i].handleSelection(event, window);
		textboxes[i].handleTextInput(event);
	}
}

// BFS Flood fill algorithm to find the min/max of coordinates inside individual sprite
void FloodFill(int x, int y, sf::Color currentcolor) {

	if (currentcolor == backgrondColor)
	{
		return;
	}

	fills.clear(); // clear fills
	bool isAlreadyVisit = false; // bool use to check if it's already visit

	int runcount = 0; // Run count use for debugging

	//// Set width/height for Image boundary
	int w = currentImage.getSize().x;
	int h = currentImage.getSize().y;

	//// Set first item in queue
	std::queue<sf::Vector2i> queue;
	queue.push(sf::Vector2i(x, y));

	while (queue.empty() == false) {

		//// runtime checking
		// std::cout << "Queue size = " << queue.size() << " Run count = " << runcount << " Fill size = " << fills.size() << std::endl;
		// runcount++;

		//// pull by queue and pop out immediately
		sf::Vector2i currentQueue = queue.front();
		queue.pop();

		//// check already visit position
		isAlreadyVisit = false;

		//// check out of bounds and background color matching
		if ((currentQueue.x < 0) ||
			(currentQueue.x >= w) ||
			(currentQueue.y < 0) ||
			(currentQueue.y >= h ||
				(currentImage.getPixel(currentQueue.x, currentQueue.y) == backgrondColor)))
		{
			continue;
		}
		else
		{
			//// Check and set if it is already visit or not
			if (visitGrid[currentQueue.x][currentQueue.y] == 1) {
				continue;
			}
			else if (visitGrid[currentQueue.x][currentQueue.y] == 0) {
				visitGrid[currentQueue.x][currentQueue.y] = 1;
			}

			//// push_back() the fill grid. push() the new item in queue
			fills.push_back(sf::Vector2i(currentQueue.x, currentQueue.y));
			queue.push(sf::Vector2i(currentQueue.x + 1, currentQueue.y));
			queue.push(sf::Vector2i(currentQueue.x - 1, currentQueue.y));
			queue.push(sf::Vector2i(currentQueue.x, currentQueue.y + 1));
			queue.push(sf::Vector2i(currentQueue.x, currentQueue.y - 1));
		}
	}
}

// Function to setup the start position for BFS, call the BFS function, and save the slice coordinate to sliceFrames vector
void BorderSetup(sf::Vector2i mousPos) {

	//// Clear grid that use to check if already visit
	memset(visitGrid, 0, sizeof(visitGrid));

	//// Check if the mouse is not in the image area
	if (sf::Mouse::getPosition(window).x < imageOriginX ||
		sf::Mouse::getPosition(window).x > imageOriginX + (currentSprite.getTextureRect().width * spritemultiplyer) ||
		sf::Mouse::getPosition(window).y < imageOriginY ||
		sf::Mouse::getPosition(window).y > imageOriginY + (currentSprite.getTextureRect().height * spritemultiplyer))
	{
		// out of boundery error
	}
	else {

		sf::Color currentColor = currentImage.getPixel((sf::Mouse::getPosition(window).x - imageOriginX) / spritemultiplyer, (sf::Mouse::getPosition(window).y - imageOriginY) / spritemultiplyer);

		//// Start Position converting to image real size
		int startPosX = (sf::Mouse::getPosition(window).x - imageOriginX) / spritemultiplyer;
		int startPosY = (sf::Mouse::getPosition(window).y - imageOriginY) / spritemultiplyer;

		//// check if it's not start with background color
		if (currentColor == backgrondColor) {
			return;
		}
		else { //// else, start flood fill

			FloodFill(startPosX, startPosY, currentColor);

			//// Calculate the min/max to set the slicing boundary
			int minX, maxX, minY, maxY;
			for (int i = 0; i < fills.size(); i++) {

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

			//// Create slice via Slice struct
			slice newslice = { sf::Vector2i(minX, minY),
							  sf::Vector2i(maxX, minY),
							  sf::Vector2i(minX, maxY),
							  sf::Vector2i(maxX, maxY) };

			//// Setup rectangle display border
			sf::RectangleShape* border = new sf::RectangleShape();
			sf::Vector2i topleft_resize = (sf::Vector2i(imageOriginX + (newslice.topleft.x * spritemultiplyer), imageOriginY + (newslice.topleft.y * spritemultiplyer)));
			sf::Vector2i bottomright_resize = (sf::Vector2i(imageOriginX + (newslice.bottomright.x * spritemultiplyer), imageOriginY + (newslice.bottomright.y * spritemultiplyer)));
			border->setFillColor(sf::Color::Transparent);
			border->setOutlineColor(sf::Color::Red);
			border->setOutlineThickness(2.0f);
			border->setPosition(sf::Vector2f(topleft_resize));
			border->setSize(sf::Vector2f(bottomright_resize.x - topleft_resize.x + 1, bottomright_resize.y - topleft_resize.y + 1));

			//// check if that individual sprite is already slice up
			bool isDup = false;
			for (int i = 0; i < sliceFrames.size(); i++) {
				if (sliceFrames[i].topleft == newslice.topleft &&
					sliceFrames[i].topright == newslice.topright &&
					sliceFrames[i].bottomleft == newslice.bottomleft &&
					sliceFrames[i].bottomright == newslice.bottomright) {

					isDup = true;
				}
			}
			if (!isDup) {
				pinnedBorders.push_back(border);
				sliceFrames.push_back(newslice);
			}
		}
	}
}

void EyedropBackgroundColor() {
	backgrondColor = currentImage.getPixel((sf::Mouse::getPosition(window).x - imageOriginX) / spritemultiplyer, (sf::Mouse::getPosition(window).y - imageOriginY) / spritemultiplyer);
	std::stringstream ss;
	ss << "Background Color = " << (int)backgrondColor.r << ", " << (int)backgrondColor.g << ", " << (int)backgrondColor.b << ", " << (int)backgrondColor.a;
	texts[0]->setString(ss.str());
}
void GameUpdate(double dt, long frame, int& state) {

	mousePos = (sf::Vector2f)sf::Mouse::getPosition(window);

	for (int i = 0; i < buttons.size(); i++) {
		if (mousestate == MouseState::Release) {
			buttons[i]->handleClick(mousePos);
		}
	}

	if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
		if ((editmode == EditMode::Editing) &&
			(mousestate == MouseState::Release) &&
			(buttons[0]->isHovered(mousePos) == false))
		{
			BorderSetup(sf::Mouse::getPosition(window));
		}
		if ((eyedropper == EditMode::Editing) &&
			(mousestate == MouseState::Release) &&
			(buttons[4]->isHovered(mousePos) == false))
		{
			std::cout << "Hello" << std::endl;
			EyedropBackgroundColor();
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


	currentSprite.setPosition(IMAGE_POSITION_X, IMAGE_POSITION_Y);
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
		window.draw(*pinnedBorders[i]);
	}

	// swap framebuffer
	window.display();
}

void GameClean() {

}