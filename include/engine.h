#ifndef ENGINE_H
#define ENGINE_H

#include <string>
#include <map>
#include "watch.h"
#include "brainmode.h"
#include "brainstatus.h"
#include "position.h"
#include "types.h"
#include "transpose.h"
#include "movegen.h"
#include "eval.h"
#include "search.h"

#define MIN_ENGINE_ELO 700
#define MAX_ENGINE_ELO 1400

void engine_init();
void options_init();

// Engine actions
void stop_search();
void start_search();
void Ponderhit();
void update_think_time();

// Gets
//Color GetTurn();
string GetPRNG(); // get pseudo random generator hash key (should be Zobrist or BCH)
string GetEngineInfo(bool to_uci);
string GetEngineOptions();
Position& GetCurrentPosition();

// Sets
void SetSearchmovesMode(ExtMove* selectedMoves);
void SetPonderMode();
void SetPonderHit();
void set_wtime(int timeLeft);
void set_btime(int timeLeft);
void set_winc(int increment);
void set_binc(int increment);
void SetMovestogo(int movesUntilNextTimeControl);
void SetDepthMode(int pliesDepth);
void SetNodesMode(int nodesDepth);
void SetMateMode(int mateDepth);
void SetMovetimeMode(int searchTime);
//void SetPerftMode(int depth);
void SetInfiniteMode();
void SetVerboseMode(bool isVerboseMode);
void SetDepthLevel(int depth);
void SetTimeLevel(int timeLevel);
void set_start_position();
void set_position(string strPos);
void set_debug(bool isDebugOn);
void set_option(string name, string value);
void SetSmartTiming(bool value);

// tools
// TODO: move them into dedicated cpp/h files
void display_moves(ExtMove* moves);
void display_scores(ExtMove* moves);
void display_best_pv(unsigned int maxDepth, uint64_t nodeNb, uint64_t time, Value score, Move* pv);
void display_pv(Move* pv);
//void DisplayPv(ExtMove* pv);
//void DisplayPv(Move* pv, int maxDepth);
bool IsCheckmate(Position& position);
bool IsFiftyMovesDraw(Position& pos);
//void Sort(vector<MoveStack>& scores);
void sort(ExtMove* t);
string score_to_string(int score);
void clear_history();
void display_engine_monitor();
void display_engine_movelist();
void display_engine_cuttoff_accuracy(int maxDepth);
void display_engine_hashtable_usage();
void display_engine_depthstats(int* statTable,int maxDepth);


// exposed variables
extern BrainMode     brainMode;
extern BrainStatus   brainStatus;
extern Watch         watch;
extern ExtMove       bestMove;
extern Move          ponderMove;
extern unsigned long thinkDelay;
extern Position      currentPosition;
extern unsigned long nodeMaxNumber;
extern unsigned long nbProcessedNodes;
extern unsigned long nbCurrSearchTotalGenNodes;
extern unsigned long nbCurrSearchProcessedNodes;
extern int cutOffStatTable[20];
//extern unsigned long sumNodeCutOffIndex;
extern int           depthLevel;
extern ExtMove       moves[MAX_MOVES];
extern bool          debugMode;
extern bool          useTrans;
extern bool          isBlackbox;
// extern TranspositionTable    TTable;
extern int           selectiveDepth;
extern bool          UCI_AnalyseMode;
extern bool          UCI_Chess960;
extern bool          Ponder;
extern bool          isPondering;
extern int           minZeroWindowDepth;
//extern bool          UCI_LimitStrength;
//extern int           UCI_Elo;
//extern vector<MoveScorePv> scores; // TODO: merge with pvbuffer
extern Move* bestPv;
extern Move pvbuffer[MAX_MOVES][MAX_MOVES]; //pvbuffer[max move nb for depth 0][max depth];
extern map<Move,Move*> pvhash; // hash map to retrieve the pv line associated to a move
//extern long history[2][64][64]; // history heuristic
extern long history[16][64]; // history heuristic

// MultiCut parameters
extern int M;// M is the number of moves to look at when checking for mc-prune.
extern int C;// C is the number of cutoffs to cause an mc-prune, C < M.
extern int R;// R is the search depth reduction for mc-prune searches.

#endif
