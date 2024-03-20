#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <cstring>
#include <math.h>

extern sf::RenderWindow window;

void GameInit();
void GameUpdate(double dt);
void GameDraw(double dt);
void GameClean();
