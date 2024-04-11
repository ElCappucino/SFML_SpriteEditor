#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <cstring>
#include <math.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/random.hpp>


extern sf::RenderWindow window;

// -------------------------------------------
// Defines
// -------------------------------------------
#define IMAGE_POSITION_X			500
#define IMAGE_POSITION_Y			400

enum class EditMode { Standby, Editing };
enum class MouseState { Hold, Release, Clicked };

typedef struct {
	sf::Vector2i topleft;
	sf::Vector2i topright;
	sf::Vector2i bottomleft;
	sf::Vector2i bottomright;
} slice;

void TextBoxHandler(const sf::Event& event, sf::RenderWindow& window);
void GameInit();
void GameUpdate(double dt, long frame, int& state);
void GameDraw(double dt);
void GameClean();
