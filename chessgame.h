#pragma once
#include <SFML/Graphics.hpp>
#include <map>
#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <fstream>
#include <sstream>
#include <random>
#include "Piece.h"
#include "Button.h"
#include "Constants.h"

enum class GameState { MAIN_MENU, PLAYING_FRIEND, PLAYING_AI, GAME_OVER };

class ChessGame {
public:
    ChessGame();
    void run();

private:
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
    GameState state = GameState::MAIN_MENU;
    bool whiteTurn = true;
    bool gameOver = false;
    bool playWithAI = false;

    // Drag/drop
    bool dragging = false;
    int draggedIndex = -1;
    sf::Vector2f dragOffset;

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
    void processEvents();
    void handleMenuEvent(const sf::Event& e);
    void handleGameOverEvent(const sf::Event& e);
    void handleGameEvent(const sf::Event& e);

    // Helper
    void recordCapture(bool capturerIsWhite, const std::string& capturedName);
};