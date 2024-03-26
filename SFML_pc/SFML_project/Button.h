#pragma once
#include "game.h"
#include <functional>

class Button {
public:
    sf::RectangleShape buttonShape;
    sf::Text buttonText;
    sf::Color normalColor, hoverColor, clickColor;
    enum class State { Normal, Hovered, Clicked };
    State state = Button::State::Normal;
    std::function<void()> onClick; // Callback function for click
    // Constructor with text, position, size, and default colors
    Button(std::string text, sf::Vector2f position, sf::Vector2f size,
        sf::Color normalColor,
        sf::Color hoverColor,
        sf::Color clickColor
        , int charsize, sf::Font &font);

    // Function to check if mouse is hovering over the button
    bool isHovered(sf::Vector2f& mousePos){
        return buttonShape.getGlobalBounds().contains(mousePos);
    }

    // Function to handle button click and update state
    void handleClick(sf::Vector2f& mousePos) {
        if (isHovered(mousePos)) {
            if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
            {
                state = Button::State::Clicked;
                // onClick();
            }
            else {
                state = Button::State::Hovered;
            }
        }
        else {
            state = Button::State::Normal;
        }
        updateFillColor();
    }

    // Function to set a callback function to be called on button click
    void setOnClickCallback(std::function<void()>& callback) {
        onClick = callback;
    }

    // Function to draw the button on the window
    void draw(sf::RenderWindow& window) {
        window.draw(buttonShape);
        window.draw(buttonText);
    }

    void setFont(sf::Font font) {
        buttonText.setFont(font);
    }
    sf::RectangleShape getButtonShape() {
        return buttonShape;
    }
    sf::Text getButtonText() {
        return buttonText;
    }
private:
    

    // Helper functions to update button color based on state
    void updateFillColor() {
        switch (state) {
        case State::Normal:
            buttonShape.setFillColor(normalColor);
            break;
        case State::Hovered:
            buttonShape.setFillColor(hoverColor);
            break;
        case State::Clicked:
            buttonShape.setFillColor(clickColor);
            break;
        }
    }

    void updateTextPosition(){
        buttonText.setPosition(buttonShape.getPosition() + sf::Vector2f(buttonShape.getSize().x / 2.f - buttonText.getLocalBounds().width / 2.f,
                buttonShape.getSize().y / 2.f - buttonText.getLocalBounds().height / 2.f));
    }
};

// Button constructor implementation
Button::Button(std::string text, sf::Vector2f position, sf::Vector2f size,
    sf::Color normalColor, sf::Color hoverColor, sf::Color clickColor, int charsize, sf::Font &font) :
    buttonShape(size) {

    buttonText.setString(text);
    buttonText.setFont(font);
    buttonText.setCharacterSize(charsize); // Set your desired character size
    buttonText.setFillColor(sf::Color::Black);

    buttonShape.setPosition(position);
    
    this->normalColor = normalColor;
    this->hoverColor = hoverColor;
    this->clickColor = clickColor;
    updateFillColor();
    updateTextPosition();
}