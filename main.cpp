#include "ChessGame.h"
#include <iostream>

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