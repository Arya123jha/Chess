<<<<<<< HEAD
#pragma once
=======
#ifndef PIECE_H
#define PIECE_H

>>>>>>> 11ed285900d2ae6925459c826014a259e6eb0b8a
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <string>
<<<<<<< HEAD
#include "Constants.h"

using namespace std;
using namespace Chess;

=======
#include <map>
#include "constants.h"

using namespace std;
>>>>>>> 11ed285900d2ae6925459c826014a259e6eb0b8a

class Piece {
public:
    sf::Sprite sprite;
    int row = 0, col = 0;
    string name;
    bool hasMoved = false;

    Piece(const string& name_, int r, int c, const sf::Texture& t);
    virtual ~Piece() = default;

    virtual bool isValidMove(int newRow, int newCol, const vector<unique_ptr<Piece>>& pieces) const = 0;
    void setPosition(int r, int c);
    sf::Sprite& getSprite() { return sprite; }
    int getRow() const { return row; }
    int getCol() const { return col; }
    const string& getName() const { return name; }
    bool isWhite() const { return !name.empty() && name[0] == 'w'; }
    bool getHasMoved() const { return hasMoved; }

    bool isSquareOccupiedBySameColor(int r, int c, const vector<unique_ptr<Piece>>& pieces) const;
    bool isPathClear(int newRow, int newCol, const vector<unique_ptr<Piece>>& pieces) const;
};

class Pawn : public Piece {
public:
    Pawn(const string& n, int r, int c, const sf::Texture& t);
    bool isValidMove(int newRow, int newCol, const vector<unique_ptr<Piece>>& pieces) const override;
};

class Rook : public Piece {
public:
    Rook(const string& n, int r, int c, const sf::Texture& t);
    bool isValidMove(int newRow, int newCol, const vector<unique_ptr<Piece>>& pieces) const override;
};

class Knight : public Piece {
public:
    Knight(const string& n, int r, int c, const sf::Texture& t);
    bool isValidMove(int newRow, int newCol, const vector<unique_ptr<Piece>>& pieces) const override;
};

class Bishop : public Piece {
public:
    Bishop(const string& n, int r, int c, const sf::Texture& t);
    bool isValidMove(int newRow, int newCol, const vector<unique_ptr<Piece>>& pieces) const override;
};

class Queen : public Piece {
public:
    Queen(const string& n, int r, int c, const sf::Texture& t);
    bool isValidMove(int newRow, int newCol, const vector<unique_ptr<Piece>>& pieces) const override;
};

class King : public Piece {
public:
    King(const string& n, int r, int c, const sf::Texture& t);
    bool isValidMove(int newRow, int newCol, const vector<unique_ptr<Piece>>& pieces) const override;
};

unique_ptr<Piece> createPieceFromName(const string& name, int r, int c, const map<string, sf::Texture>& textures);
vector<unique_ptr<Piece>> clonePieces(const vector<unique_ptr<Piece>>& pieces, const map<string, sf::Texture>& textures);
bool isSquareAttacked(int r, int c, bool byWhite, const vector<unique_ptr<Piece>>& pieces);
pair<int, int> findKingPos(bool white, const vector<unique_ptr<Piece>>& pieces);
bool isKingInCheck(bool white, const vector<unique_ptr<Piece>>& pieces);
bool wouldMoveLeaveKingInCheck(int pieceIndex, int toRow, int toCol, const vector<unique_ptr<Piece>>& pieces, const map<string, sf::Texture>& textures);
bool hasLegalMove(bool white, const vector<unique_ptr<Piece>>& pieces, const map<string, sf::Texture>& textures);
string coordToAlgebraic(int row, int col);
char pieceToLetter(const string& name);
<<<<<<< HEAD
int pieceValue(const string& name);
=======
int pieceValue(const string& name);

#endif 
>>>>>>> 11ed285900d2ae6925459c826014a259e6eb0b8a
