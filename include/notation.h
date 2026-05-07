#ifndef NOTATION_H
#define NOTATION_H

#include <string>
#include "types.h"

class Position;

std::string score_to_uci(Value v, Value alpha = -VALUE_INFINITE, Value beta = VALUE_INFINITE);
Move move_from_uci(const Position& pos, std::string& str);
const std::string move_to_uci(Move m, bool chess960);
const std::string move_to_san(Position& pos, Move m);
std::string pretty_pv(Position& pos, int depth, Value score, int64_t msecs, Move pv[]);

#endif // #ifndef NOTATION_H_INCLUDED
