#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <iostream>


int main()
{
    // create the window
    sf::RenderWindow window(sf::VideoMode(800, 600), "My window");

    sf::Font font;
    if (!font.loadFromFile("THSarabunNew.ttf"))
    {
        // error...
    }
    sf::Text text;

    // select the font
    text.setFont(font); // font is a sf::Font

    // set the string to display
    text.setString(L"Hello world สวัสดีครับ");

    // set the character size
    text.setCharacterSize(48); // in pixels, not points!

    // set the color
    text.setFillColor(sf::Color::Green);
    text.setPosition(20, 100);



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
        window.draw(text);

        // end the current frame
        window.display();
    }

    return 0;
}