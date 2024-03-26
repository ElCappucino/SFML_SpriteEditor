#pragma once
#include "game.h"
#include <string>

class Textbox {
public:
	sf::Text text;
	int posX, posY, width, height;
	bool isSelected;
	sf::String currentString;
	sf::RectangleShape textboxBounds;
	Textbox(int posX, int posY, int width, int height);
	void handleSelection(const sf::Event& event, sf::RenderWindow& window);
	void handleTextInput(const sf::Event& event);
	void updateTextbox();
	void drawTextbox(sf::RenderWindow& window);
};