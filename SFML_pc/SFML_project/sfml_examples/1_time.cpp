#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <iostream>

sf::RenderWindow window;




int main()
{
    // create window
    window.create(sf::VideoMode(1000, 1000), "My window");

    
    sf::Clock clock1;
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



        // draw framebuffer
        window.clear(sf::Color::Black);

        window.display();
    }



    return 0;
}