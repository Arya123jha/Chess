#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <vector>
// #include "Piece.hpp"  // Use relative path instead of absolute

int main() {
    // Create window
    sf::RenderWindow window(sf::VideoMode(800, 800), "Chess");

    // Define colors
    const sf::Color lightSquare(240, 255, 255);
    const sf::Color darkSquare(165, 136, 99);

    // Create Board Grid
    const int BOARD_SIZE = 8;
    const int SQUARE_SIZE = 100;

    sf::RectangleShape squares[BOARD_SIZE][BOARD_SIZE];

    for (int row = 0; row < BOARD_SIZE; row++) {
        for (int col = 0; col < BOARD_SIZE; col++) {
            squares[row][col].setSize(sf::Vector2f(SQUARE_SIZE, SQUARE_SIZE));
            squares[row][col].setPosition(sf::Vector2f(col * SQUARE_SIZE, row * SQUARE_SIZE));

            if ((row + col) % 2 == 0) {
                squares[row][col].setFillColor(lightSquare);
            }
            else {
                squares[row][col].setFillColor(darkSquare);
            }
        }
    }

    while (window.isOpen()) {
        // SFML 2.6 event handling
        for (sf::Event event; window.pollEvent(event);) {
            switch (event.type) {
            case sf::Event::Closed:
                window.close();
                break;
                // Add other event cases as needed
            default:
                break;
            }
        }

        window.clear(sf::Color::White);

        // Draw the board
        for (int row = 0; row < BOARD_SIZE; row++) {
            for (int col = 0; col < BOARD_SIZE; col++) {
                window.draw(squares[row][col]);
            }
        }

        window.display();
    }
    return 0;
}