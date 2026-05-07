#ifndef TRANSPOSITION_H
#define TRANSPOSITION_H

#include <map>
#include <string.h>
#include "position.h"
#include "types.h"
#include "notation.h"
using namespace std;

/*
Typically, the following information is stored as determined by the search [14] :
x-Zobrist- or BCH-key, to look whether the position is the right one while probing
o-Best- or Refutation move
o-Depth (draft)
o-Score, either with Integrated Bound and Value or otherwise with
o-Type of Node [15]
o-PV-Node, Score is Exact
o-All-Node, Score is Upper Bound
o-Cut-Node, Score is Lower Bound
o-Age is used to determine when to overwrite entries from searching previous positions during the game of chess*/
typedef struct
{
    int height;
    int bestvalue;
    int alpha;
    int beta;
} TTElem;


class TranspositionTable
{
public:
    TranspositionTable();
    ~TranspositionTable();

    void resize(int sizeInMB);
    unsigned int GetUsagePerMil();
    void clear();

    // from http://www.fierz.ch/strategy2.htm
    void store(const Position& p, /*int depth,*/ int height, Value bestvalue, Value alpha, Value beta);
    bool lookup(const Position& p, /*int depth,*/ int height, Value* alpha, Value* beta, Value* hashvalue);

    unsigned long GetHits();

    //void PushUpKillerMoves(ExtMove* moveList);

private:
    unsigned long maxSize; // maximum nb of elements
    map<Key, TTElem> table;
    // metrics
    unsigned long hits;
};


#endif
