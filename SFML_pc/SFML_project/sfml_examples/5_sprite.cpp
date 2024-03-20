#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <iostream>


int main()
{
    // create the window
    sf::RenderWindow window(sf::VideoMode(800, 600), "My window");

    // create a texture, sprite
    // dont destroy texture
    sf::Texture tex_enemy1, tex_enemy2;
    if (!tex_enemy1.loadFromFile("enemy1.png"))
    {
        // error...
    }
    if (!tex_enemy2.loadFromFile("enemy2.png"))
    {
        // error...
    }
    sf::Sprite sprite1, sprite2;
    sprite1.setTexture(tex_enemy1);
    sprite2.setTexture(tex_enemy2);
    //sprite.setTextureRect(sf::IntRect(50, 50, 100, 100));
  
    //sprite1.setOrigin(sf::Vector2f(96.f, 96.f));
    
    // position
    sprite1.setPosition(sf::Vector2f(0.f, 0.f)); // absolute position
    sprite2.setPosition(sf::Vector2f(120.f, 0.f)); // absolute position
    //sprite1.move(sf::Vector2f(5.f, 10.f)); // offset relative to the current position

    // rotation
    //sprite1.setRotation(30.f); // absolute angle
    //sprite1.rotate(15.f); // offset relative to the current angle

    // scale
    //sprite1.setScale(sf::Vector2f(0.5f, 2.f)); // absolute scale factor
    //sprite1.scale(sf::Vector2f(1.5f, 3.f)); // factor relative to the current scale

    


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

        // clear the window with black color
        window.clear(sf::Color::Black);

        // draw everything here...
        // window.draw(...);
        window.draw(sprite1);
        window.draw(sprite2);

        // end the current frame
        window.display();
    }

    return 0;
}