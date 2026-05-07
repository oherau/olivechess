#ifndef EVAL_H
#define EVAL_H

#include "position.h"

// global properties (declared in .cpp)
extern int queenWeight;
extern int rookWeight;
extern int bishopWeight;
extern int knightWeight;
extern int pawnWeight;

extern int kingMobilityFactor;
extern int queenMobilityFactor;
extern int rookMobilityFactor;
extern int bishopMobilityFactor;
extern int knightMobilityFactor;
extern int pawnMobilityFactor;

extern int pawnStructureFactor;

// functions
Value evaluate_board(Position& b);
Value evaluate_board(Position& b, Color turn);
Value evaluate_board(Position& b, Color turn, int depth);
Value evaluate_static_score(Position& p);
//Value EvaluateMobilityScore(Position& p);
//Value EvaluatePawnHandicapScore(Position& p);
//Value EvaluatePsqtScore(Position& p);
Value evaluate_attack_score(Position& p);



class Position;

namespace Eval {

extern void init();
extern Value evaluate(const Position& pos);
extern std::string trace(const Position& pos);

}



#endif
