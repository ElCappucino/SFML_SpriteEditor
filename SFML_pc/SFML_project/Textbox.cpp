#include "Textbox.h"

Textbox::Textbox(int posX, int posY, int width, int height) {
    currentString = "";
    text.setString(currentString);
    text.setPosition(posX, posY);
    this->posX = posX;
    this->posY = posY;
    this->width = width;
    this->height = height;
    isSelected = false;
    textboxBounds.setPosition(posX, posY);
    textboxBounds.setSize(sf::Vector2f(width, height));
    textboxBounds.setFillColor(sf::Color::Red);  // Transparent for visual outline
    textboxBounds.setOutlineColor(sf::Color::Red);        // Red outline for visibility
    textboxBounds.setOutlineThickness(2.0f);
}
void Textbox::updateTextbox() {
    
    // Adjust textbox position and size based on your layout
    
}

void Textbox::drawTextbox(sf::RenderWindow &window) {
    // updateTextbox();
    window.draw(textboxBounds);
    window.draw(text);
    
}
void Textbox::handleTextInput(const sf::Event& event) {
    if (event.type == sf::Event::TextEntered && isSelected) {
        // Check for backspace (delete character)
        if (event.text.unicode == 8) {
            if (!currentString.isEmpty()) {
                currentString.erase(currentString.getSize() - 1);
                text.setString(currentString);
            }
        }
        else {
            // Handle printable characters (adjust for character limits if needed)
            currentString += static_cast<char>(event.text.unicode);
            text.setString(currentString);
        }
    }
}

void Textbox::handleSelection(const sf::Event& event, sf::RenderWindow& window) {
    if (event.type == sf::Event::MouseButtonPressed) {
        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        // Check if mouse click is within textbox bounds (adjust based on your drawing position)
        if (mousePos.x > posX && mousePos.x < posX + width &&
            mousePos.y > posY && mousePos.y < posY + height) {
            isSelected = true;
        }
        else {
            isSelected = false;
        }
    }
}