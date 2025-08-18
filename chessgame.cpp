<<<<<<< HEAD
#include "ChessGame.h"
#include <algorithm>
#include <ctime>
#include <cmath>

// -------------------- Local helpers (free functions) --------------------

namespace {

    // board helpers
    inline bool onBoard(int r, int c) {
        return r >= 0 && r < Chess::BOARD_SIZE && c >= 0 && c < Chess::BOARD_SIZE;
    }

    int findPieceIndexAt(int r, int c, const std::vector<std::unique_ptr<Piece>>& pieces) {
        for (int i = 0; i < static_cast<int>(pieces.size()); ++i) {
            if (pieces[i]->getRow() == r && pieces[i]->getCol() == c) return i;
        }
        return -1;
    }

    int findKingIndex(bool white, const std::vector<std::unique_ptr<Piece>>& pieces) {
        const std::string target = white ? "w_king" : "b_king";
        for (int i = 0; i < static_cast<int>(pieces.size()); ++i) {
            if (pieces[i]->getName() == target) return i;
        }
        return -1;
    }

    // convert pixel -> board
    sf::Vector2i pixelToBoard(const sf::RenderWindow& window, const sf::Vector2i& pixel) {
        sf::Vector2f world = window.mapPixelToCoords(pixel);
        int c = static_cast<int>(world.x) / Chess::TILE_SIZE;
        int r = static_cast<int>(world.y) / Chess::TILE_SIZE;
        return { r, c };
    }

    // sprite for a given piece name at tile
    sf::Sprite makePieceSprite(const std::map<std::string, sf::Texture>& textures,
        const std::string& name, int r, int c, float offsetX = 0.f, float offsetY = 0.f) {
        sf::Sprite s;
        auto it = textures.find(name);
        if (it != textures.end()) s.setTexture(it->second);
        s.setPosition(static_cast<float>(c * Chess::TILE_SIZE) + offsetX,
            static_cast<float>(r * Chess::TILE_SIZE) + offsetY);
        // optional fit: assume your texture is already sized for the tile; otherwise scale here
        return s;
    }

    // lightweight addPiece used by startNewGame (unqualified name resolves to this free function)
    void addPiece(const std::string& name, int r, int c,
        const std::map<std::string, sf::Texture>& textures,
        std::vector<std::unique_ptr<Piece>>& pieces) {
        pieces.push_back(createPieceFromName(name, r, c, textures));
    }

} // namespace

ChessGame::ChessGame() :
    window(sf::VideoMode(Chess::TILE_SIZE* Chess::BOARD_SIZE + Chess::SIDEBAR_WIDTH,
        Chess::TILE_SIZE* Chess::BOARD_SIZE), "SFML Chess"),
    rng(static_cast<unsigned>(std::time(nullptr)))
{
    if (!font.loadFromFile("arial-font/arial.ttf")) {
        throw std::runtime_error("Failed to load font arial.ttf");
    }
=======
#include "chessgame.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <ctime>
#include <iostream>

Button::Button(const string& label, sf::Font& font, sf::Vector2f pos, sf::Vector2f size, unsigned int chsize) {
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

ChessGame::ChessGame() : window(sf::VideoMode(TILE_SIZE* BOARD_SIZE + SIDEBAR_WIDTH, TILE_SIZE* BOARD_SIZE), "SFML Chess") {
    rng.seed((unsigned)time(nullptr));
    if (!font.loadFromFile("arial-font/arial.ttf")) throw runtime_error("Failed to load font arial.ttf - place in exe folder or change path.");
>>>>>>> 11ed285900d2ae6925459c826014a259e6eb0b8a
    loadTextures();
    setupBoardUI();
    setupMenuUI();
}

<<<<<<< HEAD
void ChessGame::run() {
    while (window.isOpen()) {
        processEvents();

        // AI move logic (AI plays Black by default here)
        if (state == GameState::PLAYING_AI && !whiteTurn && !gameOver) {
            if (!aiThinking) {
                aiThinking = true;
                aiClock.restart();
            }
            else if (aiClock.getElapsedTime().asSeconds() >= Chess::AI_THINK_SECONDS) {
                if (makeAIMove(false)) {
                    whiteTurn = !whiteTurn;
                    if (isKingInCheck(whiteTurn, pieces)) {
                        if (!hasLegalMove(whiteTurn, pieces, textures)) {
                            endGameMessage = whiteTurn ? "Black wins - Checkmate" : "White wins - Checkmate";
                            gameOver = true;
                            state = GameState::GAME_OVER;
                        }
                    }
                    else if (!hasLegalMove(whiteTurn, pieces, textures)) {
                        endGameMessage = "Draw - Stalemate";
                        gameOver = true;
                        state = GameState::GAME_OVER;
                    }
                }
                aiThinking = false;
            }
        }

        render();
    }
}

// -------------------- loading & setup --------------------

void ChessGame::loadTextures() {
    std::vector<std::string> names = {
        "w_pawn", "w_rook", "w_knight", "w_bishop", "w_queen", "w_king",
        "b_pawn", "b_rook", "b_knight", "b_bishop", "b_queen", "b_king"
    };

    for (const auto& name : names) {
        sf::Texture texture;
        if (!texture.loadFromFile("Assets/" + name + ".png")) {
            std::cerr << "Failed to load texture: " << name << ".png\n";
            texture.create(1, 1); // placeholder
        }
        textures[name] = texture;
=======
void ChessGame::loadTextures() {
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
>>>>>>> 11ed285900d2ae6925459c826014a259e6eb0b8a
    }
}

void ChessGame::setupBoardUI() {
<<<<<<< HEAD
    for (int r = 0; r < Chess::BOARD_SIZE; ++r) {
        for (int c = 0; c < Chess::BOARD_SIZE; ++c) {
            squares[r][c].setSize(sf::Vector2f(Chess::TILE_SIZE, Chess::TILE_SIZE));
            squares[r][c].setPosition(c * Chess::TILE_SIZE, r * Chess::TILE_SIZE);
            squares[r][c].setFillColor((r + c) % 2 == 0 ? baseLightColor : baseDarkColor);
        }
    }

    titleText.setFont(font);
    titleText.setString("SFML Chess");
    titleText.setCharacterSize(64);
    titleText.setFillColor(sf::Color::Black);

    subText.setFont(font);
    subText.setString("Choose mode");
    subText.setCharacterSize(26);
    subText.setFillColor(sf::Color(20, 20, 20));

    footerText.setFont(font);
    footerText.setString("Drag pieces | U:Undo  S:Save  L:Load  R:Restart  Esc:Menu");
    footerText.setCharacterSize(14);
    footerText.setFillColor(sf::Color(30, 30, 30));
}

void ChessGame::setupMenuUI() {
    const float btnW = 360.f, btnH = 72.f;
    const float centerX = window.getSize().x / 2.f;
    const float centerY = window.getSize().y / 2.f;

    playFriendBtn = Button("Play with Friend", font,
        { centerX - btnW / 2, centerY - btnH - 10.f }, { btnW, btnH });
    playAIBtn = Button("Play with AI", font,
        { centerX - btnW / 2, centerY + 10.f }, { btnW, btnH });
    exitBtn = Button("Exit", font,
        { centerX - btnW / 2, centerY + btnH + 30.f }, { btnW, btnH });

    const float sx = Chess::TILE_SIZE * Chess::BOARD_SIZE;
    backToMenuBtn = Button("Back to Menu", font,
        { sx + 12.f, static_cast<float>(window.getSize().y) - 80.f },
        { Chess::SIDEBAR_WIDTH - 24.f, 60.f }, 22);
}

// -------------------- game flow --------------------

void ChessGame::startNewGame(bool vsAI) {
    pieces.clear();
    playWithAI = vsAI;
    whiteTurn = true;
    gameOver = false;
    endGameMessage.clear();
    historyStates.clear();
    capturedByWhite.clear();
    capturedByBlack.clear();
    enPassantTarget = { -1, -1 };
    draggedIndex = -1;
    dragging = false;

    // Black back rank
    addPiece("b_rook", 0, 0, textures, pieces);
    addPiece("b_knight", 0, 1, textures, pieces);
    addPiece("b_bishop", 0, 2, textures, pieces);
    addPiece("b_queen", 0, 3, textures, pieces);
    addPiece("b_king", 0, 4, textures, pieces);
    addPiece("b_bishop", 0, 5, textures, pieces);
    addPiece("b_knight", 0, 6, textures, pieces);
    addPiece("b_rook", 0, 7, textures, pieces);
    for (int c = 0; c < 8; ++c) addPiece("b_pawn", 1, c, textures, pieces);

    // White back rank
    addPiece("w_rook", 7, 0, textures, pieces);
    addPiece("w_knight", 7, 1, textures, pieces);
    addPiece("w_bishop", 7, 2, textures, pieces);
    addPiece("w_queen", 7, 3, textures, pieces);
    addPiece("w_king", 7, 4, textures, pieces);
    addPiece("w_bishop", 7, 5, textures, pieces);
    addPiece("w_knight", 7, 6, textures, pieces);
    addPiece("w_rook", 7, 7, textures, pieces);
    for (int c = 0; c < 8; ++c) addPiece("w_pawn", 6, c, textures, pieces);
=======
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

void ChessGame::setupMenuUI() {
    float btnW = 360.f, btnH = 72.f;
    float centerX = window.getSize().x / 2.f;
    float centerY = window.getSize().y / 2.f;

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

void ChessGame::addPiece(const string& name, int r, int c) {
    pieces.push_back(createPieceFromName(name, r, c, textures));
}

void ChessGame::startNewGame(bool vsAI) {
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
>>>>>>> 11ed285900d2ae6925459c826014a259e6eb0b8a

    saveState();
    state = vsAI ? GameState::PLAYING_AI : GameState::PLAYING_FRIEND;
}

<<<<<<< HEAD
void ChessGame::saveState() {
    std::vector<std::unique_ptr<Piece>> snapshot;
    snapshot.reserve(pieces.size());
    for (const auto& p : pieces) {
        snapshot.push_back(createPieceFromName(p->getName(), p->getRow(), p->getCol(), textures));
    }
    historyStates.push_back(std::move(snapshot));
}

void ChessGame::undo() {
    if (historyStates.size() <= 1) return; // nothing to undo (keep at least initial)
    historyStates.pop_back();
    pieces.clear();
    for (const auto& p : historyStates.back()) {
        pieces.push_back(createPieceFromName(p->getName(), p->getRow(), p->getCol(), textures));
    }
    // Reset some transient state
    gameOver = false;
    state = playWithAI ? GameState::PLAYING_AI : GameState::PLAYING_FRIEND;
    whiteTurn = !whiteTurn; // undo flips turn back
    enPassantTarget = { -1, -1 }; // conservative reset
}

void ChessGame::saveToFile(const std::string& fname) {
    std::ofstream out(fname);
    if (!out) return;
    // simple format: first line: whiteTurn playWithAI
    out << (whiteTurn ? 1 : 0) << " " << (playWithAI ? 1 : 0) << "\n";
    // pieces
    for (const auto& p : pieces) {
        out << p->getName() << " " << p->getRow() << " " << p->getCol() << "\n";
    }
}

void ChessGame::loadFromFile(const std::string& fname) {
    std::ifstream in(fname);
    if (!in) return;
    pieces.clear();

    int wt = 1, ai = 0;
    in >> wt >> ai;
    whiteTurn = (wt != 0);
    playWithAI = (ai != 0);

    std::string name;
    int r, c;
    while (in >> name >> r >> c) {
        pieces.push_back(createPieceFromName(name, r, c, textures));
    }
    historyStates.clear();
    saveState();
    gameOver = false;
    state = playWithAI ? GameState::PLAYING_AI : GameState::PLAYING_FRIEND;
}

// returns true if move applied
bool ChessGame::tryMove(int pieceIndex, int toR, int toC) {
    if (pieceIndex < 0 || pieceIndex >= static_cast<int>(pieces.size())) return false;

=======
void ChessGame::saveState() { historyStates.push_back(clonePieces(pieces, textures)); }

void ChessGame::undo() {
    if (historyStates.size() < 2) return;
    historyStates.pop_back();
    pieces = clonePieces(historyStates.back(), textures);
    whiteTurn = !whiteTurn;
    gameOver = false; aiThinking = false; enPassantTarget = { -1,-1 }; endGameMessage.clear();
}

void ChessGame::saveToFile(const string& fname) {
    ofstream ofs(fname);
    if (!ofs) { cerr << "Save failed\n"; return; }
    ofs << (whiteTurn ? 1 : 0) << "\n";
    ofs << pieces.size() << "\n";
    for (auto& p : pieces) ofs << p->getName() << " " << p->getRow() << " " << p->getCol() << " " << p->getHasMoved() << "\n";

    ofs << "CAPW " << capturedByWhite.size() << "\n";
    for (auto& s : capturedByWhite) ofs << s << "\n";
    ofs << "CAPB " << capturedByBlack.size() << "\n";
    for (auto& s : capturedByBlack) ofs << s << "\n";

    cerr << "Saved " << fname << "\n";
}

void ChessGame::loadFromFile(const string& fname) {
    ifstream ifs(fname);
    if (!ifs) { cerr << "Load failed\n"; return; }
    pieces.clear(); int wt; ifs >> wt; whiteTurn = wt == 1;
    int n; ifs >> n;
    for (int i = 0; i < n; ++i) { string nm; int r, c; bool hm; ifs >> nm >> r >> c >> hm; pieces.push_back(createPieceFromName(nm, r, c, textures)); pieces.back()->hasMoved = hm; }

    capturedByWhite.clear(); capturedByBlack.clear();
    string tag;
    if (ifs >> tag) {
        if (tag == "CAPW") {
            size_t m; ifs >> m;
            string line; getline(ifs, line);
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

bool ChessGame::tryMove(int pieceIndex, int toR, int toC) {
    if (pieceIndex < 0 || pieceIndex >= (int)pieces.size()) return false;
>>>>>>> 11ed285900d2ae6925459c826014a259e6eb0b8a
    Piece* moving = pieces[pieceIndex].get();
    if (!moving->isValidMove(toR, toC, pieces)) return false;
    if (moving->isSquareOccupiedBySameColor(toR, toC, pieces)) return false;
    if (wouldMoveLeaveKingInCheck(pieceIndex, toR, toC, pieces, textures)) return false;

<<<<<<< HEAD
    // Handle en passant
    if (moving->getName().find("pawn") != std::string::npos &&
        std::abs(toC - moving->getCol()) == 1 &&
        toR == enPassantTarget.first && toC == enPassantTarget.second) {
        int victimRow = moving->getRow();
        int victimCol = toC;
        for (int i = 0; i < static_cast<int>(pieces.size()); ++i) {
            if (pieces[i]->getRow() == victimRow &&
                pieces[i]->getCol() == victimCol &&
                pieces[i]->getName().find("pawn") != std::string::npos) {
                recordCapture(moving->isWhite(), pieces[i]->getName());
                pieces.erase(pieces.begin() + i);
                if (i < pieceIndex) --pieceIndex;
                break;
=======
    if (moving->getName().find("pawn") != string::npos && abs(toC - moving->getCol()) == 1 &&
        none_of(pieces.begin(), pieces.end(), [&](const unique_ptr<Piece>& p) { return p->getRow() == toR && p->getCol() == toC; })) {
        if (enPassantTarget.first == toR && enPassantTarget.second == toC) {
            int victimRow = moving->getRow(), victimCol = toC;
            for (int i = 0; i < (int)pieces.size(); ++i) {
                if (pieces[i]->getRow() == victimRow && pieces[i]->getCol() == victimCol &&
                    pieces[i]->getName().find("pawn") != string::npos && pieces[i]->isWhite() != moving->isWhite()) {
                    recordCapture(moving->isWhite(), pieces[i]->getName());
                    pieces.erase(pieces.begin() + i);
                    if (i < pieceIndex) --pieceIndex;
                    break;
                }
>>>>>>> 11ed285900d2ae6925459c826014a259e6eb0b8a
            }
        }
    }

<<<<<<< HEAD
    // Normal captures
    for (int i = 0; i < static_cast<int>(pieces.size()); ++i) {
=======
    for (int i = 0; i < (int)pieces.size(); ++i) {
>>>>>>> 11ed285900d2ae6925459c826014a259e6eb0b8a
        if (i != pieceIndex && pieces[i]->getRow() == toR && pieces[i]->getCol() == toC) {
            recordCapture(moving->isWhite(), pieces[i]->getName());
            pieces.erase(pieces.begin() + i);
            if (i < pieceIndex) --pieceIndex;
            break;
        }
    }

<<<<<<< HEAD
    // Update en passant target
    if (moving->getName().find("pawn") != std::string::npos &&
        std::abs(toR - moving->getRow()) == 2) {
        enPassantTarget = { (toR + moving->getRow()) / 2, toC };
    }
    else {
        enPassantTarget = { -1, -1 };
    }

    // Move the piece
    pieces[pieceIndex]->setPosition(toR, toC);

    // Pawn promotion
    if (pieces[pieceIndex]->getName().find("pawn") != std::string::npos) {
        if ((pieces[pieceIndex]->isWhite() && toR == 0) ||
            (!pieces[pieceIndex]->isWhite() && toR == 7)) {
            std::string newName = pieces[pieceIndex]->isWhite() ? "w_queen" : "b_queen";
            pieces[pieceIndex] = createPieceFromName(newName, toR, toC, textures);
=======
    if (moving->getName().find("pawn") != string::npos && abs(toR - moving->getRow()) == 2) {
        int passedRow = (toR + moving->getRow()) / 2;
        enPassantTarget = { passedRow, toC };
    }
    else enPassantTarget = { -1,-1 };

    pieces[pieceIndex]->setPosition(toR, toC);
    if (pieces[pieceIndex]->getName().find("pawn") != string::npos) {
        if ((pieces[pieceIndex]->isWhite() && pieces[pieceIndex]->getRow() == 0) || (!pieces[pieceIndex]->isWhite() && pieces[pieceIndex]->getRow() == 7)) {
            string newName = pieces[pieceIndex]->isWhite() ? "w_queen" : "b_queen";
            pieces[pieceIndex] = createPieceFromName(newName, toR, toC, textures);
            pieces[pieceIndex]->hasMoved = true;
>>>>>>> 11ed285900d2ae6925459c826014a259e6eb0b8a
        }
    }

    saveState();
    return true;
}

<<<<<<< HEAD
// -------------------- AI --------------------

std::vector<std::tuple<int, int, bool>> ChessGame::computeLegalMovesForPiece(int index) {
    std::vector<std::tuple<int, int, bool>> moves; // (r,c,isCapture)
    if (index < 0 || index >= static_cast<int>(pieces.size())) return moves;
    const auto& p = pieces[index];
    for (int r = 0; r < Chess::BOARD_SIZE; ++r) {
        for (int c = 0; c < Chess::BOARD_SIZE; ++c) {
            if (!p->isValidMove(r, c, pieces)) continue;
            if (p->isSquareOccupiedBySameColor(r, c, pieces)) continue;
            if (wouldMoveLeaveKingInCheck(index, r, c, pieces, textures)) continue;
            bool isCap = (findPieceIndexAt(r, c, pieces) != -1);
            moves.emplace_back(r, c, isCap);
        }
    }
    return moves;
}

bool ChessGame::tryAndApplyAIMove(const Candidate& cand) {
    return tryMove(cand.pieceIndex, cand.tr, cand.tc);
}

bool ChessGame::makeAIMove(bool aiWhite) {
    // Collect candidates
    std::vector<Candidate> candidates;

    auto valueOf = [](const std::string& name) -> int {
        // basic material heuristic
        if (name.find("pawn") != std::string::npos) return 1;
        if (name.find("knight") != std::string::npos) return 3;
        if (name.find("bishop") != std::string::npos) return 3;
        if (name.find("rook") != std::string::npos) return 5;
        if (name.find("queen") != std::string::npos) return 9;
        if (name.find("king") != std::string::npos) return 100;
        return 0;
        };

    for (int i = 0; i < static_cast<int>(pieces.size()); ++i) {
        if (pieces[i]->isWhite() != aiWhite) continue;
        auto legal = computeLegalMovesForPiece(i);
        for (auto& mv : legal) {
            int r, c; bool isCap;
            std::tie(r, c, isCap) = mv;
            int capVal = 0;
            bool givesChk = false;

            if (isCap) {
                int idx = findPieceIndexAt(r, c, pieces);
                if (idx != -1) capVal = valueOf(pieces[idx]->getName());
            }

            // check if this move gives check (simulate minimally)
            // Duplicate the board
            std::vector<std::unique_ptr<Piece>> sim;
            sim.reserve(pieces.size());
            for (const auto& p : pieces)
                sim.push_back(createPieceFromName(p->getName(), p->getRow(), p->getCol(), textures));

            // apply sim move
            // remove captured
            for (int k = 0; k < static_cast<int>(sim.size()); ++k) {
                if (k != i && sim[k] && sim[k]->getRow() == r && sim[k]->getCol() == c) {
                    sim.erase(sim.begin() + k);
                    if (k < i) --i; // adjust if needed, but we won't use i afterward
                    break;
                }
            }
            // move
            sim[i]->setPosition(r, c);
            givesChk = isKingInCheck(!aiWhite, sim);

            candidates.push_back({ i, r, c, isCap, capVal, givesChk });
        }
    }

    if (candidates.empty()) return false;

    // Heuristic: prefer giving check, then higher capture value; otherwise random
    std::stable_sort(candidates.begin(), candidates.end(), [](const Candidate& a, const Candidate& b) {
        if (a.givesCheck != b.givesCheck) return a.givesCheck && !b.givesCheck;
        if (a.captureValue != b.captureValue) return a.captureValue > b.captureValue;
        return false;
        });

    // Among top few, pick randomly to avoid being too deterministic
    size_t bucket = std::min<size_t>(5, candidates.size());
    std::uniform_int_distribution<size_t> dist(0, bucket - 1);
    Candidate pick = candidates[dist(rng)];

    return tryAndApplyAIMove(pick);
}

// -------------------- rendering --------------------

void ChessGame::render() {
    window.clear(sf::Color(235, 235, 235));

    switch (state) {
    case GameState::MAIN_MENU:
        renderMainMenu();
        break;
    case GameState::PLAYING_FRIEND:
    case GameState::PLAYING_AI:
        renderBoardAndSidebar();
        if (gameOver) renderGameOverOverlay();
        break;
    case GameState::GAME_OVER:
        renderBoardAndSidebar();
        renderGameOverOverlay();
        break;
    }

=======
bool ChessGame::makeAIMove(bool aiWhite) {
    vector<Candidate> all;
    for (int i = 0; i < (int)pieces.size(); ++i) {
        if (pieces[i]->isWhite() != aiWhite) continue;
        for (int r = 0; r < BOARD_SIZE; ++r) for (int c = 0; c < BOARD_SIZE; ++c) {
            if (!pieces[i]->isValidMove(r, c, pieces)) continue;
            if (pieces[i]->isSquareOccupiedBySameColor(r, c, pieces)) continue;
            if (wouldMoveLeaveKingInCheck(i, r, c, pieces, textures)) continue;
            bool isCap = false; int capVal = 0;
            for (const auto& p : pieces) if (p->getRow() == r && p->getCol() == c && p->isWhite() != aiWhite) { isCap = true; capVal = pieceValue(p->getName()); break; }
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
    vector<Candidate> checks;
    for (auto& c : all) if (c.givesCheck) checks.push_back(c);
    if (!checks.empty()) {
        uniform_int_distribution<int> d(0, (int)checks.size() - 1);
        Candidate sel = checks[d(rng)];
        return tryAndApplyAIMove(sel);
    }
    int bestVal = 0;
    for (auto& c : all) if (c.isCapture) bestVal = max(bestVal, c.captureValue);
    if (bestVal > 0) {
        vector<Candidate> bestCaps;
        for (auto& c : all) if (c.isCapture && c.captureValue == bestVal) bestCaps.push_back(c);
        uniform_int_distribution<int> d(0, (int)bestCaps.size() - 1);
        Candidate sel = bestCaps[d(rng)];
        return tryAndApplyAIMove(sel);
    }
    uniform_int_distribution<int> d(0, (int)all.size() - 1);
    Candidate sel = all[d(rng)];
    return tryAndApplyAIMove(sel);
}

bool ChessGame::tryAndApplyAIMove(const Candidate& cand) {
    int idx = -1;
    if (cand.pieceIndex >= 0 && cand.pieceIndex < (int)pieces.size()) idx = cand.pieceIndex;
    if (idx == -1) {
        for (int i = 0; i < (int)pieces.size(); ++i) if (pieces[i]->getName() == pieces[cand.pieceIndex]->getName()) { idx = i; break; }
    }
    if (idx == -1) return false;
    return tryMove(idx, cand.tr, cand.tc);
}

vector<tuple<int, int, bool>> ChessGame::computeLegalMovesForPiece(int index) {
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

void ChessGame::updateCheckHighlights() {
    for (int r = 0; r < BOARD_SIZE; ++r) {
        for (int c = 0; c < BOARD_SIZE; ++c) {
            bool light = (r + c) % 2 == 0;
            squares[r][c].setFillColor(light ? baseLightColor : baseDarkColor);
        }
    }
    auto wk = findKingPos(true, pieces);
    if (wk.first != -1) {
        if (isSquareAttacked(wk.first, wk.second, false, pieces)) {
            squares[wk.first][wk.second].setFillColor(sf::Color(220, 20, 60));
        }
    }
    auto bk = findKingPos(false, pieces);
    if (bk.first != -1) {
        if (isSquareAttacked(bk.first, bk.second, true, pieces)) {
            squares[bk.first][bk.second].setFillColor(sf::Color(220, 20, 60));
        }
    }
}

void ChessGame::drawKingCheckOutlines() {
    auto wk = findKingPos(true, pieces);
    if (wk.first != -1 && isSquareAttacked(wk.first, wk.second, false, pieces)) {
        sf::RectangleShape rect(sf::Vector2f((float)TILE_SIZE - 8.f, (float)TILE_SIZE - 8.f));
        rect.setPosition((float)wk.second * TILE_SIZE + 4.f, (float)wk.first * TILE_SIZE + 4.f);
        rect.setFillColor(sf::Color(0, 0, 0, 0));
        rect.setOutlineColor(sf::Color(220, 20, 60));
        rect.setOutlineThickness(4.f);
        window.draw(rect);
    }
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

void ChessGame::run() {
    while (window.isOpen()) {
        processEvents();

        if (state == GameState::PLAYING_AI && !gameOver) {
            if (!whiteTurn) {
                if (!aiThinking) {
                    aiThinking = true; aiClock.restart();
                }
                else {
                    if (aiClock.getElapsedTime().asSeconds() >= AI_THINK_SECONDS) {
                        bool moved = makeAIMove(false);
                        aiThinking = false;
                        if (moved) {
                            whiteTurn = !whiteTurn;
                            if (isKingInCheck(whiteTurn, pieces)) {}
                            if (!hasLegalMove(whiteTurn, pieces, textures)) {
                                if (isKingInCheck(whiteTurn, pieces)) endGameMessage = string((whiteTurn ? "White" : "Black")) + " - Checkmate";
                                else endGameMessage = "Draw - Stalemate";
                                gameOver = true; state = GameState::GAME_OVER;
                            }
                        }
                        else {
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

void ChessGame::processEvents() {
    sf::Event e;
    while (window.pollEvent(e)) {
        if (e.type == sf::Event::Closed) window.close();
        if (state == GameState::MAIN_MENU) handleMenuEvent(e);
        else if (state == GameState::PLAYING_FRIEND || state == GameState::PLAYING_AI) handleGameEvent(e);
        else if (state == GameState::GAME_OVER) handleGameOverEvent(e);
    }
}

void ChessGame::handleMenuEvent(const sf::Event& e) {
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

void ChessGame::handleGameOverEvent(const sf::Event& e) {
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

void ChessGame::handleGameEvent(const sf::Event& e) {
    if (e.type == sf::Event::MouseButtonPressed && e.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mp = window.mapPixelToCoords({ e.mouseButton.x,e.mouseButton.y });
        float sx = TILE_SIZE * BOARD_SIZE;
        if (mp.x >= sx) {
            if (backToMenuBtn.isClicked(mp)) { state = GameState::MAIN_MENU; return; }
        }
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
            if (isKingInCheck(whiteTurn, pieces)) {}
            if (!hasLegalMove(whiteTurn, pieces, textures)) {
                if (isKingInCheck(whiteTurn, pieces)) endGameMessage = string((whiteTurn ? "White" : "Black")) + " - Checkmate";
                else endGameMessage = "Draw - Stalemate";
                gameOver = true; state = GameState::GAME_OVER;
            }
        }
        else {
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

void ChessGame::render() {
    window.clear(sf::Color(40, 40, 40));
    if (state == GameState::MAIN_MENU) renderMainMenu();
    else {
        renderBoardAndSidebar();
        if (dragging && draggedIndex != -1) drawHighlightsForPiece(draggedIndex);
        drawKingCheckOutlines();
        if (state == GameState::GAME_OVER) renderGameOverOverlay();
    }
>>>>>>> 11ed285900d2ae6925459c826014a259e6eb0b8a
    window.display();
}

void ChessGame::renderMainMenu() {
<<<<<<< HEAD
    // Simple centered layout
    sf::Text title = titleText;
    title.setPosition(window.getSize().x / 2.f - title.getLocalBounds().width / 2.f, 110.f);

    sf::Text subtitle = subText;
    subtitle.setPosition(window.getSize().x / 2.f - subtitle.getLocalBounds().width / 2.f, 200.f);

    window.draw(title);
    window.draw(subtitle);
=======
    sf::RectangleShape bg(sf::Vector2f((float)window.getSize().x, (float)window.getSize().y));
    bg.setFillColor(sf::Color(230, 240, 230));
    window.draw(bg);

    sf::FloatRect tb = titleText.getLocalBounds();
    float centerX = window.getSize().x / 2.f;
    float centerY = window.getSize().y / 2.f;

    titleText.setPosition(centerX - tb.width / 2.f, centerY - 200.f);
    titleText.setFillColor(sf::Color(20, 20, 20));
    window.draw(titleText);

    sf::FloatRect sb = subText.getLocalBounds();
    subText.setPosition(centerX - sb.width / 2.f, titleText.getPosition().y + tb.height + 18.f);
    subText.setFillColor(sf::Color(60, 60, 60));
    window.draw(subText);
>>>>>>> 11ed285900d2ae6925459c826014a259e6eb0b8a

    playFriendBtn.draw(window);
    playAIBtn.draw(window);
    exitBtn.draw(window);
<<<<<<< HEAD
}

void ChessGame::renderBoardAndSidebar() {
    // board
    for (int r = 0; r < Chess::BOARD_SIZE; ++r) {
        for (int c = 0; c < Chess::BOARD_SIZE; ++c) {
            window.draw(squares[r][c]);
        }
    }

    // show king-in-check outlines if any
    drawKingCheckOutlines();

    // draw pieces (dragged one last so it stays on top)
    int drawDraggedLater = dragging ? draggedIndex : -1;
    for (int i = 0; i < static_cast<int>(pieces.size()); ++i) {
        if (i == drawDraggedLater) continue;
        auto& p = pieces[i];
        auto spr = makePieceSprite(textures, p->getName(), p->getRow(), p->getCol());
        window.draw(spr);
    }

    // draw legal move highlights for dragged piece
    if (dragging && draggedIndex >= 0) {
        drawHighlightsForPiece(draggedIndex);
    }

    // draw the dragged piece following cursor
    if (dragging && draggedIndex >= 0) {
        auto& p = pieces[draggedIndex];
        sf::Vector2i mousePix = sf::Mouse::getPosition(window);
        sf::Vector2f world = window.mapPixelToCoords(mousePix);
        sf::Sprite s = makePieceSprite(textures, p->getName(), 0, 0,
            world.x - Chess::TILE_SIZE / 2.f, world.y - Chess::TILE_SIZE / 2.f);
        window.draw(s);
    }

    // sidebar
    drawSidebar();

    // footer
    sf::Text foot = footerText;
    foot.setPosition(8.f, Chess::TILE_SIZE * Chess::BOARD_SIZE + 4.f);
    window.draw(foot);
}

void ChessGame::drawSidebar() {
    const float sx = Chess::TILE_SIZE * Chess::BOARD_SIZE;
    const float w = static_cast<float>(Chess::SIDEBAR_WIDTH);
    const float h = static_cast<float>(Chess::TILE_SIZE * Chess::BOARD_SIZE);

    sf::RectangleShape panel({ w, h });
    panel.setPosition(sx, 0.f);
    panel.setFillColor(sf::Color(245, 245, 245));
    panel.setOutlineThickness(1.f);
    panel.setOutlineColor(sf::Color(200, 200, 200));
    window.draw(panel);

    sf::Text turnText;
    turnText.setFont(font);
    turnText.setCharacterSize(22);
    turnText.setFillColor(sf::Color::Black);
    turnText.setString(gameOver ? "Game Over" : (whiteTurn ? "Turn: White" : "Turn: Black"));
    turnText.setPosition(sx + 14.f, 12.f);
    window.draw(turnText);

    // captured rows
    float y = 48.f;
    sf::Text capTitle;
    capTitle.setFont(font);
    capTitle.setCharacterSize(18);
    capTitle.setFillColor(sf::Color(40, 40, 40));

    capTitle.setString("Captured by White:");
    capTitle.setPosition(sx + 14.f, y);
    window.draw(capTitle);
    y += 26.f;
    drawCapturedRow(sx + 14.f, y, capturedByWhite);

    y += 12.f;
    capTitle.setString("Captured by Black:");
    capTitle.setPosition(sx + 14.f, y);
    window.draw(capTitle);
    y += 26.f;
    drawCapturedRow(sx + 14.f, y, capturedByBlack);

    // Controls
        sf::Text ctrl("\n\nControls:\nDrag pieces\nU:Undo  S:Save  L:Load\nR:Restart  Esc:Menu", font, 14);
       ctrl.setFillColor(sf::Color::Black);
       ctrl.setPosition(sx + 14.f, y);
       window.draw(ctrl);

    // back-to-menu button in sidebar when in game
    backToMenuBtn.draw(window);
}

void ChessGame::drawCapturedRow(float startX, float& y, const std::vector<std::string>& captured, float iconSize, float padding) {
    float x = startX;
    for (const auto& name : captured) {
        sf::Sprite s;
        auto it = textures.find(name);
        if (it != textures.end()) s.setTexture(it->second);
        // scale to iconSize if texture not matching
        sf::Vector2u texSize = it != textures.end() ? it->second.getSize() : sf::Vector2u(1, 1);
        float scale = iconSize / static_cast<float>(Chess::TILE_SIZE);
        s.setScale(scale, scale);
        s.setPosition(x, y);
        window.draw(s);
        x += iconSize + padding;
        if (x > (Chess::TILE_SIZE * Chess::BOARD_SIZE + Chess::SIDEBAR_WIDTH - iconSize - 10)) {
            x = startX; y += iconSize + padding;
        }
    }
    // advance y past last line
    if (!captured.empty()) y += iconSize + padding;
}

void ChessGame::drawHighlightsForPiece(int pieceIndex) {
    auto legal = computeLegalMovesForPiece(pieceIndex);

    sf::CircleShape dot(10.f);
    dot.setFillColor(sf::Color(0, 0, 0, 70));

    sf::RectangleShape capRect(sf::Vector2f(Chess::TILE_SIZE - 6.f, Chess::TILE_SIZE - 6.f));
    capRect.setFillColor(sf::Color::Transparent);
    capRect.setOutlineThickness(3.f);
    capRect.setOutlineColor(sf::Color(180, 0, 0, 150));

    for (auto& mv : legal) {
        int r, c; bool isCap;
        std::tie(r, c, isCap) = mv;

        if (isCap) {
            capRect.setPosition(c * Chess::TILE_SIZE + 3.f, r * Chess::TILE_SIZE + 3.f);
            window.draw(capRect);
        }
        else {
            dot.setPosition(c * Chess::TILE_SIZE + Chess::TILE_SIZE / 2.f - 10.f,
                r * Chess::TILE_SIZE + Chess::TILE_SIZE / 2.f - 10.f);
            window.draw(dot);
=======

    sf::FloatRect fb = footerText.getLocalBounds();
    footerText.setPosition(centerX - fb.width / 2.f, (float)window.getSize().y - 30.f);
    footerText.setFillColor(sf::Color(80, 80, 80));
    window.draw(footerText);
}

void ChessGame::renderBoardAndSidebar() {
    updateCheckHighlights();

    for (int r = 0; r < BOARD_SIZE; ++r) for (int c = 0; c < BOARD_SIZE; ++c) window.draw(squares[r][c]);
    for (int i = 0; i < (int)pieces.size(); ++i) if (i != draggedIndex) window.draw(pieces[i]->getSprite());
    if (draggedIndex != -1 && draggedIndex < (int)pieces.size()) window.draw(pieces[draggedIndex]->getSprite());
    drawSidebar();
}

void ChessGame::drawCapturedRow(float startX, float& y, const vector<string>& captured, float iconSize, float padding) {
    float x = startX;
    float maxW = (float)SIDEBAR_WIDTH - 24.f;
    float step = iconSize + padding;
    for (size_t i = 0; i < captured.size(); ++i) {
        auto it = textures.find(captured[i]);
        if (it == textures.end()) continue;
        sf::Sprite s(it->second);
        sf::FloatRect lb = s.getLocalBounds();
        if (lb.width > 0.f && lb.height > 0.f) {
            float scale = min(iconSize / lb.width, iconSize / lb.height);
            s.setScale(scale, scale);
        }
        s.setPosition(startX + (float)((int)(x - startX) % (int)maxW), y);
        window.draw(s);

        x += step;
        if (x - startX + iconSize > maxW) {
            x = startX;
            y += iconSize + padding;
        }
    }
    y += iconSize + padding * 0.5f;
}

void ChessGame::drawSidebar() {
    float sx = TILE_SIZE * BOARD_SIZE;
    sf::RectangleShape sidebar(sf::Vector2f((float)SIDEBAR_WIDTH, (float)TILE_SIZE * BOARD_SIZE));
    sidebar.setPosition(sx, 0);
    sidebar.setFillColor(sf::Color(40, 40, 40));
    window.draw(sidebar);

    float y = 12.f;

    sf::Text tturn("Turn: " + string(whiteTurn ? "White" : "Black"), font, 20);
    tturn.setFillColor(sf::Color::White);
    tturn.setPosition(sx + 12.f, y);
    window.draw(tturn);
    y += 28.f;

    sf::Text tmode("Mode: " + string(playWithAI ? "Play vs AI" : "2 Players"), font, 16);
    tmode.setFillColor(sf::Color(200, 200, 200));
    tmode.setPosition(sx + 12.f, y);
    window.draw(tmode);
    y += 34.f;

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

    sf::Text ctrl("Controls:\nDrag pieces\nU:Undo  S:Save  L:Load\nR:Restart  Esc:Menu", font, 14);
    ctrl.setFillColor(sf::Color::White);
    ctrl.setPosition(sx + 12.f, max(y + 8.f, (float)TILE_SIZE * BOARD_SIZE - 180.f));
    window.draw(ctrl);

    backToMenuBtn.draw(window);
}

void ChessGame::drawHighlightsForPiece(int pieceIndex) {
    auto moves = computeLegalMovesForPiece(pieceIndex);
    for (auto& t : moves) {
        int r = get<0>(t), c = get<1>(t); bool isCap = get<2>(t);
        sf::Vector2f center((c + 0.5f) * TILE_SIZE, (r + 0.5f) * TILE_SIZE);
        if (!isCap) {
            sf::CircleShape circle(TILE_SIZE * 0.12f);
            circle.setOrigin(circle.getRadius(), circle.getRadius());
            circle.setPosition(center);
            circle.setFillColor(sf::Color(30, 180, 30, 180));
            window.draw(circle);
        }
        else {
            sf::RectangleShape rect(sf::Vector2f((float)TILE_SIZE - 8.f, (float)TILE_SIZE - 8.f));
            rect.setPosition((float)c * TILE_SIZE + 4.f, (float)r * TILE_SIZE + 4.f);
            rect.setFillColor(sf::Color(0, 0, 0, 0));
            rect.setOutlineColor(sf::Color(220, 20, 60)); rect.setOutlineThickness(4.f);
            window.draw(rect);
>>>>>>> 11ed285900d2ae6925459c826014a259e6eb0b8a
        }
    }
}

void ChessGame::renderGameOverOverlay() {
<<<<<<< HEAD
    // dimmer
    sf::RectangleShape overlay({ static_cast<float>(Chess::TILE_SIZE * Chess::BOARD_SIZE),
                                 static_cast<float>(Chess::TILE_SIZE * Chess::BOARD_SIZE) });
    overlay.setFillColor(sf::Color(0, 0, 0, 110));
    overlay.setPosition(0.f, 0.f);
    window.draw(overlay);

    // message
    sf::Text msg;
    msg.setFont(font);
    msg.setString(endGameMessage.empty() ? "Game Over" : endGameMessage);
    msg.setCharacterSize(40);
    msg.setFillColor(sf::Color::White);
    auto bounds = msg.getLocalBounds();
    msg.setPosition(Chess::TILE_SIZE * Chess::BOARD_SIZE / 2.f - bounds.width / 2.f,
        Chess::TILE_SIZE * Chess::BOARD_SIZE / 2.f - bounds.height);
    window.draw(msg);
}

void ChessGame::updateCheckHighlights() {
    // no state caching; drawing handled in drawKingCheckOutlines()
}

void ChessGame::drawKingCheckOutlines() {
    // Outline any king currently in check
    for (int side = 0; side < 2; ++side) {
        bool white = (side == 0);
        if (!isKingInCheck(white, pieces)) continue;
        int kIdx = findKingIndex(white, pieces);
        if (kIdx < 0) continue;
        int r = pieces[kIdx]->getRow();
        int c = pieces[kIdx]->getCol();

        sf::RectangleShape outline(sf::Vector2f(Chess::TILE_SIZE - 4.f, Chess::TILE_SIZE - 4.f));
        outline.setPosition(c * Chess::TILE_SIZE + 2.f, r * Chess::TILE_SIZE + 2.f);
        outline.setFillColor(sf::Color::Transparent);
        outline.setOutlineThickness(4.f);
        outline.setOutlineColor(sf::Color(220, 40, 40, 220));
        window.draw(outline);
    }
}

// -------------------- events --------------------

void ChessGame::processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }

        switch (state) {
        case GameState::MAIN_MENU:
            handleMenuEvent(event);
            break;
        case GameState::PLAYING_FRIEND:
        case GameState::PLAYING_AI:
            handleGameEvent(event);
            break;
        case GameState::GAME_OVER:
            handleGameOverEvent(event);
            break;
        }
    }
}

void ChessGame::handleMenuEvent(const sf::Event& e) {
    if (e.type == sf::Event::MouseButtonPressed && e.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mousePos = window.mapPixelToCoords({ e.mouseButton.x, e.mouseButton.y });

        if (playFriendBtn.isClicked(mousePos)) {
            startNewGame(false);
        }
        else if (playAIBtn.isClicked(mousePos)) {
            startNewGame(true);
        }
        else if (exitBtn.isClicked(mousePos)) {
            window.close();
        }
    }
}

void ChessGame::handleGameOverEvent(const sf::Event& e) {
    if (e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::Escape) {
        state = GameState::MAIN_MENU;
        return;
    }
    if (e.type == sf::Event::MouseButtonPressed && e.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mousePos = window.mapPixelToCoords({ e.mouseButton.x, e.mouseButton.y });
        if (backToMenuBtn.isClicked(mousePos)) {
            state = GameState::MAIN_MENU;
        }
    }
}

void ChessGame::handleGameEvent(const sf::Event& e) {
    // global hotkeys
    if (e.type == sf::Event::KeyPressed) {
        if (e.key.code == sf::Keyboard::Escape) {
            state = GameState::MAIN_MENU;
            return;
        }
        if (e.key.code == sf::Keyboard::U) {
            undo();
            return;
        }
        if (e.key.code == sf::Keyboard::S) {
            saveToFile("savegame.txt");
            return;
        }
        if (e.key.code == sf::Keyboard::L) {
            loadFromFile("savegame.txt");
            return;
        }
        if (e.key.code == sf::Keyboard::R) {
            startNewGame(playWithAI);
            return;
        }
    }

    // sidebar button
    if (e.type == sf::Event::MouseButtonPressed && e.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mousePos = window.mapPixelToCoords({ e.mouseButton.x, e.mouseButton.y });
        if (backToMenuBtn.isClicked(mousePos)) {
            state = GameState::MAIN_MENU;
            return;
        }
    }

    // if AI turn in vs AI, block user interaction
    if (state == GameState::PLAYING_AI && !whiteTurn) return;
    if (gameOver) return;

    // dragging
    if (e.type == sf::Event::MouseButtonPressed && e.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2i boardPos = pixelToBoard(window, { e.mouseButton.x, e.mouseButton.y });
        int r = boardPos.x, c = boardPos.y;
        if (!onBoard(r, c)) return;

        int idx = findPieceIndexAt(r, c, pieces);
        if (idx != -1) {
            // must be side to move
            if (pieces[idx]->isWhite() == whiteTurn) {
                dragging = true;
                draggedIndex = idx;
                sf::Vector2f mouseWorld = window.mapPixelToCoords({ e.mouseButton.x, e.mouseButton.y });
                dragOffset = { mouseWorld.x - c * Chess::TILE_SIZE, mouseWorld.y - r * Chess::TILE_SIZE };
            }
        }
    }
    else if (e.type == sf::Event::MouseButtonReleased && e.mouseButton.button == sf::Mouse::Left) {
        if (!dragging || draggedIndex < 0) return;
        sf::Vector2i boardPos = pixelToBoard(window, { e.mouseButton.x, e.mouseButton.y });
        int r = boardPos.x, c = boardPos.y;
        dragging = false;

        if (!onBoard(r, c)) { draggedIndex = -1; return; }

        bool moved = tryMove(draggedIndex, r, c);
        draggedIndex = -1;

        if (moved) {
            whiteTurn = !whiteTurn;

            // check end conditions
            if (isKingInCheck(whiteTurn, pieces)) {
                if (!hasLegalMove(whiteTurn, pieces, textures)) {
                    endGameMessage = whiteTurn ? "Black wins - Checkmate" : "White wins - Checkmate";
                    gameOver = true;
                    state = GameState::GAME_OVER;
                }
            }
            else if (!hasLegalMove(whiteTurn, pieces, textures)) {
                endGameMessage = "Draw - Stalemate";
                gameOver = true;
                state = GameState::GAME_OVER;
            }
        }
    }
}

// -------------------- helper --------------------

void ChessGame::recordCapture(bool capturerIsWhite, const std::string& capturedName) {
    if (capturerIsWhite) capturedByWhite.push_back(capturedName);
    else capturedByBlack.push_back(capturedName);
}
=======
    sf::RectangleShape overlay(sf::Vector2f((float)TILE_SIZE * BOARD_SIZE, (float)TILE_SIZE * BOARD_SIZE));
    overlay.setFillColor(sf::Color(0, 0, 0, 150));
    window.draw(overlay);

    string msg = endGameMessage.empty() ? "Game Over" : endGameMessage;
    sf::Text t(msg, font, 48); t.setFillColor(sf::Color::White);
    sf::FloatRect tb = t.getLocalBounds();
    t.setPosition((TILE_SIZE * BOARD_SIZE - tb.width) / 2.f, (TILE_SIZE * BOARD_SIZE - tb.height) / 2.f - 40.f);
    window.draw(t);

    string reason;
    if (msg.find("Checkmate") != string::npos) reason = "Checkmate — King is attacked and has no legal moves.";
    else if (msg.find("Stalemate") != string::npos) reason = "Stalemate — side has no legal moves but is not in check.";
    else reason = "Game finished.";
    sf::Text rtext(reason, font, 18); rtext.setFillColor(sf::Color::White);
    sf::FloatRect rb = rtext.getLocalBounds();
    rtext.setPosition((TILE_SIZE * BOARD_SIZE - rb.width) / 2.f, (TILE_SIZE * BOARD_SIZE - rb.height) / 2.f + 20.f);
    window.draw(rtext);
}

void ChessGame::recordCapture(bool capturerIsWhite, const string& capturedName) {
    if (capturerIsWhite) capturedByWhite.push_back(capturedName);
    else capturedByBlack.push_back(capturedName);
}
>>>>>>> 11ed285900d2ae6925459c826014a259e6eb0b8a
