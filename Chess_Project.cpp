#include <SFML/Graphics.hpp>
#include <map>
#include <string>
#include <vector>

// Chessboard constants
const int TILE_SIZE = 80;      // Size of each chessboard square (in pixels)
const int BOARD_SIZE = 8;      // 8×8 chessboard

int main() {

    // Create game window
    sf::RenderWindow window(
        sf::VideoMode(BOARD_SIZE * TILE_SIZE, BOARD_SIZE * TILE_SIZE),
        "Chess Game"
    );

    // Colors for the board squares
    sf::Color lightSquare(240, 255, 255); // Light square color
    sf::Color darkSquare(165, 136, 99);   // Dark square color

    // Load all piece textures
    std::map<std::string, sf::Texture> pieceTextures; // Map from piece name → texture

    // Piece names matching PNG files
    std::vector<std::string> pieceNames = {
        "w_pawn", "w_rook", "w_knight", "w_bishop", "w_queen", "w_king",
        "b_pawn", "b_rook", "b_knight", "b_bishop", "b_queen", "b_king"
    };

    // Load textures for each piece
    for (const auto& name : pieceNames) {
        sf::Texture texture;
        if (!texture.loadFromFile("Assets/"+name + ".png")) { // Load from file
            return -1; // Exit if a texture can't be loaded
        }
        pieceTextures[name] = texture; // Store in the map
    }

  
   
    // Base Class Piece
    class Piece {
    public: 
        sf::Sprite sprite; // The image to display
        int row, col;      // Board position (0-7 for both row & col)
        std::string name;  // Name like "w_pawn" or "b_queen"
		bool hasMoved = false; // Track if piece has moved (for castling, en passant, etc.)
    public:

        // Constructor
        Piece(const std::string& name, int row, int col, const sf::Texture& texture)
            : name(name), row(row), col(col) {
            sprite.setTexture(texture);
                sprite.setPosition(static_cast<float>(col) * static_cast<float>(TILE_SIZE), static_cast<float>(row) * static_cast<float>(TILE_SIZE));
        }
        virtual ~Piece() = default;


       // Pure virtual function for movement validation
       virtual bool isValidMove(int newRow, int newCol, const std::vector<std::unique_ptr<Piece>>& pieces) const = 0;

       void setPosition(int newRow, int newCol) {
           row = newRow;
           col = newCol;
           sprite.setPosition(static_cast<float>(col) * static_cast<float>(TILE_SIZE), static_cast<float>(row) * static_cast<float>(TILE_SIZE));
           hasMoved = true;
       }
	   // Getters for piece properties
       sf::Sprite& getSprite() { return sprite; }
       int getRow()  { return row; }
       int getCol()  { return col; }
       const std::string& getName() const { return name; }
       bool getHasMoved() const { return hasMoved; }
       bool isWhite() const { return name[0] == 'w'; }

       // Helper function to check if square is occupied by same color
       bool isSquareOccupiedBySameColor(int row, int col, const std::vector<std::unique_ptr<Piece>>& pieces) const {
           for (const auto& piece : pieces) {
               if (piece->getRow() == row && piece->getCol() == col && piece->isWhite() == this->isWhite()) {
                   return true;
               }
           }
           return false;
       }

       // Helper function to check if path is clear (for sliding pieces)
       bool isPathClear(int newRow, int newCol, const std::vector<std::unique_ptr<Piece>>& pieces) const {
           int rowStep = (newRow > row) ? 1 : (newRow < row) ? -1 : 0;
           int colStep = (newCol > col) ? 1 : (newCol < col) ? -1 : 0;

           int currentRow = row + rowStep;
           int currentCol = col + colStep;

           while (currentRow != newRow || currentCol != newCol) {
               for (const auto& piece : pieces) {
                   if (piece->getRow() == currentRow && piece->getCol() == currentCol) {
                       return false; // There's a piece in the way
                   }
               }
               currentRow += rowStep;
               currentCol += colStep;
           }
           return true;
       }
    };
   
	// Derived classes for each piece type
    class Pawn : public Piece {
    public:
        Pawn(const std::string& name, int row, int col, const sf::Texture& texture)
            : Piece(name, row, col, texture) {
        }

        bool isValidMove(int newRow, int newCol, const std::vector<std::unique_ptr<Piece>>& pieces) const override {
            int direction = isWhite() ? -1 : 1; // White pawns move up (decreasing row), black down

            // Check for standard move
            if (col == newCol) {
                // Single move forward
                if (newRow == row + direction) {
                    // Check if destination is empty
                    for (const auto& piece : pieces) {
                        if (piece->getRow() == newRow && piece->getCol() == newCol) {
                            return false;
                        }
                    }
                    return true;
                }
                // Double move from starting position
                else if (newRow == row + 2 * direction && !hasMoved) {
                    // Check if both squares in front are empty
                    bool pathClear = true;
                    for (const auto& piece : pieces) {
                        if (piece->getCol() == col &&
                            ((piece->getRow() == row + direction) || (piece->getRow() == row + 2 * direction))) {
                            pathClear = false;
                            break;
                        }
                    }
                    return pathClear;
                }
            }
            // Check for capture
            else if (abs(newCol - col) == 1 && newRow == row + direction) {
                // Check if there's an opponent's piece to capture
                for (const auto& piece : pieces) {
                    if (piece->getRow() == newRow && piece->getCol() == newCol && piece->isWhite() != this->isWhite()) {
                        return true;
                    }
                }

            }

            return false;
        }
    };

    class Rook : public Piece {
    public:
        Rook(const std::string& name, int row, int col, const sf::Texture& texture)
            : Piece(name, row, col, texture) {
        }

        bool isValidMove(int newRow, int newCol, const std::vector<std::unique_ptr<Piece>>& pieces) const override {
            // Rook moves straight in any direction
            if (newRow != row && newCol != col) return false;

            // Check if path is clear
            if (!isPathClear(newRow, newCol, pieces)) return false;

            // Check if destination is occupied by same color
            return !isSquareOccupiedBySameColor(newRow, newCol, pieces);
        }
    };

    class Knight : public Piece {
    public:
        Knight(const std::string& name, int row, int col, const sf::Texture& texture)
            : Piece(name, row, col, texture) {
        }

        bool isValidMove(int newRow, int newCol, const std::vector<std::unique_ptr<Piece>>& pieces) const override {
            // Knight moves in L-shape
            int rowDiff = abs(newRow - row);
            int colDiff = abs(newCol - col);

            if (!((rowDiff == 2 && colDiff == 1) || (rowDiff == 1 && colDiff == 2))) {
                return false;
            }

            // Check if destination is occupied by same color
            return !isSquareOccupiedBySameColor(newRow, newCol, pieces);
        }
    };

    class Bishop : public Piece {
    public:
        Bishop(const std::string& name, int row, int col, const sf::Texture& texture)
            : Piece(name, row, col, texture) {
        }

        bool isValidMove(int newRow, int newCol, const std::vector<std::unique_ptr<Piece>>& pieces) const override {
            // Bishop moves diagonally
            if (abs(newRow - row) != abs(newCol - col)) return false;

            // Check if path is clear
            if (!isPathClear(newRow, newCol, pieces)) return false;

            // Check if destination is occupied by same color
            return !isSquareOccupiedBySameColor(newRow, newCol, pieces);
        }
    };

    class Queen : public Piece {
    public:
        Queen(const std::string& name, int row, int col, const sf::Texture& texture)
            : Piece(name, row, col, texture) {
        }

        bool isValidMove(int newRow, int newCol, const std::vector<std::unique_ptr<Piece>>& pieces) const override {
            // Queen moves like rook or bishop
            bool isStraight = (newRow == row || newCol == col);
            bool isDiagonal = (abs(newRow - row) == abs(newCol - col));

            if (!isStraight && !isDiagonal) return false;

            // Check if path is clear
            if (!isPathClear(newRow, newCol, pieces)) return false;

            // Check if destination is occupied by same color
            return !isSquareOccupiedBySameColor(newRow, newCol, pieces);
        }
    };

    class King : public Piece {
    public:
        King(const std::string& name, int row, int col, const sf::Texture& texture)
            : Piece(name, row, col, texture) {
        }

        bool isValidMove(int newRow, int newCol, const std::vector<std::unique_ptr<Piece>>& pieces) const override {
            // King moves one square in any direction
            int rowDiff = abs(newRow - row);
            int colDiff = abs(newCol - col);

            if (rowDiff > 1 || colDiff > 1) {
                // TODO: Add castling logic here
                return false;
            }

            // Check if destination is occupied by same color
            return !isSquareOccupiedBySameColor(newRow, newCol, pieces);
        }
    };

    std::vector<std::unique_ptr<Piece>> pieces; // All pieces in the game

    // ======================
   // Helper to add a piece
   // ======================
    auto addPiece = [&](const std::string& name, int row, int col) {
        if (name.find("pawn") != std::string::npos) {
            pieces.push_back(std::make_unique<Pawn>(name, row, col, pieceTextures[name]));
        }
        else if (name.find("rook") != std::string::npos) {
            pieces.push_back(std::make_unique<Rook>(name, row, col, pieceTextures[name]));
        }
        else if (name.find("knight") != std::string::npos) {
            pieces.push_back(std::make_unique<Knight>(name, row, col, pieceTextures[name]));
        }
        else if (name.find("bishop") != std::string::npos) {
            pieces.push_back(std::make_unique<Bishop>(name, row, col, pieceTextures[name]));
        }
        else if (name.find("queen") != std::string::npos) {
            pieces.push_back(std::make_unique<Queen>(name, row, col, pieceTextures[name]));
        }
        else if (name.find("king") != std::string::npos) {
            pieces.push_back(std::make_unique<King>(name, row, col, pieceTextures[name]));
        }
        };


    // Place black pieces
    addPiece("b_rook", 0, 0);
    addPiece("b_knight", 0, 1);
    addPiece("b_bishop", 0, 2);
    addPiece("b_queen", 0, 3);
    addPiece("b_king", 0, 4);
    addPiece("b_bishop", 0, 5);
    addPiece("b_knight", 0, 6);
    addPiece("b_rook", 0, 7);
    for (int col = 0; col < 8; ++col)
        addPiece("b_pawn", 1, col);


    // Place white pieces
    addPiece("w_rook", 7, 0);
    addPiece("w_knight", 7, 1);
    addPiece("w_bishop", 7, 2);
    addPiece("w_queen", 7, 3);
    addPiece("w_king", 7, 4);
    addPiece("w_bishop", 7, 5);
    addPiece("w_knight", 7, 6);
    addPiece("w_rook", 7, 7);
    for (int col = 0; col < 8; ++col)
        addPiece("w_pawn", 6, col);

    // =========================================================
     // PHASE 2 VARIABLES: Drag-and-drop movement & turn handling
     // =========================================================
    bool isDragging = false;       // Are we currently dragging a piece?
    int draggedIndex = -1;         // Index of the piece being dragged in 'pieces' vector
    sf::Vector2f dragOffset;       // Distance between mouse click & piece position
    bool whiteTurn = true;         // True if it's white's turn, false if black's

    // ======================
    // Main game loop
    // ======================
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            // Close window event
            if (event.type == sf::Event::Closed)
                window.close();

            // ----------------------
            // Mouse pressed: Start dragging
            // ----------------------
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);

                // Loop backwards so topmost piece is selected if stacked
                for (int i = pieces.size() - 1; i >= 0; --i) {
                    if (pieces[i]->getSprite().getGlobalBounds().contains(mousePos)) {
                        // Only allow dragging of correct color piece
                        if ((whiteTurn && pieces[i]->name[0] == 'w') ||
                            (!whiteTurn && pieces[i]->name[0] == 'b')) {
                            isDragging = true;
                            draggedIndex = i;
                            dragOffset = mousePos - pieces[i]->getSprite().getPosition(); // Remember offset
                            break; // Stop checking once found
                        }
                    }
                }
            }

            // ----------------------
            // Mouse released: Snap to square
            // ----------------------
            if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
                if (isDragging && draggedIndex != -1) {
                    // Get center position of the dragged piece
                    sf::Vector2f pos = pieces[draggedIndex]->getSprite().getPosition() +
                        sf::Vector2f(TILE_SIZE / 2, TILE_SIZE / 2);

                    // Convert to board coordinates
                    int newCol = pos.x / TILE_SIZE;
                    int newRow = pos.y / TILE_SIZE;

                    // Keep inside the board
                    if (newCol < 0) newCol = 0;
                    if (newCol >= BOARD_SIZE) newCol = BOARD_SIZE - 1;
                    if (newRow < 0) newRow = 0;
                    if (newRow >= BOARD_SIZE) newRow = BOARD_SIZE - 1;

                    // Snap the piece to the calculated square
                    pieces[draggedIndex]->getSprite().setPosition(newCol* TILE_SIZE, newRow* TILE_SIZE);
                    pieces[draggedIndex]->row = newRow;
                    pieces[draggedIndex]->col = newCol;

                    // Stop dragging
                    isDragging = false;

                    // Switch turn after move
                    whiteTurn = !whiteTurn;
                }
            }

            // ----------------------
            // Mouse moved: Update piece position while dragging
            // ----------------------
            if (event.type == sf::Event::MouseMoved) {
                if (isDragging && draggedIndex != -1) {
                    sf::Vector2f mousePos(event.mouseMove.x, event.mouseMove.y);
                    // Move piece with mouse, keeping the click offset
                    pieces[draggedIndex]->getSprite().setPosition(mousePos - dragOffset);
                }
            }
        }

        // ======================
        // Drawing section
        // ======================
        window.clear();

        // Draw chessboard
        for (int row = 0; row < BOARD_SIZE; ++row) {
            for (int col = 0; col < BOARD_SIZE; ++col) {
                sf::RectangleShape square(sf::Vector2f(TILE_SIZE, TILE_SIZE));
                square.setPosition(static_cast<float>(col) * static_cast<float>(TILE_SIZE), static_cast<float>(row) * static_cast<float>(TILE_SIZE));
                square.setFillColor((row + col) % 2 == 0 ? lightSquare : darkSquare);
                window.draw(square);
            }
        }

        // Draw all pieces (dragged piece drawn last so it appears on top)
        for (int i = 0; i < pieces.size(); ++i) {
            if (i != draggedIndex)
                window.draw(pieces[i]->getSprite()); // Draw all except the one being dragged
        }
        if (draggedIndex != -1)
            window.draw(pieces[draggedIndex]->getSprite()); // Draw dragged piece on top

        window.display();
    }
}