#include <iostream>
#include <sstream>
#include <string.h>
#include <set>
#include <assert.h>
#include <fstream>
#include "uci.h"
#include "testools.h"
#include "notation.h"
#include "logger.h"

extern bool isSearchMove;

//using namespace std;
StateInfo stateInfos[MAX_MOVES];

void uci_loop()
{
    Log log;
    bool traceInputs = true;
    string cmd, token;

    while (token != "quit")
    {
        //Threads.DisplayStatus();
        if (!getline(cin, cmd)) // Block here waiting for input
            cmd = "quit";

        if(traceInputs) log << cmd << endl;
        // {
        //     log << "gui=>uci " << cmd << endl;
        //     // std::ofstream outfile;
        //     // outfile.open("olivechess.log", std::ios_base::app);
        //     // outfile << "gui=>uci " << cmd << endl;
        // }

        istringstream is(cmd);

        is >> skipws >> token;

        if (token == "quit" || token == "stop")
        {
            stop_search();
        }
        else if (token == "ponderhit")
        {
            SetPonderHit();
        }
        else if (token == "go")
        {
            uci_go(is);
        }
        else if (token == "ucinewgame")
        {
            set_start_position();
        }
        else if (token == "isready")
        {
            cout <<  "readyok" << endl;
        }
        else if (token == "position")
        {
            uci_set_position(is);
        }
        else if (token == "setoption")
        {
            uci_set_option(is);
        }
        else if (token == "d")
        {
            cout << currentPosition.pretty() << endl;
        }
        else if (token == "debug")
        {
            string value;
            is >> skipws >> value;
            set_debug((value=="on"));
        }
        else if (token == "uci")
        {
            cout <<  GetEngineInfo(true) << endl
                 << GetEngineOptions()
                 << "uciok"      << endl;
        }
        else if ( token == "test" )
        {
            set_debug(true);
            DebugTest test;
            test.LaunchTests();
        }
        else
        {
            cout <<  "Unknown command: " << cmd << endl;
        }
    }
}


const char* StartFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

void uci_set_position(istringstream& is)
{
    Move m;
    string token, fen;
    Position fenPos;
    //assert(brainStatus != THINKING);
    //bool prev_value_isSearchMove = isSearchMove;
    //BrainStatus previousBrainStatus = brainStatus;
    isSearchMove = false;
    while(brainStatus == THINKING){
        Sleep(10);
    }

    is >> token;

    if (token == "startpos")
    {
        fen = StartFEN;
        is >> token; // Consume "moves" token if any
    }
    else if (token == "fen")
        while (is >> token && token != "moves")
            fen += token + " ";
    else
        return;

    fenPos.set(fen, UCI_Chess960, NULL);

    // Parse move list (if any)
    int dpth=0;
    while (is >> token && (m = move_from_uci(fenPos, token)) != MOVE_NONE)
    {
        // cout << "info move token: " << token << endl;
        // cout << "info " << currentPosition.pretty() << endl;
        StateInfo* newSt = &(stateInfos[dpth]);
        assert(fenPos.piece_on(from_sq(m)) != NO_PIECE);
        fenPos.do_move(m, *newSt);
        dpth++;
    }

    // TODO: be carefull on that affectation
    currentPosition = fenPos;
    // isSearchMove = prev_value_isSearchMove;
    // brainStatus = previousBrainStatus;
    start_search();
}



// set_option() is called when engine receives the "setoption" UCI command. The
// function updates the UCI option ("name") to the given value ("value").

void uci_set_option(istringstream& is)
{

    string token, name, value;

    is >> token; // Consume "name" token

    // Read option name (can contain spaces)
    while (is >> token && token != "value")
        name += string(" ", !name.empty()) + token;

    // Read option value (can contain spaces)
    while (is >> token)
        value += string(" ", !value.empty()) + token;

    set_option(name,value);
}

// go() is called when engine receives the "go" UCI command. The function sets
// the thinking time and other parameters from the input string, and then starts
// the main searching thread.

void uci_go(istringstream& is)
{
    // default mode for 'go' unless some option specify a different mode
    brainMode = ADJUSTTIME_MODE;

    string token;
    std::set<Move> searchMoves;
    int time[] = { 0, 0 }, inc[] = { 0, 0 };

    bool startAfterParsing = false;
    while (is >> token)
    {
        if (token == "infinite")
        {
            SetInfiniteMode();
            startAfterParsing = true;
        }
        else if (token == "ponder")
        {
            SetPonderMode();
            startAfterParsing = true;
        }
        else if (token == "wtime")
        {
            is >> time[WHITE];
        }
        else if (token == "btime")
        {
            is >> time[BLACK];
        }
        else if (token == "winc")
        {
            is >> inc[WHITE];
        }
        else if (token == "binc")
        {
            is >> inc[BLACK];
        }
        else if (token == "movestogo")
        {
            int movesToGo;
            is >> movesToGo;
            SetMovestogo(movesToGo);
            startAfterParsing = true;
        }
        else if (token == "depth")
        {
            int depth;
            is >> depth;
            SetDepthMode(depth);
            startAfterParsing = true;
        }
        else if (token == "nodes")
        {
            int maxNodes;
            is >> maxNodes;
            SetNodesMode(maxNodes);
            startAfterParsing = true;
        }
        else if (token == "movetime")
        {
            int movetime;
            is >> movetime;
            SetMovetimeMode(movetime);
            startAfterParsing = true;
        }
        else if (token == "searchmoves")
        {
            // TODO: refactor this part
            //while (is >> token)
            //    searchMoves.insert(move_from_uci(pos, token));
        }
        else if (token == "perft")
        {
            // no real search
            int depth;
            is >> depth;
            perft_search(currentPosition, depth);
            
        }
    }

    if(startAfterParsing) start_search();
}
