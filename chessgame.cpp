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
    loadTextures();
    setupBoardUI();
    setupMenuUI();
}

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
    }
}

void ChessGame::setupBoardUI() {
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

    saveState();
    state = vsAI ? GameState::PLAYING_AI : GameState::PLAYING_FRIEND;
}

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

    Piece* moving = pieces[pieceIndex].get();
    if (!moving->isValidMove(toR, toC, pieces)) return false;
    if (moving->isSquareOccupiedBySameColor(toR, toC, pieces)) return false;
    if (wouldMoveLeaveKingInCheck(pieceIndex, toR, toC, pieces, textures)) return false;

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
            }
        }
    }

    // Normal captures
    for (int i = 0; i < static_cast<int>(pieces.size()); ++i) {
        if (i != pieceIndex && pieces[i]->getRow() == toR && pieces[i]->getCol() == toC) {
            recordCapture(moving->isWhite(), pieces[i]->getName());
            pieces.erase(pieces.begin() + i);
            if (i < pieceIndex) --pieceIndex;
            break;
        }
    }

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
        }
    }

    saveState();
    return true;
}

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

    window.display();
}

void ChessGame::renderMainMenu() {
    // Simple centered layout
    sf::Text title = titleText;
    title.setPosition(window.getSize().x / 2.f - title.getLocalBounds().width / 2.f, 110.f);

    sf::Text subtitle = subText;
    subtitle.setPosition(window.getSize().x / 2.f - subtitle.getLocalBounds().width / 2.f, 200.f);

    window.draw(title);
    window.draw(subtitle);

    playFriendBtn.draw(window);
    playAIBtn.draw(window);
    exitBtn.draw(window);
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
        }
    }
}

void ChessGame::renderGameOverOverlay() {
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
