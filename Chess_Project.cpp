#include <SFML/Graphics.hpp>
#include <map>
#include <string>
#include <vector>
#include <memory>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <random>
#include <ctime>

using namespace std;

// ---------- Constants ----------
const int TILE_SIZE = 80;                // size of board tile in pixels
const int BOARD_SIZE = 8;                // 8x8 board
const int SIDEBAR_WIDTH = 320;           // sidebar width in pixels
const float AI_THINK_SECONDS = 1.5f;     // AI thinking time in seconds

// ---------- Forward declarations ----------
class Piece;
class Pawn; class Rook; class Knight; class Bishop; class Queen; class King;

// ---------- Base piece and derived classes ----------
class Piece {
public:
    sf::Sprite sprite;                 // visual sprite
    int row = 0, col = 0;              // board coordinates
    string name;                       // e.g., "w_pawn"
    bool hasMoved = false;             // for castling / pawn double move

    // Constructor sets texture and initial position
    Piece(const string& name_, int r, int c, const sf::Texture& t)
        : name(name_), row(r), col(c) {
        sprite.setTexture(t);
        sprite.setPosition(static_cast<float>(col) * TILE_SIZE, static_cast<float>(row) * TILE_SIZE);
    }
    virtual ~Piece() = default;

    // Each derived piece implements movement rules here
    virtual bool isValidMove(int newRow, int newCol, const vector<unique_ptr<Piece>>& pieces) const = 0;

    // Put piece to new logical position and update sprite
    void setPosition(int r, int c) {
        row = r; col = c;
        sprite.setPosition(static_cast<float>(col) * TILE_SIZE, static_cast<float>(row) * TILE_SIZE);
        hasMoved = true;
    }
    sf::Sprite& getSprite() { return sprite; }
    int getRow() const { return row; }
    int getCol() const { return col; }
    const string& getName() const { return name; }
    bool isWhite() const { return !name.empty() && name[0] == 'w'; }
    bool getHasMoved() const { return hasMoved; }

    // Check if target square is occupied by same color
    bool isSquareOccupiedBySameColor(int r, int c, const vector<unique_ptr<Piece>>& pieces) const {
        for (const auto& p : pieces) if (p->getRow() == r && p->getCol() == c && p->isWhite() == isWhite()) return true;
        return false;
    }

    // Check path clear for sliding pieces (rook/bishop/queen)
    bool isPathClear(int newRow, int newCol, const vector<unique_ptr<Piece>>& pieces) const {
        int rowStep = (newRow > row) ? 1 : (newRow < row) ? -1 : 0;
        int colStep = (newCol > col) ? 1 : (newCol < col) ? -1 : 0;
        int r = row + rowStep, c = col + colStep;
        while (r != newRow || c != newCol) {
            for (const auto& p : pieces) if (p->getRow() == r && p->getCol() == c) return false;
            r += rowStep; c += colStep;
        }
        return true;
    }
};

class Pawn : public Piece {
public:
    Pawn(const string& n, int r, int c, const sf::Texture& t) : Piece(n, r, c, t) {}
    bool isValidMove(int newRow, int newCol, const vector<unique_ptr<Piece>>& pieces) const override {
        int dir = isWhite() ? -1 : 1;
        if (col == newCol) {
            if (newRow == row + dir) {
                for (const auto& p : pieces) if (p->getRow() == newRow && p->getCol() == newCol) return false;
                return true;
            }
            if (newRow == row + 2 * dir && !hasMoved) {
                for (const auto& p : pieces) {
                    if (p->getCol() == col && (p->getRow() == row + dir || p->getRow() == row + 2 * dir)) return false;
                }
                return true;
            }
        }
        else if (abs(newCol - col) == 1 && newRow == row + dir) {
            for (const auto& p : pieces) if (p->getRow() == newRow && p->getCol() == newCol && p->isWhite() != isWhite()) return true;
        }
        return false;
    }
};

class Rook : public Piece {
public:
    Rook(const string& n, int r, int c, const sf::Texture& t) : Piece(n, r, c, t) {}
    bool isValidMove(int newRow, int newCol, const vector<unique_ptr<Piece>>& pieces) const override {
        if (newRow != row && newCol != col) return false;
        if (!isPathClear(newRow, newCol, pieces)) return false;
        return !isSquareOccupiedBySameColor(newRow, newCol, pieces);
    }
};

class Knight : public Piece {
public:
    Knight(const string& n, int r, int c, const sf::Texture& t) : Piece(n, r, c, t) {}
    bool isValidMove(int newRow, int newCol, const vector<unique_ptr<Piece>>& pieces) const override {
        int dr = abs(newRow - row), dc = abs(newCol - col);
        if (!((dr == 2 && dc == 1) || (dr == 1 && dc == 2))) return false;
        return !isSquareOccupiedBySameColor(newRow, newCol, pieces);
    }
};

class Bishop : public Piece {
public:
    Bishop(const string& n, int r, int c, const sf::Texture& t) : Piece(n, r, c, t) {}
    bool isValidMove(int newRow, int newCol, const vector<unique_ptr<Piece>>& pieces) const override {
        if (abs(newRow - row) != abs(newCol - col)) return false;
        if (!isPathClear(newRow, newCol, pieces)) return false;
        return !isSquareOccupiedBySameColor(newRow, newCol, pieces);
    }
};

class Queen : public Piece {
public:
    Queen(const string& n, int r, int c, const sf::Texture& t) : Piece(n, r, c, t) {}
    bool isValidMove(int newRow, int newCol, const vector<unique_ptr<Piece>>& pieces) const override {
        bool straight = (newRow == row || newCol == col);
        bool diag = (abs(newRow - row) == abs(newCol - col));
        if (!straight && !diag) return false;
        if (!isPathClear(newRow, newCol, pieces)) return false;
        return !isSquareOccupiedBySameColor(newRow, newCol, pieces);
    }
};

class King : public Piece {
public:
    King(const string& n, int r, int c, const sf::Texture& t) : Piece(n, r, c, t) {}
    bool isValidMove(int newRow, int newCol, const vector<unique_ptr<Piece>>& pieces) const override {
        int dr = abs(newRow - row), dc = abs(newCol - col);
        if (dr > 1 || dc > 1) return false; // castling handled separately if added later
        return !isSquareOccupiedBySameColor(newRow, newCol, pieces);
    }
};

// ---------- Utilities ----------
unique_ptr<Piece> createPieceFromName(const string& name, int r, int c, const map<string, sf::Texture>& textures) {
    auto it = textures.find(name);
    if (it == textures.end()) throw runtime_error("Texture missing for: " + name);
    const sf::Texture& t = it->second;
    if (name.find("pawn") != string::npos) return make_unique<Pawn>(name, r, c, t);
    if (name.find("rook") != string::npos) return make_unique<Rook>(name, r, c, t);
    if (name.find("knight") != string::npos) return make_unique<Knight>(name, r, c, t);
    if (name.find("bishop") != string::npos) return make_unique<Bishop>(name, r, c, t);
    if (name.find("queen") != string::npos) return make_unique<Queen>(name, r, c, t);
    if (name.find("king") != string::npos) return make_unique<King>(name, r, c, t);
    throw runtime_error("Unknown piece type: " + name);
}

vector<unique_ptr<Piece>> clonePieces(const vector<unique_ptr<Piece>>& pieces, const map<string, sf::Texture>& textures) {
    vector<unique_ptr<Piece>> copy;
    copy.reserve(pieces.size());
    for (const auto& p : pieces) {
        auto np = createPieceFromName(p->getName(), p->getRow(), p->getCol(), textures);
        np->hasMoved = p->hasMoved;
        copy.push_back(move(np));
    }
    return copy;
}

// attack detection (used for check detection)
bool isSquareAttacked(int r, int c, bool byWhite, const vector<unique_ptr<Piece>>& pieces) {
    for (const auto& p : pieces) {
        if (p->isWhite() != byWhite) continue;
        string nm = p->getName();
        int pr = p->getRow(), pc = p->getCol();
        if (nm.find("pawn") != string::npos) {
            int dir = p->isWhite() ? -1 : 1;
            if (pr + dir == r && (pc - 1 == c || pc + 1 == c)) return true;
        }
        else if (nm.find("knight") != string::npos) {
            int dr = abs(pr - r), dc = abs(pc - c);
            if ((dr == 2 && dc == 1) || (dr == 1 && dc == 2)) return true;
        }
        else if (nm.find("bishop") != string::npos) {
            if (abs(pr - r) == abs(pc - c)) {
                int rs = (r > pr) ? 1 : -1, cs = (c > pc) ? 1 : -1;
                int rr = pr + rs, cc = pc + cs; bool blocked = false;
                while (rr != r && cc != c) {
                    for (const auto& o : pieces) if (o->getRow() == rr && o->getCol() == cc) { blocked = true; break; }
                    if (blocked) break;
                    rr += rs; cc += cs;
                }
                if (!blocked) return true;
            }
        }
        else if (nm.find("rook") != string::npos) {
            if (pr == r || pc == c) {
                int rs = (r == pr) ? 0 : ((r > pr) ? 1 : -1), cs = (c == pc) ? 0 : ((c > pc) ? 1 : -1);
                int rr = pr + rs, cc = pc + cs; bool blocked = false;
                while (rr != r || cc != c) {
                    for (const auto& o : pieces) if (o->getRow() == rr && o->getCol() == cc) { blocked = true; break; }
                    if (blocked) break;
                    rr += rs; cc += cs;
                }
                if (!blocked) return true;
            }
        }
        else if (nm.find("queen") != string::npos) {
            // combine rook & bishop
            if (pr == r || pc == c) {
                int rs = (r == pr) ? 0 : ((r > pr) ? 1 : -1), cs = (c == pc) ? 0 : ((c > pc) ? 1 : -1);
                int rr = pr + rs, cc = pc + cs; bool blocked = false;
                while (rr != r || cc != c) {
                    for (const auto& o : pieces) if (o->getRow() == rr && o->getCol() == cc) { blocked = true; break; }
                    if (blocked) break;
                    rr += rs; cc += cs;
                }
                if (!blocked) return true;
            }
            else if (abs(pr - r) == abs(pc - c)) {
                int rs = (r > pr) ? 1 : -1, cs = (c > pc) ? 1 : -1;
                int rr = pr + rs, cc = pc + cs; bool blocked = false;
                while (rr != r && cc != c) {
                    for (const auto& o : pieces) if (o->getRow() == rr && o->getCol() == cc) { blocked = true; break; }
                    if (blocked) break;
                    rr += rs; cc += cs;
                }
                if (!blocked) return true;
            }
        }
        else if (nm.find("king") != string::npos) {
            int dr = abs(pr - r), dc = abs(pc - c);
            if (dr <= 1 && dc <= 1) return true;
        }
    }
    return false;
}

// find king pos for color
pair<int, int> findKingPos(bool white, const vector<unique_ptr<Piece>>& pieces) {
    for (const auto& p : pieces) if (p->isWhite() == white && p->getName().find("king") != string::npos) return { p->getRow(), p->getCol() };
    return { -1,-1 };
}
bool isKingInCheck(bool white, const vector<unique_ptr<Piece>>& pieces) {
    auto kp = findKingPos(white, pieces);
    if (kp.first == -1) return false;
    return isSquareAttacked(kp.first, kp.second, !white, pieces);
}

// simulate a move to check whether it leaves own king in check
bool wouldMoveLeaveKingInCheck(int pieceIndex, int toRow, int toCol, const vector<unique_ptr<Piece>>& pieces, const map<string, sf::Texture>& textures) {
    auto copy = clonePieces(pieces, textures);
    if (pieceIndex < 0 || pieceIndex >= (int)copy.size()) return true;
    string movingName = pieces[pieceIndex]->getName();
    bool movingColor = pieces[pieceIndex]->isWhite();
    int idx = -1;
    for (int i = 0; i < (int)copy.size(); ++i) {
        if (copy[i]->getRow() == pieces[pieceIndex]->getRow() && copy[i]->getCol() == pieces[pieceIndex]->getCol() && copy[i]->getName() == movingName) { idx = i; break; }
    }
    if (idx == -1) return true;
    for (int i = 0; i < (int)copy.size(); ++i) {
        if (i != idx && copy[i]->getRow() == toRow && copy[i]->getCol() == toCol) { copy.erase(copy.begin() + i); if (i < idx) --idx; break; }
    }
    copy[idx]->setPosition(toRow, toCol);
    return isKingInCheck(movingColor, copy);
}

// check if color has any legal move
bool hasLegalMove(bool white, const vector<unique_ptr<Piece>>& pieces, const map<string, sf::Texture>& textures) {
    for (int i = 0; i < (int)pieces.size(); ++i) {
        if (pieces[i]->isWhite() != white) continue;
        for (int r = 0; r < BOARD_SIZE; ++r) for (int c = 0; c < BOARD_SIZE; ++c) {
            if (!pieces[i]->isValidMove(r, c, pieces)) continue;
            if (pieces[i]->isSquareOccupiedBySameColor(r, c, pieces)) continue;
            if (!wouldMoveLeaveKingInCheck(i, r, c, pieces, textures)) return true;
        }
    }
    return false;
}

// notation helpers and piece values
string coordToAlgebraic(int row, int col) { char file = 'a' + col; char rank = '8' - row; return string() + file + rank; }
char pieceToLetter(const string& name) {
    if (name.find("pawn") != string::npos) return ' ';
    if (name.find("rook") != string::npos) return 'R';
    if (name.find("knight") != string::npos) return 'N';
    if (name.find("bishop") != string::npos) return 'B';
    if (name.find("queen") != string::npos) return 'Q';
    if (name.find("king") != string::npos) return 'K';
    return ' ';
}
int pieceValue(const string& name) {
    if (name.find("pawn") != string::npos) return 1;
    if (name.find("knight") != string::npos) return 3;
    if (name.find("bishop") != string::npos) return 3;
    if (name.find("rook") != string::npos) return 5;
    if (name.find("queen") != string::npos) return 9;
    if (name.find("king") != string::npos) return 100;
    return 0;
}

// ---------- Small UI Button helper ----------
struct Button {
    sf::RectangleShape shape;
    sf::Text text;
    Button() = default;
    Button(const string& label, sf::Font& font, sf::Vector2f pos, sf::Vector2f size, unsigned int chsize = 28) {
        shape.setSize(size); shape.setPosition(pos);
        shape.setFillColor(sf::Color(80, 80, 80));
        shape.setOutlineColor(sf::Color(220, 220, 220)); shape.setOutlineThickness(3.f);
        text.setFont(font); text.setString(label); text.setCharacterSize(chsize); text.setFillColor(sf::Color::White);
        sf::FloatRect b = text.getLocalBounds();
        text.setPosition(pos.x + (size.x - b.width) / 2.f - b.left, pos.y + (size.y - b.height) / 2.f - b.top - 4);
    }
    void draw(sf::RenderWindow& w) const { w.draw(shape); w.draw(text); }
    bool isClicked(sf::Vector2f mousePos) const { return shape.getGlobalBounds().contains(mousePos); }
    void updateHover(sf::Vector2f mousePos) {
        if (shape.getGlobalBounds().contains(mousePos)) shape.setFillColor(sf::Color(110, 110, 110));
        else shape.setFillColor(sf::Color(80, 80, 80));
    }
};

// ---------- Main ChessGame class (OOP container for everything) ----------
enum class GameState { MAIN_MENU, PLAYING_FRIEND, PLAYING_AI, GAME_OVER };

class ChessGame {
public:
    sf::RenderWindow window;               // main window
    sf::Font font;                         // font for UI
    map<string, sf::Texture> textures;     // piece textures
    vector<unique_ptr<Piece>> pieces;      // current board pieces
    sf::RectangleShape squares[BOARD_SIZE][BOARD_SIZE]; // board squares visuals
    sf::Color baseLightColor = sf::Color(240, 217, 181);
    sf::Color baseDarkColor = sf::Color(181, 136, 99);

    // UI elements
    Button playFriendBtn, playAIBtn, exitBtn, backToMenuBtn;
    sf::Text titleText, subText;
    sf::Text footerText;

    // game state vars
    GameState state = GameState::MAIN_MENU;
    bool whiteTurn = true;
    bool gameOver = false;
    bool playWithAI = false;

    // drag/drop
    bool dragging = false;
    int draggedIndex = -1;
    sf::Vector2f dragOffset;

    // en-passant
    pair<int, int> enPassantTarget = { -1,-1 };

    // undo stack
    vector<vector<unique_ptr<Piece>>> historyStates;

    // RNG for AI
    mt19937 rng;

    // AI candidate struct
    struct Candidate { int pieceIndex; int tr, tc; bool isCapture; int captureValue; bool givesCheck; };

    // AI thinking control
    bool aiThinking = false;
    sf::Clock aiClock;

    // end game message (displayed on overlay)
    string endGameMessage;

    // NEW: captured pieces trackers
    vector<string> capturedByWhite; // white captured black pieces
    vector<string> capturedByBlack; // black captured white pieces

    // Constructor
    ChessGame() : window(sf::VideoMode(TILE_SIZE* BOARD_SIZE + SIDEBAR_WIDTH, TILE_SIZE* BOARD_SIZE), "SFML Chess") {
        rng.seed((unsigned)time(nullptr));
        // load font (change path if you store elsewhere)
        if (!font.loadFromFile("arial-font/arial.ttf")) throw runtime_error("Failed to load font arial.ttf - place in exe folder or change path.");
        loadTextures();
        setupBoardUI();
        setupMenuUI();
    }

    // Load textures (Assets/w_pawn.png etc.)
    void loadTextures() {
        vector<string> names = { "w_pawn","w_rook","w_knight","w_bishop","w_queen","w_king",
                                 "b_pawn","b_rook","b_knight","b_bishop","b_queen","b_king" };
        for (auto& n : names) {
            sf::Texture t;
            string path = "Assets/" + n + ".png";
            if (!t.loadFromFile(path)) {
                cerr << "Warning: failed to load " << path << " — creating placeholder\n";
                t.create(1, 1); // placeholder
            }
            textures[n] = t;
        }
    }

    // Setup board square colors and title/subtitle text
    void setupBoardUI() {
        for (int r = 0; r < BOARD_SIZE; ++r) for (int c = 0; c < BOARD_SIZE; ++c) {
            squares[r][c].setSize({ (float)TILE_SIZE,(float)TILE_SIZE });
            squares[r][c].setPosition(c * TILE_SIZE, r * TILE_SIZE);
            bool light = (r + c) % 2 == 0;
            squares[r][c].setFillColor(light ? baseLightColor : baseDarkColor);
        }

        titleText.setFont(font); titleText.setCharacterSize(64); titleText.setFillColor(sf::Color::Black);
        titleText.setString("SFML Chess");

        subText.setFont(font); subText.setCharacterSize(26); subText.setFillColor(sf::Color(20, 20, 20));
        subText.setString("Choose mode");

        footerText.setFont(font); footerText.setCharacterSize(14); footerText.setFillColor(sf::Color(30, 30, 30));
        footerText.setString("Drag pieces | U:Undo  S:Save  L:Load  R:Restart  Esc:Menu");
    }

    // Setup centered menu buttons (fully centered on the full window, not the left area)
    void setupMenuUI() {
        // Buttons width 360, height 72
        float btnW = 360.f, btnH = 72.f;
        float centerX = window.getSize().x / 2.f;
        float centerY = window.getSize().y / 2.f;

        // Buttons stacked vertically with pleasing gaps
        playFriendBtn = Button("Play with Friend", font,
            { centerX - btnW / 2.f, centerY - btnH - 10.f },
            { btnW, btnH }, 28);
        playAIBtn = Button("Play with AI", font,
            { centerX - btnW / 2.f, centerY + 10.f },
            { btnW, btnH }, 28);
        exitBtn = Button("Exit", font,
            { centerX - btnW / 2.f, centerY + btnH + 30.f },
            { btnW, btnH }, 28);

        float sx = TILE_SIZE * BOARD_SIZE;
        backToMenuBtn = Button("Back to Menu", font,
            { sx + 12.f, (float)window.getSize().y - 80.f },
            { SIDEBAR_WIDTH - 24.f, 60.f }, 22);
    }

    // Add piece helper
    void addPiece(const string& name, int r, int c) {
        pieces.push_back(createPieceFromName(name, r, c, textures));
    }

    // Start a new game (clear board and populate)
    void startNewGame(bool vsAI) {
        pieces.clear(); playWithAI = vsAI; whiteTurn = true; gameOver = false; aiThinking = false;
        enPassantTarget = { -1,-1 }; endGameMessage.clear(); historyStates.clear();
        capturedByWhite.clear(); capturedByBlack.clear();

        // Black pieces
        addPiece("b_rook", 0, 0); addPiece("b_knight", 0, 1); addPiece("b_bishop", 0, 2); addPiece("b_queen", 0, 3);
        addPiece("b_king", 0, 4); addPiece("b_bishop", 0, 5); addPiece("b_knight", 0, 6); addPiece("b_rook", 0, 7);
        for (int c = 0; c < 8; ++c) addPiece("b_pawn", 1, c);
        // White pieces
        addPiece("w_rook", 7, 0); addPiece("w_knight", 7, 1); addPiece("w_bishop", 7, 2); addPiece("w_queen", 7, 3);
        addPiece("w_king", 7, 4); addPiece("w_bishop", 7, 5); addPiece("w_knight", 7, 6); addPiece("w_rook", 7, 7);
        for (int c = 0; c < 8; ++c) addPiece("w_pawn", 6, c);

        saveState();
        state = vsAI ? GameState::PLAYING_AI : GameState::PLAYING_FRIEND;
    }

    // Save board clone for undo
    void saveState() { historyStates.push_back(clonePieces(pieces, textures)); }

    // Undo last move
    void undo() {
        if (historyStates.size() < 2) return;
        historyStates.pop_back();
        pieces = clonePieces(historyStates.back(), textures);
        whiteTurn = !whiteTurn;
        gameOver = false; aiThinking = false; enPassantTarget = { -1,-1 }; endGameMessage.clear();

        // NOTE: For simplicity, captured lists are not time-travelled by undo.
        // If you want, you can snapshot and restore capturedByWhite/Black similarly to pieces.
    }

    // Simple save/load
    void saveToFile(const string& fname) {
        ofstream ofs(fname);
        if (!ofs) { cerr << "Save failed\n"; return; }
        ofs << (whiteTurn ? 1 : 0) << "\n";
        ofs << pieces.size() << "\n";
        for (auto& p : pieces) ofs << p->getName() << " " << p->getRow() << " " << p->getCol() << " " << p->getHasMoved() << "\n";

        // persist captured pieces lists
        ofs << "CAPW " << capturedByWhite.size() << "\n";
        for (auto& s : capturedByWhite) ofs << s << "\n";
        ofs << "CAPB " << capturedByBlack.size() << "\n";
        for (auto& s : capturedByBlack) ofs << s << "\n";

        cerr << "Saved " << fname << "\n";
    }
    void loadFromFile(const string& fname) {
        ifstream ifs(fname);
        if (!ifs) { cerr << "Load failed\n"; return; }
        pieces.clear(); int wt; ifs >> wt; whiteTurn = wt == 1;
        int n; ifs >> n;
        for (int i = 0; i < n; ++i) { string nm; int r, c; bool hm; ifs >> nm >> r >> c >> hm; pieces.push_back(createPieceFromName(nm, r, c, textures)); pieces.back()->hasMoved = hm; }

        // try to read captured lists if present
        capturedByWhite.clear(); capturedByBlack.clear();
        string tag;
        if (ifs >> tag) {
            if (tag == "CAPW") {
                size_t m; ifs >> m;
                string line; getline(ifs, line); // consume endline
                for (size_t i = 0; i < m; ++i) { string s; getline(ifs, s); if (!s.empty()) capturedByWhite.push_back(s); }
                ifs >> tag;
            }
            if (tag == "CAPB") {
                size_t m; ifs >> m;
                string line; getline(ifs, line);
                for (size_t i = 0; i < m; ++i) { string s; getline(ifs, s); if (!s.empty()) capturedByBlack.push_back(s); }
            }
        }

        historyStates.clear(); saveState();
        state = playWithAI ? GameState::PLAYING_AI : GameState::PLAYING_FRIEND; endGameMessage.clear();
    }

    // Try to move pieceIndex to (toR,toC). Handles captures, en-passant, promotion.
    bool tryMove(int pieceIndex, int toR, int toC) {
        if (pieceIndex < 0 || pieceIndex >= (int)pieces.size()) return false;
        Piece* moving = pieces[pieceIndex].get();
        if (!moving->isValidMove(toR, toC, pieces)) return false;
        if (moving->isSquareOccupiedBySameColor(toR, toC, pieces)) return false;
        if (wouldMoveLeaveKingInCheck(pieceIndex, toR, toC, pieces, textures)) return false;

        // handle en-passant capture
        if (moving->getName().find("pawn") != string::npos && abs(toC - moving->getCol()) == 1 &&
            none_of(pieces.begin(), pieces.end(), [&](const unique_ptr<Piece>& p) { return p->getRow() == toR && p->getCol() == toC; })) {
            if (enPassantTarget.first == toR && enPassantTarget.second == toC) {
                int victimRow = moving->getRow(), victimCol = toC;
                for (int i = 0; i < (int)pieces.size(); ++i) {
                    if (pieces[i]->getRow() == victimRow && pieces[i]->getCol() == victimCol &&
                        pieces[i]->getName().find("pawn") != string::npos && pieces[i]->isWhite() != moving->isWhite()) {
                        // record captured name before erase
                        recordCapture(moving->isWhite(), pieces[i]->getName());
                        pieces.erase(pieces.begin() + i);
                        if (i < pieceIndex) --pieceIndex;
                        break;
                    }
                }
            }
        }

        // capture on destination
        for (int i = 0; i < (int)pieces.size(); ++i) {
            if (i != pieceIndex && pieces[i]->getRow() == toR && pieces[i]->getCol() == toC) {
                // record captured name before erase
                recordCapture(moving->isWhite(), pieces[i]->getName());
                pieces.erase(pieces.begin() + i);
                if (i < pieceIndex) --pieceIndex;
                break;
            }
        }

        // en-passant target update for double pawn move
        if (moving->getName().find("pawn") != string::npos && abs(toR - moving->getRow()) == 2) {
            int passedRow = (toR + moving->getRow()) / 2;
            enPassantTarget = { passedRow, toC };
        }
        else enPassantTarget = { -1,-1 };

        // perform move and auto-promote pawns to queen
        pieces[pieceIndex]->setPosition(toR, toC);
        if (pieces[pieceIndex]->getName().find("pawn") != string::npos) {
            if ((pieces[pieceIndex]->isWhite() && pieces[pieceIndex]->getRow() == 0) || (!pieces[pieceIndex]->isWhite() && pieces[pieceIndex]->getRow() == 7)) {
                string newName = pieces[pieceIndex]->isWhite() ? "w_queen" : "b_queen";
                pieces[pieceIndex] = createPieceFromName(newName, toR, toC, textures);
                pieces[pieceIndex]->hasMoved = true;
            }
        }

        saveState();
        return true;
    }

    // AI move generation and selection: prioritize check, highest capture, else random
    bool makeAIMove(bool aiWhite) {
        vector<Candidate> all;
        for (int i = 0; i < (int)pieces.size(); ++i) {
            if (pieces[i]->isWhite() != aiWhite) continue;
            for (int r = 0; r < BOARD_SIZE; ++r) for (int c = 0; c < BOARD_SIZE; ++c) {
                if (!pieces[i]->isValidMove(r, c, pieces)) continue;
                if (pieces[i]->isSquareOccupiedBySameColor(r, c, pieces)) continue;
                if (wouldMoveLeaveKingInCheck(i, r, c, pieces, textures)) continue;
                bool isCap = false; int capVal = 0;
                for (const auto& p : pieces) if (p->getRow() == r && p->getCol() == c && p->isWhite() != aiWhite) { isCap = true; capVal = pieceValue(p->getName()); break; }
                // simulate to see if gives check
                auto copy = clonePieces(pieces, textures);
                int idx = -1;
                for (int k = 0; k < (int)copy.size(); ++k) if (copy[k]->getRow() == pieces[i]->getRow() && copy[k]->getCol() == pieces[i]->getCol() && copy[k]->getName() == pieces[i]->getName()) { idx = k; break; }
                if (idx == -1) continue;
                for (int k = 0; k < (int)copy.size(); ++k) if (k != idx && copy[k]->getRow() == r && copy[k]->getCol() == c) { copy.erase(copy.begin() + k); if (k < idx) --idx; break; }
                copy[idx]->setPosition(r, c);
                bool givesCheck = isKingInCheck(!aiWhite, copy);
                all.push_back({ i,r,c,isCap,capVal,givesCheck });
            }
        }
        if (all.empty()) return false;
        // picks moves that give check
        vector<Candidate> checks;
        for (auto& c : all) if (c.givesCheck) checks.push_back(c);
        if (!checks.empty()) {
            uniform_int_distribution<int> d(0, (int)checks.size() - 1);
            Candidate sel = checks[d(rng)];
            return tryAndApplyAIMove(sel);
        }
        // else best capture
        int bestVal = 0;
        for (auto& c : all) if (c.isCapture) bestVal = max(bestVal, c.captureValue);
        if (bestVal > 0) {
            vector<Candidate> bestCaps;
            for (auto& c : all) if (c.isCapture && c.captureValue == bestVal) bestCaps.push_back(c);
            uniform_int_distribution<int> d(0, (int)bestCaps.size() - 1);
            Candidate sel = bestCaps[d(rng)];
            return tryAndApplyAIMove(sel);
        }
        // else random
        uniform_int_distribution<int> d(0, (int)all.size() - 1);
        Candidate sel = all[d(rng)];
        return tryAndApplyAIMove(sel);
    }

    // Apply AI candidate by mapping to current pieces and calling tryMove()
    bool tryAndApplyAIMove(const Candidate& cand) {
        // best-effort: use index from candidate if still valid; otherwise find same-name piece
        int idx = -1;
        if (cand.pieceIndex >= 0 && cand.pieceIndex < (int)pieces.size()) idx = cand.pieceIndex;
        if (idx == -1) {
            for (int i = 0; i < (int)pieces.size(); ++i) if (pieces[i]->getName() == pieces[cand.pieceIndex]->getName()) { idx = i; break; }
        }
        if (idx == -1) return false;
        return tryMove(idx, cand.tr, cand.tc);
    }

    // compute legal moves for a piece (take into account self-check)
    vector<tuple<int, int, bool>> computeLegalMovesForPiece(int index) {
        vector<tuple<int, int, bool>> out;
        if (index < 0 || index >= (int)pieces.size()) return out;
        for (int r = 0; r < BOARD_SIZE; ++r) for (int c = 0; c < BOARD_SIZE; ++c) {
            if (!pieces[index]->isValidMove(r, c, pieces)) continue;
            if (pieces[index]->isSquareOccupiedBySameColor(r, c, pieces)) continue;
            if (wouldMoveLeaveKingInCheck(index, r, c, pieces, textures)) continue;
            bool isCap = false;
            for (const auto& p : pieces) if (p->getRow() == r && p->getCol() == c && p->isWhite() != pieces[index]->isWhite()) { isCap = true; break; }
            out.emplace_back(r, c, isCap);
        }
        return out;
    }

    // Update squares' colors to default and mark king tiles red if in check
    void updateCheckHighlights() {
        // reset board colors
        for (int r = 0; r < BOARD_SIZE; ++r) {
            for (int c = 0; c < BOARD_SIZE; ++c) {
                bool light = (r + c) % 2 == 0;
                squares[r][c].setFillColor(light ? baseLightColor : baseDarkColor);
            }
        }
        // find white king and black king and mark attacked squares red
        auto wk = findKingPos(true, pieces);
        if (wk.first != -1) {
            if (isSquareAttacked(wk.first, wk.second, false, pieces)) {
                squares[wk.first][wk.second].setFillColor(sf::Color(220, 20, 60)); // red
            }
        }
        auto bk = findKingPos(false, pieces);
        if (bk.first != -1) {
            if (isSquareAttacked(bk.first, bk.second, true, pieces)) {
                squares[bk.first][bk.second].setFillColor(sf::Color(220, 20, 60)); // red
            }
        }
    }

    // Draw red outline around king(s) that are currently in check
    void drawKingCheckOutlines() {
        // white king attacked by black?
        auto wk = findKingPos(true, pieces);
        if (wk.first != -1 && isSquareAttacked(wk.first, wk.second, false, pieces)) {
            sf::RectangleShape rect(sf::Vector2f((float)TILE_SIZE - 8.f, (float)TILE_SIZE - 8.f));
            rect.setPosition((float)wk.second * TILE_SIZE + 4.f, (float)wk.first * TILE_SIZE + 4.f);
            rect.setFillColor(sf::Color(0, 0, 0, 0));
            rect.setOutlineColor(sf::Color(220, 20, 60));
            rect.setOutlineThickness(4.f);
            window.draw(rect);
        }
        // black king attacked by white?
        auto bk = findKingPos(false, pieces);
        if (bk.first != -1 && isSquareAttacked(bk.first, bk.second, true, pieces)) {
            sf::RectangleShape rect(sf::Vector2f((float)TILE_SIZE - 8.f, (float)TILE_SIZE - 8.f));
            rect.setPosition((float)bk.second * TILE_SIZE + 4.f, (float)bk.first * TILE_SIZE + 4.f);
            rect.setFillColor(sf::Color(0, 0, 0, 0));
            rect.setOutlineColor(sf::Color(220, 20, 60));
            rect.setOutlineThickness(4.f);
            window.draw(rect);
        }
    }

    // ---------- Main loop ----------
    void run() {
        while (window.isOpen()) {
            processEvents();

            // Handle AI thinking / timing (AI plays black when playWithAI)
            if (state == GameState::PLAYING_AI && !gameOver) {
                if (!whiteTurn) { // AI's turn (black)
                    if (!aiThinking) {
                        aiThinking = true; aiClock.restart();
                    }
                    else {
                        if (aiClock.getElapsedTime().asSeconds() >= AI_THINK_SECONDS) {
                            bool moved = makeAIMove(false);
                            aiThinking = false;
                            if (moved) {
                                whiteTurn = !whiteTurn;
                                // check for checks & endgame
                                if (isKingInCheck(whiteTurn, pieces)) { /* optional console message */ }
                                if (!hasLegalMove(whiteTurn, pieces, textures)) {
                                    if (isKingInCheck(whiteTurn, pieces)) endGameMessage = string((whiteTurn ? "White" : "Black")) + " - Checkmate";
                                    else endGameMessage = "Draw - Stalemate";
                                    gameOver = true; state = GameState::GAME_OVER;
                                }
                            }
                            else {
                                // AI had no move
                                if (isKingInCheck(false, pieces)) endGameMessage = "White - Checkmate";
                                else endGameMessage = "Draw - Stalemate";
                                gameOver = true; state = GameState::GAME_OVER;
                            }
                        }
                    }
                }
                else {
                    aiThinking = false;
                }
            }

            render();
        }
    }

    // ---------- Event handling ----------
    void processEvents() {
        sf::Event e;
        while (window.pollEvent(e)) {
            if (e.type == sf::Event::Closed) window.close();
            if (state == GameState::MAIN_MENU) handleMenuEvent(e);
            else if (state == GameState::PLAYING_FRIEND || state == GameState::PLAYING_AI) handleGameEvent(e);
            else if (state == GameState::GAME_OVER) handleGameOverEvent(e);
        }
    }

    // Main menu events (hover + click on centered buttons)
    void handleMenuEvent(const sf::Event& e) {
        if (e.type == sf::Event::MouseMoved) {
            playFriendBtn.updateHover((sf::Vector2f)sf::Mouse::getPosition(window));
            playAIBtn.updateHover((sf::Vector2f)sf::Mouse::getPosition(window));
            exitBtn.updateHover((sf::Vector2f)sf::Mouse::getPosition(window));
        }
        if (e.type == sf::Event::MouseButtonPressed && e.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2f mp = window.mapPixelToCoords({ e.mouseButton.x,e.mouseButton.y });
            if (playFriendBtn.isClicked(mp)) startNewGame(false);
            else if (playAIBtn.isClicked(mp)) startNewGame(true);
            else if (exitBtn.isClicked(mp)) window.close();
        }
    }

    // Game-over screen events (Back to menu button in sidebar)
    void handleGameOverEvent(const sf::Event& e) {
        if (e.type == sf::Event::MouseButtonPressed && e.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2f mp = window.mapPixelToCoords({ e.mouseButton.x,e.mouseButton.y });
            if (backToMenuBtn.isClicked(mp)) {
                state = GameState::MAIN_MENU;
                endGameMessage.clear();
            }
        }
        if (e.type == sf::Event::KeyPressed) {
            if (e.key.code == sf::Keyboard::R) state = GameState::MAIN_MENU;
            if (e.key.code == sf::Keyboard::U) undo();
        }
    }

    // Handle gameplay events: drag/drop, keyboard shortcuts
    void handleGameEvent(const sf::Event& e) {
        if (e.type == sf::Event::MouseButtonPressed && e.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2f mp = window.mapPixelToCoords({ e.mouseButton.x,e.mouseButton.y });
            float sx = TILE_SIZE * BOARD_SIZE;
            // check sidebar back button click first
            if (mp.x >= sx) {
                if (backToMenuBtn.isClicked(mp)) { state = GameState::MAIN_MENU; return; }
            }
            // otherwise pick topmost piece on clicked square
            int c = static_cast<int>(mp.x) / TILE_SIZE;
            int r = static_cast<int>(mp.y) / TILE_SIZE;
            if (r >= 0 && r < BOARD_SIZE && c >= 0 && c < BOARD_SIZE) {
                for (int i = (int)pieces.size() - 1; i >= 0; --i) {
                    if (pieces[i]->getRow() == r && pieces[i]->getCol() == c) {
                        if (pieces[i]->isWhite() == whiteTurn) {
                            dragging = true; draggedIndex = i;
                            dragOffset = pieces[i]->getSprite().getPosition() - mp;
                        }
                        break;
                    }
                }
            }
        }
        else if (e.type == sf::Event::MouseMoved) {
            if (dragging && draggedIndex != -1) {
                sf::Vector2f mp = window.mapPixelToCoords({ e.mouseMove.x, e.mouseMove.y });
                pieces[draggedIndex]->getSprite().setPosition(mp + dragOffset);
            }
        }
        else if (e.type == sf::Event::MouseButtonReleased && e.mouseButton.button == sf::Mouse::Left) {
            if (!dragging) return;
            sf::Vector2f mp = window.mapPixelToCoords({ e.mouseButton.x,e.mouseButton.y });
            int c = static_cast<int>((mp.x + dragOffset.x) / TILE_SIZE);
            int r = static_cast<int>((mp.y + dragOffset.y) / TILE_SIZE);
            r = clamp(r, 0, BOARD_SIZE - 1);
            c = clamp(c, 0, BOARD_SIZE - 1);

            bool ok = tryMove(draggedIndex, r, c);
            if (ok) {
                whiteTurn = !whiteTurn;
                if (playWithAI && !whiteTurn) { aiThinking = true; aiClock.restart(); }
                if (isKingInCheck(whiteTurn, pieces)) { /* check notification can go here */ }
                if (!hasLegalMove(whiteTurn, pieces, textures)) {
                    if (isKingInCheck(whiteTurn, pieces)) endGameMessage = string((whiteTurn ? "White" : "Black")) + " - Checkmate";
                    else endGameMessage = "Draw - Stalemate";
                    gameOver = true; state = GameState::GAME_OVER;
                }
            }
            else {
                // invalid move -> snap back
                if (draggedIndex >= 0 && draggedIndex < (int)pieces.size()) pieces[draggedIndex]->setPosition(pieces[draggedIndex]->getRow(), pieces[draggedIndex]->getCol());
            }
            dragging = false; draggedIndex = -1;
        }
        else if (e.type == sf::Event::KeyPressed) {
            if (e.key.code == sf::Keyboard::U) undo();
            else if (e.key.code == sf::Keyboard::S) saveToFile("chess_save.txt");
            else if (e.key.code == sf::Keyboard::L) loadFromFile("chess_save.txt");
            else if (e.key.code == sf::Keyboard::R) startNewGame(playWithAI);
            else if (e.key.code == sf::Keyboard::Escape) state = GameState::MAIN_MENU;
        }
    }

    // ---------- Rendering ----------
    void render() {
        window.clear(sf::Color(40, 40, 40));
        if (state == GameState::MAIN_MENU) renderMainMenu();
        else {
            renderBoardAndSidebar();
            // highlight valid moves for current dragged piece
            if (dragging && draggedIndex != -1) drawHighlightsForPiece(draggedIndex);
            // draw king outlines if in check (on top of pieces)
            drawKingCheckOutlines();
            // if game over, overlay message
            if (state == GameState::GAME_OVER) renderGameOverOverlay();
        }
        window.display();
    }

    // Render main menu: soft background, truly centered title & buttons
    void renderMainMenu() {
        // pleasant background
        sf::RectangleShape bg(sf::Vector2f((float)window.getSize().x, (float)window.getSize().y));
        bg.setFillColor(sf::Color(230, 240, 230));
        window.draw(bg);

        // center title horizontally; place a bit above center
        sf::FloatRect tb = titleText.getLocalBounds();
        float centerX = window.getSize().x / 2.f;
        float centerY = window.getSize().y / 2.f;

        titleText.setPosition(centerX - tb.width / 2.f, centerY - 200.f);
        titleText.setFillColor(sf::Color(20, 20, 20));
        window.draw(titleText);

        // subtitle centered under title
        sf::FloatRect sb = subText.getLocalBounds();
        subText.setPosition(centerX - sb.width / 2.f, titleText.getPosition().y + tb.height + 18.f);
        subText.setFillColor(sf::Color(60, 60, 60));
        window.draw(subText);

        // draw buttons (positions defined relative to full window center in setupMenuUI)
        playFriendBtn.draw(window);
        playAIBtn.draw(window);
        exitBtn.draw(window);

        // footer centered at bottom
        sf::FloatRect fb = footerText.getLocalBounds();
        footerText.setPosition(centerX - fb.width / 2.f, (float)window.getSize().y - 30.f);
        footerText.setFillColor(sf::Color(80, 80, 80));
        window.draw(footerText);
    }

    // Render board and right sidebar
    void renderBoardAndSidebar() {
        // update check-highlights before drawing squares
        updateCheckHighlights();

        // board squares
        for (int r = 0; r < BOARD_SIZE; ++r) for (int c = 0; c < BOARD_SIZE; ++c) window.draw(squares[r][c]);
        // pieces
        for (int i = 0; i < (int)pieces.size(); ++i) if (i != draggedIndex) window.draw(pieces[i]->getSprite());
        if (draggedIndex != -1 && draggedIndex < (int)pieces.size()) window.draw(pieces[draggedIndex]->getSprite());
        // sidebar contents (turn, mode, captured pieces, controls, back button)
        drawSidebar();
    }

    // Draw small sprites in rows inside the sidebar for captured pieces
    void drawCapturedRow(float startX, float& y, const vector<string>& captured, float iconSize = 36.f, float padding = 6.f) {
        float x = startX;
        float maxW = (float)SIDEBAR_WIDTH - 24.f;
        float step = iconSize + padding;
        for (size_t i = 0; i < captured.size(); ++i) {
            auto it = textures.find(captured[i]);
            if (it == textures.end()) continue;
            sf::Sprite s(it->second);
            // scale sprite to iconSize
            sf::FloatRect lb = s.getLocalBounds();
            if (lb.width > 0.f && lb.height > 0.f) {
                float scale = min(iconSize / lb.width, iconSize / lb.height);
                s.setScale(scale, scale);
            }
            s.setPosition(startX + (float)((int)(x - startX) % (int)maxW), y);
            window.draw(s);

            x += step;
            if (x - startX + iconSize > maxW) { // wrap
                x = startX;
                y += iconSize + padding;
            }
        }
        y += iconSize + padding * 0.5f; // extra space after the row block
    }

    // Draw sidebar: turn, mode, captured pieces, controls, back button
    void drawSidebar() {
        float sx = TILE_SIZE * BOARD_SIZE;
        sf::RectangleShape sidebar(sf::Vector2f((float)SIDEBAR_WIDTH, (float)TILE_SIZE * BOARD_SIZE));
        sidebar.setPosition(sx, 0);
        sidebar.setFillColor(sf::Color(40, 40, 40));
        window.draw(sidebar);

        float y = 12.f;

        // Turn
        sf::Text tturn("Turn: " + string(whiteTurn ? "White" : "Black"), font, 20);
        tturn.setFillColor(sf::Color::White);
        tturn.setPosition(sx + 12.f, y);
        window.draw(tturn);
        y += 28.f;

        // Mode
        sf::Text tmode("Mode: " + string(playWithAI ? "Play vs AI" : "2 Players"), font, 16);
        tmode.setFillColor(sf::Color(200, 200, 200));
        tmode.setPosition(sx + 12.f, y);
        window.draw(tmode);
        y += 34.f;

        // ---- Captured by White (i.e., black pieces taken) ----
        {
            sf::Text head("Captured by White", font, 16);
            head.setFillColor(sf::Color(255, 215, 0));
            head.setPosition(sx + 12.f, y);
            window.draw(head);
            y += 22.f;
            float rowY = y;
            drawCapturedRow(sx + 12.f, rowY, capturedByWhite, 36.f, 6.f);
            y = rowY + 6.f;
        }

        // ---- Captured by Black (i.e., white pieces taken) ----
        {
            sf::Text head("Captured by Black", font, 16);
            head.setFillColor(sf::Color(173, 216, 230));
            head.setPosition(sx + 12.f, y);
            window.draw(head);
            y += 22.f;
            float rowY = y;
            drawCapturedRow(sx + 12.f, rowY, capturedByBlack, 36.f, 6.f);
            y = rowY + 6.f;
        }

        // Controls
        sf::Text ctrl("Controls:\nDrag pieces\nU:Undo  S:Save  L:Load\nR:Restart  Esc:Menu", font, 14);
        ctrl.setFillColor(sf::Color::White);
        ctrl.setPosition(sx + 12.f, max(y + 8.f, (float)TILE_SIZE * BOARD_SIZE - 180.f));
        window.draw(ctrl);

        // Back to menu button inside sidebar
        backToMenuBtn.draw(window);
    }

    // draw highlight markers for legal moves of selected piece
    void drawHighlightsForPiece(int pieceIndex) {
        auto moves = computeLegalMovesForPiece(pieceIndex);
        for (auto& t : moves) {
            int r = get<0>(t), c = get<1>(t); bool isCap = get<2>(t);
            sf::Vector2f center((c + 0.5f) * TILE_SIZE, (r + 0.5f) * TILE_SIZE);
            if (!isCap) {
                // small filled circle
                sf::CircleShape circle(TILE_SIZE * 0.12f);
                circle.setOrigin(circle.getRadius(), circle.getRadius());
                circle.setPosition(center);
                circle.setFillColor(sf::Color(30, 180, 30, 180)); // semi-transparent green
                window.draw(circle);
            }
            else {
                // red capture rectangle outline
                sf::RectangleShape rect(sf::Vector2f((float)TILE_SIZE - 8.f, (float)TILE_SIZE - 8.f));
                rect.setPosition((float)c * TILE_SIZE + 4.f, (float)r * TILE_SIZE + 4.f);
                rect.setFillColor(sf::Color(0, 0, 0, 0));
                rect.setOutlineColor(sf::Color(220, 20, 60)); rect.setOutlineThickness(4.f);
                window.draw(rect);
            }
        }
    }

    // render overlay explaining end-of-game (who won + reason)
    void renderGameOverOverlay() {
        // darken board area
        sf::RectangleShape overlay(sf::Vector2f((float)TILE_SIZE * BOARD_SIZE, (float)TILE_SIZE * BOARD_SIZE));
        overlay.setFillColor(sf::Color(0, 0, 0, 150));
        window.draw(overlay);

        // main message
        string msg = endGameMessage.empty() ? "Game Over" : endGameMessage;
        sf::Text t(msg, font, 48); t.setFillColor(sf::Color::White);
        sf::FloatRect tb = t.getLocalBounds();
        t.setPosition((TILE_SIZE * BOARD_SIZE - tb.width) / 2.f, (TILE_SIZE * BOARD_SIZE - tb.height) / 2.f - 40.f);
        window.draw(t);

        // reason below
        string reason;
        if (msg.find("Checkmate") != string::npos) reason = "Checkmate — King is attacked and has no legal moves.";
        else if (msg.find("Stalemate") != string::npos) reason = "Stalemate — side has no legal moves but is not in check.";
        else reason = "Game finished.";
        sf::Text rtext(reason, font, 18); rtext.setFillColor(sf::Color::White);
        sf::FloatRect rb = rtext.getLocalBounds();
        rtext.setPosition((TILE_SIZE * BOARD_SIZE - rb.width) / 2.f, (TILE_SIZE * BOARD_SIZE - rb.height) / 2.f + 20.f);
        window.draw(rtext);
    }

private:
    // helper to push captured piece into the correct list
    void recordCapture(bool capturerIsWhite, const string& capturedName) {
        // Normalize captured name to texture keys (already like "b_pawn" etc.)
        if (capturerIsWhite) capturedByWhite.push_back(capturedName); // white captured black piece
        else capturedByBlack.push_back(capturedName);                 // black captured white piece
    }
};

// ---------- main ----------
int main() {
    try {
        ChessGame game;
        game.run();
    }
    catch (const exception& ex) {
        cerr << "Fatal: " << ex.what() << endl;
        return 1;
    }
    return 0;
}
