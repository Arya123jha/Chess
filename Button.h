#pragma once
#include <SFML/Graphics.hpp>

struct Button {
    sf::RectangleShape shape;
    sf::Text text;

    Button() = default;
    Button(const std::string& label, sf::Font& font, sf::Vector2f pos, sf::Vector2f size, unsigned int chsize = 28);

    void draw(sf::RenderWindow& w) const;
    bool isClicked(sf::Vector2f mousePos) const;
    void updateHover(sf::Vector2f mousePos);
};
