#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <string.h>
using namespace std;
#include "Types.h"


struct GameState
{
    Color turn;
    Square lastEnPassant; // empty square where the last double-pushed pawn can be captured
    bool whiteCanCastleKingSide;
    bool whiteCanCastleQueenSide;
    bool blackCanCastleQueenSide;
    bool blackCanCastleKingSide;
    int halfMoveClock; //pliesSinceLastCaptureOrPawnMove
    int fullMoveNumber; // incremented after each black ply
    Piece playedPiece;
    Piece capturedPiece;
    Key zobrist;
};


#endif
