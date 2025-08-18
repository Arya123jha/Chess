<<<<<<< HEAD
#pragma once
=======
#ifndef CHESSGAME_H
#define CHESSGAME_H

>>>>>>> 11ed285900d2ae6925459c826014a259e6eb0b8a
#include <SFML/Graphics.hpp>
#include <map>
#include <string>
#include <vector>
#include <memory>
<<<<<<< HEAD
#include <iostream>
#include <fstream>
#include <sstream>
#include <random>
#include "Piece.h"
#include "Button.h"
#include "Constants.h"

enum class GameState { MAIN_MENU, PLAYING_FRIEND, PLAYING_AI, GAME_OVER };

=======
#include <random>
#include <iostream>
#include "piece.h"
#include "constants.h"


using namespace std;

enum class GameState { MAIN_MENU, PLAYING_FRIEND, PLAYING_AI, GAME_OVER };

struct Button {
    sf::RectangleShape shape;
    sf::Text text;
    Button() = default;
    Button(const string& label, sf::Font& font, sf::Vector2f pos, sf::Vector2f size, unsigned int chsize = 28);
    void draw(sf::RenderWindow& w) const;
    bool isClicked(sf::Vector2f mousePos) const;
    void updateHover(sf::Vector2f mousePos);
};

>>>>>>> 11ed285900d2ae6925459c826014a259e6eb0b8a
class ChessGame {
public:
    ChessGame();
    void run();

private:
<<<<<<< HEAD
    // Game components
    sf::RenderWindow window;
    sf::Font font;
    std::map<std::string, sf::Texture> textures;
    std::vector<std::unique_ptr<Piece>> pieces;
    sf::RectangleShape squares[Chess::BOARD_SIZE][Chess::BOARD_SIZE];
    sf::Color baseLightColor = sf::Color(240, 217, 181);
    sf::Color baseDarkColor = sf::Color(181, 136, 99);

    // UI elements
    Button playFriendBtn, playAIBtn, exitBtn, backToMenuBtn;
    sf::Text titleText, subText, footerText;

    // Game state
=======
    sf::RenderWindow window;
    sf::Font font;
    map<string, sf::Texture> textures;
    vector<unique_ptr<Piece>> pieces;
    sf::RectangleShape squares[BOARD_SIZE][BOARD_SIZE];
    sf::Color baseLightColor = sf::Color(240, 217, 181);
    sf::Color baseDarkColor = sf::Color(181, 136, 99);

    Button playFriendBtn, playAIBtn, exitBtn, backToMenuBtn;
    sf::Text titleText, subText;
    sf::Text footerText;

>>>>>>> 11ed285900d2ae6925459c826014a259e6eb0b8a
    GameState state = GameState::MAIN_MENU;
    bool whiteTurn = true;
    bool gameOver = false;
    bool playWithAI = false;

<<<<<<< HEAD
    // Drag/drop
=======
>>>>>>> 11ed285900d2ae6925459c826014a259e6eb0b8a
    bool dragging = false;
    int draggedIndex = -1;
    sf::Vector2f dragOffset;

<<<<<<< HEAD
    // En-passant
    std::pair<int, int> enPassantTarget = { -1,-1 };

    // Undo stack
    std::vector<std::vector<std::unique_ptr<Piece>>> historyStates;

    // AI
    std::mt19937 rng;
    bool aiThinking = false;
    sf::Clock aiClock;
    std::string endGameMessage;
    std::vector<std::string> capturedByWhite, capturedByBlack;

    // AI candidate struct
    struct Candidate {
        int pieceIndex;
        int tr, tc;
        bool isCapture;
        int captureValue;
        bool givesCheck;
    };

    // Initialization
    void loadTextures();
    void setupBoardUI();
    void setupMenuUI();

    // Game flow
    void startNewGame(bool vsAI);
    void saveState();
    void undo();
    void saveToFile(const std::string& fname);
    void loadFromFile(const std::string& fname);
    bool tryMove(int pieceIndex, int toR, int toC);
    bool makeAIMove(bool aiWhite);
    bool tryAndApplyAIMove(const Candidate& cand);
    std::vector<std::tuple<int, int, bool>> computeLegalMovesForPiece(int index);

    // Rendering
    void render();
    void renderMainMenu();
    void renderBoardAndSidebar();
    void drawSidebar();
    void drawCapturedRow(float startX, float& y, const std::vector<std::string>& captured, float iconSize = 36.f, float padding = 6.f);
    void drawHighlightsForPiece(int pieceIndex);
    void renderGameOverOverlay();
    void updateCheckHighlights();
    void drawKingCheckOutlines();

    // Event handling
=======
    pair<int, int> enPassantTarget = { -1,-1 };
    vector<vector<unique_ptr<Piece>>> historyStates;
    mt19937 rng;

    bool aiThinking = false;
    sf::Clock aiClock;
    string endGameMessage;
    vector<string> capturedByWhite;
    vector<string> capturedByBlack;

    struct Candidate { int pieceIndex; int tr, tc; bool isCapture; int captureValue; bool givesCheck; };

    void loadTextures();
    void setupBoardUI();
    void setupMenuUI();
    void addPiece(const string& name, int r, int c);
    void startNewGame(bool vsAI);
    void saveState();
    void undo();
    void saveToFile(const string& fname);
    void loadFromFile(const string& fname);
    bool tryMove(int pieceIndex, int toR, int toC);
    bool makeAIMove(bool aiWhite);
    bool tryAndApplyAIMove(const Candidate& cand);
    vector<tuple<int, int, bool>> computeLegalMovesForPiece(int index);
    void updateCheckHighlights();
    void drawKingCheckOutlines();
>>>>>>> 11ed285900d2ae6925459c826014a259e6eb0b8a
    void processEvents();
    void handleMenuEvent(const sf::Event& e);
    void handleGameOverEvent(const sf::Event& e);
    void handleGameEvent(const sf::Event& e);
<<<<<<< HEAD

    // Helper
    void recordCapture(bool capturerIsWhite, const std::string& capturedName);
};
=======
    void render();
    void renderMainMenu();
    void renderBoardAndSidebar();
    void drawCapturedRow(float startX, float& y, const vector<string>& captured, float iconSize = 36.f, float padding = 6.f);
    void drawSidebar();
    void drawHighlightsForPiece(int pieceIndex);
    void renderGameOverOverlay();
    void recordCapture(bool capturerIsWhite, const string& capturedName);
};

#endif 

>>>>>>> 11ed285900d2ae6925459c826014a259e6eb0b8a
