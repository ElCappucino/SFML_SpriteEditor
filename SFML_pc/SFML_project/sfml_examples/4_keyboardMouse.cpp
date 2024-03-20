#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <iostream>

sf::RenderWindow window;


int main()
{
    // create window
    window.create(sf::VideoMode(1000, 1000), "My window");
    window.setFramerateLimit(10);
    
    while (window.isOpen())
    {
        // handle event
        sf::Event event;
        while (window.pollEvent(event))
        {
			if (event.type == sf::Event::Closed)
				window.close();
        }

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
			std::cout << "W pressed" << std::endl;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
			std::cout << "D pressed" << std::endl;
		}
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Right))
        {
            std::cout << "Right pressed" << std::endl;
        }
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
        {
            sf::Vector2i localPosition = sf::Mouse::getPosition(window);
            std::cout << "mouse x: "  << localPosition.x << std::endl;
            std::cout << "mouse y: " << localPosition.y << std::endl;
        }


        // draw framebuffer
        window.clear(sf::Color::Black);

        window.display();
    }



    return 0;
}