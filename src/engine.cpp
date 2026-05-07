#include <stdio.h>
#include <assert.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <vector>
#include <list>
#include <math.h>
#include <cstring>
#include "logger.h"
#include "engine.h"
#include "version.h"
#include "notation.h"
#include "types.h"
#include "platform.h"
#include "thread.h"
#include "transpose.h"
#include "movegen.h"
#include "eval.h"
//#include "zobrist.h"
#include "search.h"


using namespace std;

// properties
// TODO: turn them into privates
bool debugMode;
bool useTrans;
int minZeroWindowDepth;
Position currentPosition;
Watch watch;
unsigned long nodeMaxNumber;
unsigned long nbProcessedNodes;// total nb of processed node in a search
unsigned long nbCurrSearchTotalGenNodes;
unsigned long nbCurrSearchProcessedNodes;
int cutOffStatTable[20];
unsigned long sumNodeCutOffIndex; // sum of indexes where the cuttoff occurs in each node
int selectiveDepth;
BrainMode brainMode;
BrainStatus brainStatus;
int depthLevel;
unsigned long thinkDelay;
long movestogo;
//bool UCI_LimitStrength;
//int UCI_Elo;
bool UCI_AnalyseMode;
bool UCI_Chess960;
bool Ponder;
bool isPondering;
// MultiCut parameters
int M;// M is the number of moves to look at when checking for mc-prune.
int C;// C is the number of cutoffs to cause an mc-prune, C < M.
int R;// R is the search depth reduction for mc-prune searches.
//TODO: replace by time[Color][] and inc[Color][]
long wtime;
long btime;
long winc;
long binc;
bool isVerbose;
bool isSmartTiming;
bool isBlackbox; // no-info mode to play at 100% full strength
bool isSearchMove;
// TODO: revert use of vectors to old style structs
ExtMove moves[MAX_MOVES];
Move* bestPv;
ExtMove bestMove;
Move ponderMove;
//vector<MoveScorePv> scores; // TODO: merge with pvbuffer
Move pvbuffer[MAX_MOVES][MAX_MOVES]; //pvbuffer[max move nb for depth 0][max depth];
map<Move,Move*> pvhash;
//map<Move,ExtMove[MAX_MOVES]> pvhash;
TranspositionTable TTable;


//long history[2][64][64]; // history heuristic
long history[16][64]; // history heuristic

void clear_history()
{
    memset(history,  0, 16 * 64 * sizeof(Value));
}

void engine_init()
{
// MULTICUT PARAMETERS
    M = 6;// M is the number of moves to look at when checking for mc-prune.
    C = 3;// C is the number of cutoffs to cause an mc-prune, C < M.
    R = 2;// R is the search depth reduction for mc-prune searches.

    useTrans = true; // deactivated if zero-window search
    minZeroWindowDepth = 2;

    isSearchMove = false;
    brainStatus = PAUSED;
    //do_sleep = true;

    // TODO: move this at SetStartPosition()
    // init heuristic history
    clear_history();

    // TODO debug init = Blitz 2'+6"/ply - to be removed
    set_wtime(120000);
    set_winc(6000);
    set_btime(120000);
    set_binc(6000);
    set_start_position();

    options_init();
}

void options_init()
{
    set_option("Smart Timer","true");
    set_option("Hash","16");
    set_option("Ponder", "true" );
    set_option("UCI_AnalyseMode","false");
    set_option("UCI_Chess960","false");
    //SetOption("UCI_LimitStrength","false");
    //SetOption("UCI_Elo", "1200" );

    set_option("Queen weight", "900");
    set_option("Rook weight", "500");
    set_option("Bishop weight", "325");
    set_option("Knight weight", "325");
    set_option("Pawn weight", "85");

    set_option("King mobility", "100");
    set_option("Queen mobility", "100");
    set_option("Rook mobility", "100");
    set_option("Bishop mobility", "100");
    set_option("Knight mobility", "100");
    set_option("Pawn mobility", "100");

    set_option("Pawn structure", "5");

    set_option("Blackbox", "false");
    
    set_debug(false);
}


void display_scores(ExtMove* moveList)
{
    std::cout <<  "NEW MOVES EVALUATION -------------------- " << std::endl;
    int mvCount = 0;
    for(ExtMove* mv=moveList; mv->move!= MOVE_NONE; mv++)
    {
        Move* pv = pvhash[mv->move];
        std::cout <<  move_to_uci(mv->move, currentPosition.is_chess960()) << "=";
        if( mv->value < -VALUE_KNOWN_WIN || mv->value > VALUE_KNOWN_WIN )
            std::cout <<  score_to_string(mv->value) << ' ';
        else
            std::cout <<  mv->value << ' ';

        std::cout <<  " pv: ";
        display_pv(pv);
        std::cout <<  std::endl;
        mvCount++;
    }
    std::cout <<  std::endl ;
    std::cout <<  "-------------------------------------------------" << std::endl;
}

/// Optimized bubble sort
void sort(ExtMove* mlist)
{
    bool isSorted = true;
    ExtMove* start = mlist;
    ExtMove* end = NULL;
    Move tempMove;
    Value tempValue;
    for (end = mlist; end->move != MOVE_NONE; end++) {};

    if(end == mlist) return;

    for (ExtMove* i = (end-1); i!=start; i--)
    {
        isSorted = true;
        for (ExtMove* j = mlist; j != i; j++)
        {
            if (j->value < (j+1)->value)
            {
                isSorted = false;
                tempMove = j->move;
                tempValue = j->value;
                j->move = (j+1)->move;
                j->value = (j+1)->value;
                (j+1)->move = tempMove;
                (j+1)->value = tempValue;
            }
        }
        if(isSorted) return;
    }
}


/*
 * Stop the main injector loop
 * (not ponder)
 */
void stop_search()
{
    //brainStatus = PAUSED;
    
    watch.reset();
    isSearchMove = false;
}

/*
 * Start the main injector loop
 *
 */
void start_search()
{
    watch.start();
    // Must be done before each Search (this the corresp. function)
    //TT.Clear();
    nbProcessedNodes = 0;
    //sumNodeCutOffIndex = 0;
    if (!isSearchMove)
    {
        ExtMove* end = generate<LEGAL>(currentPosition, moves);
        end->move = MOVE_NONE;
        for(ExtMove* mv = moves; mv!=end; mv++) mv->value = -VALUE_INFINITE;
    } // else moves are already provided

    if(debugMode)
        display_moves(moves);

    // reajust thinking time
    if (brainMode == ADJUSTTIME_MODE && !isPondering)
        update_think_time();

    //std::cout <<  "Switch to THINKING status" << std::endl;
    brainStatus = THINKING;
}

void display_moves(ExtMove* mlist)
{
    Position tPos(currentPosition, NULL);
    cout << tPos.pretty() << endl;
    ostringstream sb;
    sb << "info #### MOVES TO EVALUATE";
    int maxPerLine = 10;
    int i=0;
    for(ExtMove* mv=mlist; mv->move !=MOVE_NONE; mv++)
    {
        if((i % maxPerLine) == 0) sb << std::endl;
        sb << ' ' << move_to_uci(mv->move, tPos.is_chess960()) ;
        i++;
    }
    std::cout <<  sb.str() << std::endl;
}

void set_debug(bool isDebugOn)
{
    debugMode = isDebugOn;
}

void set_option(string name, string value)
{

    if(name == "Hash")
    {
        int hashSize = atoi(value.c_str());
        TTable.resize(hashSize);
    }
    /*
    else if(name == "UCI_LimitStrength")
    {
      UCI_LimitStrength = (value == "true");
    }
    else if(name == "UCI_Elo")
    {
      UCI_Elo = atoi(value.c_str());
    }
     */
    else if(name == "UCI_AnalyseMode")
    {
        UCI_AnalyseMode = (value == "true");
    }
    else if(name == "UCI_Chess960")
    {
        UCI_Chess960 = (value == "true");
    }
    else if(name == "Ponder")
    {
        Ponder = (value == "true");
    }
    else if(name == "Queen weight")
    {
        queenWeight = atoi(value.c_str());
    }
    else if(name == "Rook weight")
    {
        rookWeight = atoi(value.c_str());
    }
    else if(name == "Bishop weight")
    {
        bishopWeight = atoi(value.c_str());
    }
    else if(name == "Knight weight")
    {
        knightWeight = atoi(value.c_str());
    }
    else if(name == "Pawn weight")
    {
        pawnWeight = atoi(value.c_str());
    }

    else if(name == "King mobility")
    {
        kingMobilityFactor = atoi(value.c_str());
    }
    else if(name == "Queen mobility")
    {
        queenMobilityFactor = atoi(value.c_str());
    }
    else if(name == "Rook mobility")
    {
        rookMobilityFactor = atoi(value.c_str());
    }
    else if(name == "Bishop mobility")
    {
        bishopMobilityFactor = atoi(value.c_str());
    }
    else if(name == "Knight mobility")
    {
        knightMobilityFactor = atoi(value.c_str());
    }
    else if(name == "Pawn mobility")
    {
        pawnMobilityFactor = atoi(value.c_str());
    }
    else if(name == "Pawn structure")
    {
        pawnStructureFactor = atoi(value.c_str());
    }
    else if(name == "Smart Timer")
    {
        isSmartTiming = (value == "true");
    }
    else if(name == "Blackbox")
    {
        isBlackbox = (value == "true");
    }
    else
    {
        std::cout <<  "Unkown option: " << name << std::endl;
        return;
    }
    // at this point the option is correctly set
    if(debugMode) std::cout <<  "option [" << name <<"] = " << value << std::endl;
}


// TESTING ONLY !!! (bad perfs)
bool IsCheckmate(Position& position)
{
    // new bitboard implementation
    ExtMove lm[MAX_MOVES];
    lm[0].move = MOVE_NONE;

    //GetLegalMoves(position, lm);
    generate<LEGAL>(position, lm);

    return position.checkers() && (lm[0].move == MOVE_NONE);
}


void SetDepthLevel(int depth)
{
    depthLevel = depth;
}

void SetTimeLevel(int timeLevel)
{
    // Log log;
    // log << "MOVETIME_MODE" << endl;
    brainMode = MOVETIME_MODE;
    thinkDelay = timeLevel;
}

void set_start_position()
{
    set_position("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
}

void set_position(string strPos)
{
    //assert(brainStatus != THINKING);
    TTable.clear();
    clear_history();
    // Threads fuse
    while(brainStatus == THINKING)
    {
        Sleep(10);
    }
    brainStatus=PAUSED;
    //do_sleep = true;
    currentPosition.set(strPos, false, Threads.main_thread());
}

void Ponderhit()
{
    ////throw new NotImplementedException();
}

void SetSearchmovesMode(ExtMove* selectedMoves)
{
    // don't forget to switch to false once used
    ExtMove* c=moves;
    for(ExtMove* mv=selectedMoves; mv->move!=MOVE_NONE; mv++)
    {
        c->move = mv->move;
        c->value = mv->value;
        c++;
    }
    c->move = MOVE_NONE;
    isSearchMove = true;
}

void SetPonderMode()
{
    isPondering = true;
    //brainMode = INFINITE_MODE;
}

void SetPonderHit()
{
    // update oponent's clock
    // Stop is needed to restart local clock for the next ply
    watch.stop();
    uint64_t elpased = watch.get_elapsed_time();

    if(currentPosition.side_to_move() == WHITE)
    {
        btime -= elpased;
    }
    else
    {
        wtime -= elpased;
    }
    watch.start();

    // switch to standard search
    update_think_time();
    isPondering = false;
}

void set_wtime(int timeLeft)
{
    wtime = timeLeft;
    brainMode = ADJUSTTIME_MODE;
}

void set_btime(int timeLeft)
{
    btime = timeLeft;
    brainMode = ADJUSTTIME_MODE;
}

void set_winc(int increment)
{
    winc = increment;
    brainMode = ADJUSTTIME_MODE;
}

void set_binc(int increment)
{
    binc = increment;
    brainMode = ADJUSTTIME_MODE;
}

void SetMovestogo(int movesUntilNextTimeControl)
{
    // TODO: use this value for time control init
    //	1-decrement for each move to zero
    //	2-make a time control
    //	3-reinit value and goto (1)
    movestogo = movesUntilNextTimeControl;

}

void SetDepthMode(int pliesDepth)
{
    Log log;
    log << "DEPTH_MODE" << endl;
    brainMode = DEPTH_MODE;
    depthLevel = pliesDepth;
}

void SetNodesMode(int nodesDepth)
{
    Log log;
    log << "NODES_MODE" << endl;
    brainMode = NODES_MODE;
    nodeMaxNumber = nodesDepth;
}

void SetMateMode(int mateDepth)
{
    Log log;
    log << "MATE_MODE" << endl;
    brainMode = MATE_MODE;
    depthLevel = mateDepth;
}

void SetMovetimeMode(int searchTime)
{
    Log log;
    log << "MOVETIME_MODE" << endl;
    thinkDelay = searchTime;
    brainMode = MOVETIME_MODE;
}

void SetInfiniteMode()
{
    Log log;
    log << "INFINITE_MODE" << endl;
    brainMode = INFINITE_MODE;
}

// void SetPerftMode(int depth)
// {
//     Log log;
//     log << "PERF TEST_MODE" << endl;
//     depthLevel = depth;
//     brainMode = PERFT_MODE;
// }

void SetVerboseMode(bool isVerboseMode)
{
    isVerbose = isVerboseMode;
}

void update_think_time()
{
    int playerInc = 0;
    int playerTime = 0;
    //int oppInc = 0; // NOT USED FOR NOW
    int oppTime = 0;
    if (currentPosition.side_to_move() == WHITE)
    {
        playerInc = winc;
        playerTime = wtime;
        //oppInc = binc;
        oppTime = btime;
    }
    else
    {
        playerInc = binc;
        playerTime = btime;
        //oppInc = winc;
        oppTime = wtime;
    }
    int delay;
    int extraDelay;
    // Smart timing : the engine adjust its think time depending on
    // the time left for itself and the opponent
    // TODO: improve calculation
    // TODO: link calculation to current nodeStack/score state
    if (isSmartTiming)
    {
        if (playerInc == 0)
        {
            // TODO: improve time calculation when there is no inc

            // no increment
            // time must be managed carefully

            if (playerTime > oppTime)
            {
                delay = (int)max(playerInc, playerTime / 20) + (playerTime-oppTime)/2;
            }
            else
            {
                delay = (int)max(playerInc, playerTime / 20) ;
            }
        }
        else
        {
            // base time
            delay = playerInc;

            // extra time taken from the clock
            if(playerTime<10000)
            {
                // for timer < 10s try to save some time
                extraDelay = playerTime*60/100;
            }
            else
            {
                extraDelay = max(playerTime/min(40,max(5,abs(40-currentPosition.game_ply()))),8000);
            }
            delay += extraDelay;
        }

    }
    else
        // NO SMART TIMING
    {
        delay = (int)max(playerInc, playerTime / 20);
    }
    thinkDelay = delay;
    Log log;
    log <<  "Think delay updated : " << thinkDelay << endl;
}


void display_best_pv(unsigned int maxDepth, uint64_t nodeNb, uint64_t time, Value score, Move* pv)
{

    uint64_t nps = (time == 0) ? 0 : ((1000*nodeNb) / time);
    unsigned int hashfull = TTable.GetUsagePerMil();
    string sScore = score_to_string(score);

    std::cout <<  "info"
              << " nodes "    << nodeNb
              << " nps "      << nps
              << " depth "    << maxDepth
              << " seldepth " << selectiveDepth
              << " score "    << sScore
              << " time "     << time
              << " hashfull " << hashfull
              << " pv ";
    display_pv(pv);

    //printf("\ninfo string transposition table hits %d ", TTable.GetHits());

    std::cout <<  std::endl;
    if(debugMode) display_engine_cuttoff_accuracy(maxDepth);
    if(debugMode) display_engine_hashtable_usage();
}

void display_pv(Move* pv)
{
    if(pv != NULL)
    {
        for(Move* mv=pv; (*mv)!=MOVE_NONE; mv++)
        {
            std::cout <<  move_to_uci(*mv, UCI_Chess960) << ' ';
        }
    }
}

string score_to_string(int score)
{
    stringstream ssScore;
    if( score < -VALUE_KNOWN_WIN || score > VALUE_KNOWN_WIN )
    {
        int plies = 0;
        if (score > 0)
        {
            plies = (VALUE_MATE - score);
        }
        else
        {
            plies = (-VALUE_MATE - score);
        }
        ssScore << "mate " << plies;
    }
    else
    {
        ssScore << "cp " << score;
    }
    return ssScore.str();
}

Position& GetCurrentPosition()
{
    return currentPosition;
}


string GetEngineInfo(bool to_uci)
{
    const string months("Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec");
    string month, day, year;
    stringstream infos, date(__DATE__); // From compiler, format is "Sep 21 2008"

    infos << (to_uci ? "id name " : "")
          << "OliveChess " << Version ;

    if (Version.empty())
    {
        date >> month >> day >> year;
        infos << setw(2) << day << setw(2) << (1 + months.find(month) / 4) << year.substr(2);
    }

    infos << (Is64Bit ? " 64bits" : " 32bits")
          << (HasPext ? " BMI2" : (HasPopCnt ? " SSE4.2" : ""))
          << (to_uci  ? "\nid author ": " by ")
          << "Olivier Herau";

    return infos.str();
}

string GetEngineOptions()
{
    // TODO: simplify this method with static content
    //	EngineOptions.h should be removed as it is only used by the GUI

    stringstream sso;

    // OPTIONS TO IMPLEMENT FIRST
    sso << "option name Smart Timing type check default false" << std::endl;
    sso << "option name Hash type spin min 1 max 2048 default 16" << std::endl;
    //sso << "option name Style type combo default Standard var Dumb var Coward var Standard var Risky var Killer" << std::endl;
    sso << "option name UCI_Elo type spin min " << MIN_ENGINE_ELO << " max " << MAX_ENGINE_ELO << " default " << MAX_ENGINE_ELO << std::endl;
    sso << "option name UCI_LimitStrength type check default false" << std::endl;
    sso << "option name UCI_AnalyseMode type check default false" << std::endl;
    //sso << "option name UCI_Chess960 type check default false" << std::endl;
    sso << "option name Ponder type check default true" << std::endl;

    // Chessmen values
    sso << "option name Queen weight type spin min 0 max 2000 default 900" << std::endl;
    sso << "option name Rook weight type spin min 0 max 2000 default 500" << std::endl;
    sso << "option name Bishop weight type spin min 0 max 2000 default 325" << std::endl;
    sso << "option name Knight weight type spin min 0 max 2000 default 325" << std::endl;
    sso << "option name Pawn weight type spin min 0 max 2000 default 85" << std::endl;

    // Mobility factor
    sso << "option name King mobility type spin min 0 max 100 default 100" << std::endl;
    sso << "option name Queen mobility type spin min 0 max 100 default 100" << std::endl;
    sso << "option name Rook mobility type spin min 0 max 100 default 100" << std::endl;
    sso << "option name Bishop mobility type spin min 0 max 100 default 100" << std::endl;
    sso << "option name Knight mobility type spin min 0 max 100 default 100" << std::endl;
    sso << "option name Pawn mobility type spin min 0 max 100 default 100" << std::endl;

    // Pawn structure
    sso << "option name Pawn structure type spin min 0 max 20 default 5" << std::endl;

    //sso << "option name Selectivity type spin default 2 min 0 max 4" << std::endl;

    /* STANDARD GENERIC OPTIONS
    sso << "option name NalimovCache type spin min 1 max 256 default 32" << std::endl;
    sso << "option name NalimovPath type string default " << std::endl;
    sso << "option name Ponder type check default true" << std::endl;
    sso << "option name OwnBook type check default true" << std::endl;
    sso << "option name MultiPV type spin min 1 max 127 default 1" << std::endl;
    sso << "option name UCI_SearchMoves type check default true" << std::endl;
    sso << "option name UCI_ShowCurrLine type check default false" << std::endl;
    sso << "option name UCI_Chess960 type check default false" << std::endl;
    sso << "option name UCI_Opponent type string default " << std::endl;
    */
    /* HIARCS options
    option name CPU Usage type spin min 1 max 100 default 100
    option name OwnBook type check default true
    option name Book Learning type check default false
    option name Book Mode type combo default Tournament var Wild var Surprise var Dynamic var Tournament
    option name Book Positions type combo default 2581613 var 2581613
    option name Book Status type combo default Hiarcs13cBook.hcs OK var Hiarcs13cBook.hcs OK
    option name Use Tablebases type combo var Often var Normally var Rarely var Never default Never
    option name Swindle Opponent type check default true
    option name Smart Search type check default true
    option name Optimistic Search type check default true
    option name Combinations type check default true
    option name Search Selectivity type spin min 0 max 7 default 4
    option name Playing Style type combo default Active var Active var Aggressive var Solid
    */

    return sso.str();
}

void SetSmartTiming(bool value)
{
    isSmartTiming = value;
}

void display_engine_monitor()
{
    long time = watch.get_elapsed_time();
    uint64_t nps = (time == 0) ? 0 : ((1000*nbProcessedNodes) / time);
    std::cout <<  "info"
              << " depth " << depthLevel
              << " time " << time
              << " nodes " << nbProcessedNodes
              << " nps " << nps
              << " tbhits " << TTable.GetHits() << std::endl;
    std::cout << "info hashfull " << TTable.GetUsagePerMil() << std::endl;
}

void display_engine_hashtable_usage()
{
    std::cout << "info hashfull " << TTable.GetUsagePerMil() << std::endl;
    std::cout << "info tbhits " << TTable.GetHits() << std::endl;
}

void display_engine_cuttoff_accuracy(int maxDepth)
{
    int cuttoffaccuracy = (nbCurrSearchTotalGenNodes == 0) ? 0 : 100.0-(100.0*nbCurrSearchProcessedNodes / nbCurrSearchTotalGenNodes);
    std::cout << "info string cut-off accuracy: " << cuttoffaccuracy <<  '%' << std::endl;
    if(maxDepth<20)
    {
        cutOffStatTable[maxDepth] = cuttoffaccuracy;
    }
}

void display_engine_movelist()
{
    int i=0;
    for(ExtMove* mv=moves; mv->move!=MOVE_NONE; mv++)
    {
        i++;
        std::cout <<  "info currmove " << move_to_uci(mv->move, currentPosition.is_chess960()) << " currmovenumber " << i << std::endl;
    }
}

void display_engine_depthstats(int* statTable,int maxDepth)
{
    int tableSize = 20;
    int minIndex=2;
    int maxIndex=min(maxDepth,tableSize);
    int range = maxIndex-minIndex+1 ;
    char bar = '8';
    //char halfbar = 'o';
    char nobar = ' ';
    //int minVal=0;
    int maxVal=100;
    int i;
    if(maxIndex<minIndex) return;
    /*for(int i=0;i<tableSize;i++)
    {
      minVal = min(minVal,statTable[i]);
      maxVal = max(maxVal,statTable[i]);
    }*/

    cout << "100%" << endl;
    int step = 10;
    for(int v=100; v>=0; v-=step)
    {
        cout << " | ";
        for(i=minIndex; i<=maxIndex; i++)
        {
            if( ((statTable[i] * 100.0)/maxVal) > v)
                cout << bar << bar << nobar;
            //else if( ((statTable[i] * 100.0)/maxVal) > (v-(step/2)))
            //  cout << halfbar << halfbar << nobar;
            else
                cout << nobar << nobar << nobar;
        }
        cout << endl;
    }
    cout << " +-";
    for(i=minIndex; i<=maxIndex; i++)
    {
        cout << "---";
    }
    cout << endl << "0% ";
    for(i=minIndex; i<=maxIndex; i++)
    {
        if(i<10)
            cout << ' ';
        cout << i << ' ';
    }
    cout << endl;

    int sum=0;
    for(i=minIndex; i<=maxIndex; i++)
    {
        sum += statTable[i];
    }
    int mean = (sum/range);

    sum = 0;
    for(i=minIndex; i<=maxIndex; i++)
    {
        sum += (statTable[i] - mean)*(statTable[i] - mean);
    }
    int stdDev = sqrt((float)(sum/range));
    cout << "mean=" << mean << " stdDev=" << stdDev << endl;
}
