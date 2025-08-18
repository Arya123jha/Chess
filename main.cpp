<<<<<<< HEAD
#include "ChessGame.h"
#include <iostream>
=======
#include "chessgame.h"
#include <iostream> 
>>>>>>> 11ed285900d2ae6925459c826014a259e6eb0b8a

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