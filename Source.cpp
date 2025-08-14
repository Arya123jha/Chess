//﻿#include <SFML/Graphics.hpp>
//#include <map>
//#include <string>
//#include <vector>
//
//// ======================
//// Chessboard constants
//// ======================
//const int TILE_SIZE = 80;      // Size of each chessboard square (in pixels)
//const int BOARD_SIZE = 8;      // 8×8 chessboard
//
//int main() {
//    // ======================
//    // Create game window
//    // ======================
//    sf::RenderWindow window(
//        sf::VideoMode(BOARD_SIZE * TILE_SIZE, BOARD_SIZE * TILE_SIZE),
//        "Chess Game"
//    );
//
//    // Colors for the board squares
//    sf::Color lightSquare(240, 255, 255); // Light square color
//    sf::Color darkSquare(165, 136, 99);   // Dark square color
//
//    // ======================
//    // Load all piece textures
//    // ======================
//    std::map<std::string, sf::Texture> pieceTextures; // Map from piece name → texture
//
//    // Piece names matching PNG files
//    std::vector<std::string> pieceNames = {
//        "w_pawn", "w_rook", "w_knight", "w_bishop", "w_queen", "w_king",
//        "b_pawn", "b_rook", "b_knight", "b_bishop", "b_queen", "b_king"
//    };
//
//    // Load textures for each piece
//    for (const auto& name : pieceNames) {
//        sf::Texture texture;
//        if (!texture.loadFromFile(name + ".png")) { // Load from file
//            return -1; // Exit if a texture can't be loaded
//        }
//        pieceTextures[name] = texture; // Store in the map
//    }
//
//    // ======================
//    // Piece structure
//    // ======================
//    struct Piece {
//        sf::Sprite sprite; // The image to display
//        int row, col;      // Board position (0-7 for both row & col)
//        std::string name;  // Name like "w_pawn" or "b_queen"
//    };
//
//    std::vector<Piece> pieces; // All pieces in the game
//
//    // ======================
//    // Helper to add a piece
//    // ======================
//    auto addPiece = [&](const std::string& name, int row, int col) {
//        sf::Sprite sprite;
//        sprite.setTexture(pieceTextures[name]); // Assign texture
//        sprite.setPosition(col * TILE_SIZE, row * TILE_SIZE); // Place at correct square
//        pieces.push_back({ sprite, row, col, name }); // Store in vector
//        };
//
//    // ======================
//    // Place black pieces
//    // ======================
//    addPiece("b_rook", 0, 0);
//    addPiece("b_knight", 0, 1);
//    addPiece("b_bishop", 0, 2);
//    addPiece("b_queen", 0, 3);
//    addPiece("b_king", 0, 4);
//    addPiece("b_bishop", 0, 5);
//    addPiece("b_knight", 0, 6);
//    addPiece("b_rook", 0, 7);
//    for (int col = 0; col < 8; ++col)
//        addPiece("b_pawn", 1, col);
//
//    // ======================
//    // Place white pieces
//    // ======================
//    addPiece("w_rook", 7, 0);
//    addPiece("w_knight", 7, 1);
//    addPiece("w_bishop", 7, 2);
//    addPiece("w_queen", 7, 3);
//    addPiece("w_king", 7, 4);
//    addPiece("w_bishop", 7, 5);
//    addPiece("w_knight", 7, 6);
//    addPiece("w_rook", 7, 7);
//    for (int col = 0; col < 8; ++col)
//        addPiece("w_pawn", 6, col);
//
//    // =========================================================
//    // PHASE 2 VARIABLES: Drag-and-drop movement & turn handling
//    // =========================================================
//    bool isDragging = false;       // Are we currently dragging a piece?
//    int draggedIndex = -1;         // Index of the piece being dragged in 'pieces' vector
//    sf::Vector2f dragOffset;       // Distance between mouse click & piece position
//    bool whiteTurn = true;         // True if it's white's turn, false if black's
//
//    // ======================
//    // Main game loop
//    // ======================
//    while (window.isOpen()) {
//        sf::Event event;
//        while (window.pollEvent(event)) {
//            // Close window event
//            if (event.type == sf::Event::Closed)
//                window.close();
//
//            // ----------------------
//            // Mouse pressed: Start dragging
//            // ----------------------
//            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
//                sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
//
//                // Loop backwards so topmost piece is selected if stacked
//                for (int i = pieces.size() - 1; i >= 0; --i) {
//                    if (pieces[i].sprite.getGlobalBounds().contains(mousePos)) {
//                        // Only allow dragging of correct color piece
//                        if ((whiteTurn && pieces[i].name[0] == 'w') ||
//                            (!whiteTurn && pieces[i].name[0] == 'b')) {
//                            isDragging = true;
//                            draggedIndex = i;
//                            dragOffset = mousePos - pieces[i].sprite.getPosition(); // Remember offset
//                            break; // Stop checking once found
//                        }
//                    }
//                }
//            }
//
//            // ----------------------
//            // Mouse released: Snap to square
//            // ----------------------
//            if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
//                if (isDragging && draggedIndex != -1) {
//                    // Get center position of the dragged piece
//                    sf::Vector2f pos = pieces[draggedIndex].sprite.getPosition() +
//                        sf::Vector2f(TILE_SIZE / 2, TILE_SIZE / 2);
//
//                    // Convert to board coordinates
//                    int newCol = pos.x / TILE_SIZE;
//                    int newRow = pos.y / TILE_SIZE;
//
//                    // Keep inside the board
//                    if (newCol < 0) newCol = 0;
//                    if (newCol >= BOARD_SIZE) newCol = BOARD_SIZE - 1;
//                    if (newRow < 0) newRow = 0;
//                    if (newRow >= BOARD_SIZE) newRow = BOARD_SIZE - 1;
//
//                    // Snap the piece to the calculated square
//                    pieces[draggedIndex].sprite.setPosition(newCol * TILE_SIZE, newRow * TILE_SIZE);
//                    pieces[draggedIndex].row = newRow;
//                    pieces[draggedIndex].col = newCol;
//
//                    // Stop dragging
//                    isDragging = false;
//
//                    // Switch turn after move
//                    whiteTurn = !whiteTurn;
//                }
//            }
//
//            // ----------------------
//            // Mouse moved: Update piece position while dragging
//            // ----------------------
//            if (event.type == sf::Event::MouseMoved) {
//                if (isDragging && draggedIndex != -1) {
//                    sf::Vector2f mousePos(event.mouseMove.x, event.mouseMove.y);
//                    // Move piece with mouse, keeping the click offset
//                    pieces[draggedIndex].sprite.setPosition(mousePos - dragOffset);
//                }
//            }
//        }
//
//        // ======================
//        // Drawing section
//        // ======================
//        window.clear();
//
//        // Draw chessboard
//        for (int row = 0; row < BOARD_SIZE; ++row) {
//            for (int col = 0; col < BOARD_SIZE; ++col) {
//                sf::RectangleShape square(sf::Vector2f(TILE_SIZE, TILE_SIZE));
//                square.setPosition(col * TILE_SIZE, row * TILE_SIZE);
//                square.setFillColor((row + col) % 2 == 0 ? lightSquare : darkSquare);
//                window.draw(square);
//            }
//        }
//
//        // Draw all pieces (dragged piece drawn last so it appears on top)
//        for (int i = 0; i < pieces.size(); ++i) {
//            if (i != draggedIndex)
//                window.draw(pieces[i].sprite); // Draw all except the one being dragged
//        }
//        if (draggedIndex != -1)
//            window.draw(pieces[draggedIndex].sprite); // Draw dragged piece on top
//
//        window.display();
//    }
//}