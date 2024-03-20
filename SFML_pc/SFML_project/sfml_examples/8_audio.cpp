#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Audio.hpp>

#include <iostream>


int main()
{
    // create the window
    sf::RenderWindow window(sf::VideoMode(800, 600), "My window");

    sf::SoundBuffer buffer;
    if (!buffer.loadFromFile("mario_level.ogg"))
        return -1;
    sf::Sound sound;
    sound.setBuffer(buffer);
    sound.play();
    sound.setLoop(true);


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
            sound.pause();
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            sound.stop();
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            sound.play();
        }


        // clear the window with black color
        window.clear(sf::Color::Black);

        // draw everything here...
        // window.draw(...);
 

        // end the current frame
        window.display();
    }

    return 0;
}