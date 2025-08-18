#include "Button.h"

Button::Button(const std::string& label, sf::Font& font, sf::Vector2f pos, sf::Vector2f size, unsigned int chsize) {
    shape.setSize(size); shape.setPosition(pos);
    shape.setFillColor(sf::Color(80, 80, 80));
    shape.setOutlineColor(sf::Color(220, 220, 220)); shape.setOutlineThickness(3.f);
    text.setFont(font); text.setString(label); text.setCharacterSize(chsize); text.setFillColor(sf::Color::White);
    sf::FloatRect b = text.getLocalBounds();
    text.setPosition(pos.x + (size.x - b.width) / 2.f - b.left, pos.y + (size.y - b.height) / 2.f - b.top - 4);
}

void Button::draw(sf::RenderWindow& w) const { w.draw(shape); w.draw(text); }
bool Button::isClicked(sf::Vector2f mousePos) const { return shape.getGlobalBounds().contains(mousePos); }
void Button::updateHover(sf::Vector2f mousePos) {
    if (shape.getGlobalBounds().contains(mousePos)) shape.setFillColor(sf::Color(110, 110, 110));
    else shape.setFillColor(sf::Color(80, 80, 80));
}