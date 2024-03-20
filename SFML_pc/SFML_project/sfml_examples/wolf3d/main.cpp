#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include "game.h"
#include <iostream>

sf::RenderWindow window;

// framebuffer
sf::Texture frameBuffer;
sf::Sprite fbSprite; 
sf::Uint8* pixels;

sf::Clock clock1;


int main()
{
    // create window
    window.create(sf::VideoMode(1000, 1000), "My window");
    //window.setVerticalSyncEnabled(true);    // limit fps to 65fps
    //window.setFramerateLimit(20);

    // create framebuffer
    frameBuffer.create(1000, 1000);
    fbSprite.setTexture(frameBuffer);
    //fbSprite.setPosition(1000,0);
    //fbSprite.rotate(90);
    pixels = (sf::Uint8*)calloc(1000 * 1000 * 4, sizeof(sf::Uint8));

    int frame = 0;
    
    GameInit();

    while (window.isOpen())
    {
        // measure fps
        sf::Time elapsed1 = clock1.getElapsedTime();
        std::cout << 1/elapsed1.asSeconds() << std::endl;
        clock1.restart();

        // handle event
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        GameUpdate(frame++);
        GameDraw(frame);

        // draw framebuffer
        window.clear(sf::Color::Black);
        frameBuffer.update(pixels);
        window.draw(fbSprite);
        window.display();
    }

    GameClean();

    free(pixels);

    return 0;
}