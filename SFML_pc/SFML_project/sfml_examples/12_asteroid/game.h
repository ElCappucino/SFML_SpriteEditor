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

void GameInit();
void GameUpdate(double dt, long frame);
void GameDraw(double dt);
void GameClean();
