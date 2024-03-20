#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <iostream>


int main()
{
    // create the window
    sf::RenderWindow window(sf::VideoMode(800, 600), "My window");

    sf::Texture tex_bg;
    if (!tex_bg.loadFromFile("scene.png"))  // 534x401
    {
        // error...
    }
    sf::Sprite sprite_bg;
    sprite_bg.setTexture(tex_bg);

    // let's define a view
    sf::View view;
    view.setCenter(534.f/2, 401.f/2);
    view.setSize(sf::Vector2f(534.f, 401.f));


   



    // run the program as long as the window is open
    while (window.isOpen())
    {
        // check all the window's events that were triggered since the last iteration of the loop
        sf::Event event;
        while (window.pollEvent(event))
        {
            // "close requested" event: we close the window
            if (event.type == sf::Event::Closed)
                window.close();
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
            view.move(1.f, 0.f);
            //view.rotate(-0.5f);
            //view.zoom(0.999f);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
            view.move(-1.f, 0.f);
            //view.rotate(0.5f);
            //view.zoom(1/0.999f);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::X)) {
            view.setCenter(534.f / 2, 401.f / 2);
            view.setSize(sf::Vector2f(534.f, 401.f));
        }

        // activate it
        window.setView(view);

        if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
        {
            sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
            std::cout << "pixel pos " << pixelPos.x << " " << pixelPos.y << std::endl;

            // convert it to world coordinates
            sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos);
            std::cout << "world pos " << worldPos.x << " " << worldPos.y << std::endl;

        }
        




        // clear the window with black color
        window.clear(sf::Color::Black);

        // draw everything here...
        // window.draw(...);
        window.draw(sprite_bg);

        // end the current frame
        window.display();
    }

    return 0;
}