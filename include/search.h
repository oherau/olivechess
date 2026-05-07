#ifndef SEARCH_H
#define SEARCH_H

#include "position.h"
#include "notation.h"
#include "types.h"

void search(Position& position, int depth);
Value full_search(Position& pos, Value alpha, Value beta, int depth, int maxDepth, Move* pv);

void perft_search(Position& position, int depth);
unsigned long  recursive_perft_search(Position& pos, int depth);

//void SelectivityPruning(Position&pos, ExtMove* localmoves);

#endif
