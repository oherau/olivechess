#ifndef MOVEGEN_H
#define MOVEGEN_H

#include <map>
#include "types.h"
#include "position.h"
#include "notation.h"

using namespace std;

enum GenType
{
    CAPTURES,
    QUIETS,
    QUIET_CHECKS,
    EVASIONS,
    NON_EVASIONS,
    LEGAL
};

class Position;

// old implementation (generate, then prune illegal moves)

//ExtMove* GetLegalMoves(Position& pos, ExtMove* moves);

// new implementation (generate good moves only)
//extern int GenLegalMoves(Position& pos, ExtMove* moves);
Value potentialValue(Move move);
//void SetMvvLvaValue(ExtMove* list, Position& p);
//Value MvvLvaValue(Position& pos, Move move);

void SortByPotential(ExtMove* moves);
//void SetPotentialValue(Position& pos, ExtMove* t);
//int potentialValue(Position& pos, Move move);
//void SetHistoryValue(ExtMove* list, Position& p);

//Piece GetCPiece(Position& pos, Move move);
//Piece GetMPiece(Position& pos, Move move);
int GetVal(Piece p);

// Pruning and move ordering
//void futility_pruning(Position& pos, ExtMove* t);
void SetPotentialValue(ExtMove* list);


template<GenType>
ExtMove* generate(const Position& pos, ExtMove* mlist);

/// The MoveList struct is a simple wrapper around generate(). It sometimes comes
/// in handy to use this class instead of the low level generate() function.
template<GenType T>
struct MoveList
{

    explicit MoveList(const Position& pos) : cur(mlist), last(generate<T>(pos, mlist))
    {
        last->move = MOVE_NONE;
    }
    void operator++()
    {
        ++cur;
    }
    Move operator*() const
    {
        return cur->move;
    }
    size_t size() const
    {
        return last - mlist;
    }
    bool contains(Move m) const
    {
        for (const ExtMove* it(mlist); it != last; ++it) if (it->move == m) return true;
        return false;
    }

private:
    ExtMove mlist[MAX_MOVES];
    ExtMove *cur, *last;
};


#endif
