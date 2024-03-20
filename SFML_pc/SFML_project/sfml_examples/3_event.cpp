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
            if (event.type == sf::Event::Resized)
            {
                std::cout << "new width: " << event.size.width << std::endl;
                std::cout << "new height: " << event.size.height << std::endl;
            }
            if (event.type == sf::Event::LostFocus)
                std::cout << "lost focus " << std::endl;

            if (event.type == sf::Event::GainedFocus)
                std::cout << "gained focus " << std::endl;

            if (event.type == sf::Event::TextEntered)
            {
                if (event.text.unicode < 128)
                    std::cout << "ASCII character typed: " << static_cast<char>(event.text.unicode) << std::endl;
            }

            if (event.type == sf::Event::KeyPressed)
            {
                std::cout << event.key.scancode << std::endl;
                if (event.key.scancode == sf::Keyboard::Scan::Escape)
                {
                    std::cout << "the escape key was pressed" << std::endl;
                }
            }

            if (event.type == sf::Event::MouseWheelScrolled)
            {
                if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel)
                    std::cout << "wheel type: vertical" << std::endl;
                else if (event.mouseWheelScroll.wheel == sf::Mouse::HorizontalWheel)
                    std::cout << "wheel type: horizontal" << std::endl;
                else
                    std::cout << "wheel type: unknown" << std::endl;
                std::cout << "wheel movement: " << event.mouseWheelScroll.delta << std::endl;
                std::cout << "mouse x: " << event.mouseWheelScroll.x << std::endl;
                std::cout << "mouse y: " << event.mouseWheelScroll.y << std::endl;
            }

            if (event.type == sf::Event::MouseButtonPressed)
            {
                if (event.mouseButton.button == sf::Mouse::Right)
                {
                    std::cout << "the right button was pressed" << std::endl;
                    std::cout << "mouse x: " << event.mouseButton.x << std::endl;
                    std::cout << "mouse y: " << event.mouseButton.y << std::endl;
                }
            }

            if (event.type == sf::Event::MouseMoved)
            {
                //std::cout << "new mouse x: " << event.mouseMove.x << std::endl;
                //std::cout << "new mouse y: " << event.mouseMove.y << std::endl;
            }
        }



        // draw framebuffer
        window.clear(sf::Color::Black);

        window.display();
    }



    return 0;
}