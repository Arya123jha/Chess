#include "Piece.h"
#include "Constants.h"
#include <stdexcept>
#include <algorithm>

Piece::Piece(const string& name_, int r, int c, const sf::Texture& t) : name(name_), row(r), col(c) {
    sprite.setTexture(t);
    sprite.setPosition(static_cast<float>(col) * TILE_SIZE, static_cast<float>(row) * TILE_SIZE);
}

void Piece::setPosition(int r, int c) {
    row = r; col = c;
    sprite.setPosition(static_cast<float>(col) * TILE_SIZE, static_cast<float>(row) * TILE_SIZE);
    hasMoved = true;
}

bool Piece::isSquareOccupiedBySameColor(int r, int c, const vector<unique_ptr<Piece>>& pieces) const {
    for (const unique_ptr<Piece>& p : pieces)
        if (p->getRow() == r && p->getCol() == c && p->isWhite() == isWhite()) 
            return true;
        return false;
}

bool Piece::isPathClear(int newRow, int newCol, const vector<unique_ptr<Piece>>& pieces) const {
    int rowStep = (newRow > row) ? 1 : (newRow < row) ? -1 : 0;
    int colStep = (newCol > col) ? 1 : (newCol < col) ? -1 : 0;
    int r = row + rowStep, c = col + colStep;
    while (r != newRow || c != newCol) {
        for (const unique_ptr<Piece>& p : pieces) if (p->getRow() == r && p->getCol() == c) return false;
        r += rowStep; c += colStep;
    }
    return true;
}

// Pawn implementation
Pawn::Pawn(const string& n, int r, int c, const sf::Texture& t) : Piece(n, r, c, t) {}
bool Pawn::isValidMove(int newRow, int newCol, const vector<unique_ptr<Piece>>& pieces) const {
    int dir = isWhite() ? -1 : 1;
    if (col == newCol) {
        if (newRow == row + dir) {
            for (const unique_ptr<Piece>& p : pieces) 
                if (p->getRow() == newRow && p->getCol() == newCol) 
                    return false;
            return true;
        }
        if (newRow == row + 2 * dir && !hasMoved) {
            for (const unique_ptr<Piece>& p : pieces) {
                if (p->getCol() == col && (p->getRow() == row + dir || p->getRow() == row + 2 * dir)) return false;
            }
            return true;
        }
    }
    else if (abs(newCol - col) == 1 && newRow == row + dir) {
        for (const unique_ptr<Piece>& p : pieces) if (p->getRow() == newRow && p->getCol() == newCol && p->isWhite() != isWhite()) return true;
    }
    return false;
}

// Rook implementation
Rook::Rook(const string& n, int r, int c, const sf::Texture& t) : Piece(n, r, c, t) {}
bool Rook::isValidMove(int newRow, int newCol, const vector<unique_ptr<Piece>>& pieces) const {
    if (newRow != row && newCol != col) return false;
    if (!isPathClear(newRow, newCol, pieces)) return false;
    return !isSquareOccupiedBySameColor(newRow, newCol, pieces);
}

// Knight implementation
Knight::Knight(const string& n, int r, int c, const sf::Texture& t) : Piece(n, r, c, t) {}
bool Knight::isValidMove(int newRow, int newCol, const vector<unique_ptr<Piece>>& pieces) const {
    int dr = abs(newRow - row), dc = abs(newCol - col);
    if (!((dr == 2 && dc == 1) || (dr == 1 && dc == 2))) return false;
    return !isSquareOccupiedBySameColor(newRow, newCol, pieces);
}

// Bishop implementation
Bishop::Bishop(const string& n, int r, int c, const sf::Texture& t) : Piece(n, r, c, t) {}
bool Bishop::isValidMove(int newRow, int newCol, const vector<unique_ptr<Piece>>& pieces) const {
    if (abs(newRow - row) != abs(newCol - col)) return false;
    if (!isPathClear(newRow, newCol, pieces)) return false;
    return !isSquareOccupiedBySameColor(newRow, newCol, pieces);
}

// Queen implementation
Queen::Queen(const string& n, int r, int c, const sf::Texture& t) : Piece(n, r, c, t) {}
bool Queen::isValidMove(int newRow, int newCol, const vector<unique_ptr<Piece>>& pieces) const {
    bool straight = (newRow == row || newCol == col);
    bool diag = (abs(newRow - row) == abs(newCol - col));
    if (!straight && !diag) return false;
    if (!isPathClear(newRow, newCol, pieces)) return false;
    return !isSquareOccupiedBySameColor(newRow, newCol, pieces);
}

// King implementation
King::King(const string& n, int r, int c, const sf::Texture& t) : Piece(n, r, c, t) {}
bool King::isValidMove(int newRow, int newCol, const vector<unique_ptr<Piece>>& pieces) const {
    int dr = abs(newRow - row), dc = abs(newCol - col);
    if (dr > 1 || dc > 1) return false;
    return !isSquareOccupiedBySameColor(newRow, newCol, pieces);
}

// Utility functions
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
    for (const unique_ptr<Piece>& p : pieces) {
        unique_ptr<Piece> np = createPieceFromName(p->getName(), p->getRow(), p->getCol(), textures);
        np->getHasMoved() = p->getHasMoved();
        copy.push_back(move(np));
    }
    return copy;
}

bool isSquareAttacked(int r, int c, bool byWhite, const vector<unique_ptr<Piece>>& pieces) {
    for (const unique_ptr<Piece>& p : pieces) {
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

pair<int, int> findKingPos(bool white, const vector<unique_ptr<Piece>>& pieces) {
    for (const unique_ptr<Piece>& p : pieces) if (p->isWhite() == white && p->getName().find("king") != string::npos) return { p->getRow(), p->getCol() };
    return { -1,-1 };
}

bool isKingInCheck(bool white, const vector<unique_ptr<Piece>>& pieces) {
    auto kp = findKingPos(white, pieces);
    if (kp.first == -1) return false;
    return isSquareAttacked(kp.first, kp.second, !white, pieces);
}

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


int pieceValue(const string& name) {
    if (name.find("pawn") != string::npos) return 1;
    if (name.find("knight") != string::npos) return 3;
    if (name.find("bishop") != string::npos) return 3;
    if (name.find("rook") != string::npos) return 5;
    if (name.find("queen") != string::npos) return 9;
    if (name.find("king") != string::npos) return 100;
    return 0;
}