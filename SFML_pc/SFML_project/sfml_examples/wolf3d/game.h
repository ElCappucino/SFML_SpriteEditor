#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <cstring>
#include <math.h>

extern sf::RenderWindow window;
extern sf::Uint8* pixels;


void GameInit();
void GameUpdate(int frame);
void GameDraw(int frame);
void GameClean();
