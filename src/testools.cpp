#include <map>
#include <string>
#include <iostream>
#include <sstream>
#include "notation.h"
#include "testools.h"
#include "engine.h"
#include "movegen.h"
#include "platform.h"
//#include <unistd.h>
//#include <cstdio>
using namespace std;


#define STR_RESULT_OK "-- OK --"
#define STR_RESULT_KO "** KO **"


void DebugTest::Sandbox()
{
    std::cout <<  "************ SANDBOX *******************" << std::endl;
    Position pos;

    // Testing casteling
    //pos.SetFromFEN("r3k2r/8/8/8/8/8/8/R3K2R b KQkq - 0 1");
    //Move m = make_castle(SQ_E1,SQ_G1);
    //Move m = make_castle(SQ_E1,SQ_C1);
    //Move m = make_castle(SQ_E8,SQ_G8);
    //Move m = make_castle(SQ_E8,SQ_C8);

    // Testing en-passant
    //pos.SetFromFEN("k7/8/8/2PpP3/8/8/8/K7 w - d6 0 1");
    //Move m = make_enpassant(SQ_C5,SQ_D6);
    //Move m = make_enpassant(SQ_E5,SQ_D6);
    //pos.SetFromFEN("k7/8/8/8/2pPp3/8/8/K7 b - d3 0 1");
    //Move m = make_enpassant(SQ_C4,SQ_D3);
    //Move m = make_enpassant(SQ_E4,SQ_D3);

    //pos.SetFromFEN("rnbqkbnr/pppp1ppp/8/8/4p3/5N2/PPPPPPPP/RNBQKB1R w - - 0 1");
    //Move m = make_move(SQ_F3,SQ_D4);
    //
    //std::cout <<  "Before move " << move_to_uci(m) << std::endl;
    //pos.DebugDisplay();
    //
    //pos.Play(m);
    //std::cout <<  "After playing move "  << move_to_uci(m)  << std::endl;
    //pos.DebugDisplay();
    //
    //pos.Unplay(m);
    //std::cout <<  "After unplay move " << move_to_uci(m)  << std::endl;
    //pos.DebugDisplay();

    // Testing pawn struct eval
    //int pscore;
    //pos.SetFromFEN("k7/8/8/8/8/8/PPPPPPPP/K7 w - - 0 1");
    //pos.DebugDisplay();
    //pscore = EvaluatePawnHandicapScore(pos.board);
    //std::cout <<  "pscore=" << pscore << std::endl;
    //pos.SetFromFEN("k7/8/8/8/8/P7/PPPPPPPP/K7 w - - 0 1");
    //pos.DebugDisplay();
    //pscore = EvaluatePawnHandicapScore(pos.board);
    //std::cout <<  "pscore=" << pscore << std::endl;
    //pos.SetFromFEN("k7/8/8/8/8/1P6/P1PPPPPP/K7 w - - 0 1");
    //pos.DebugDisplay();
    //pscore = EvaluatePawnHandicapScore(pos.board);
    //std::cout <<  "pscore=" << pscore << std::endl;
    //pos.SetFromFEN("k7/8/8/8/8/8/P1PPPPPP/K7 w - - 0 1");
    //pos.DebugDisplay();
    //pscore = EvaluatePawnHandicapScore(pos.board);
    //std::cout <<  "pscore=" << pscore << std::endl;
    //pos.SetFromFEN("k7/8/8/8/8/q2n4/PPPPPPPP/K7 w - - 0 1");
    //pos.DebugDisplay();
    //pscore = EvaluatePawnHandicapScore(pos.board);
    //std::cout <<  "pscore=" << pscore << std::endl;


}



void DebugTest::LaunchTests()
{
    set_debug(true);

    map<string,string> legalMovesDatabase;// = new Hashtable();
    map<string,string> bestMovesDatabase;// = new Hashtable();
    map<string,string> matesDatabase;// = new Hashtable();
    map<string,string> perftDatabase;// = new Hashtable();

    initLegalMovesDatabase(legalMovesDatabase);
    initBestMovesDatabase(bestMovesDatabase);
    initMatesDatabase(matesDatabase);
    initPerftDatabase(perftDatabase);

    map<string,string> noCheck;// = new Hashtable();
    map<string,string> inCheck;// = new Hashtable();
    map<string,string> inCheckmate;// = new Hashtable();

    initNoCheck(noCheck);
    initInCheck(inCheck);
    initInCheckmate(inCheckmate);
    set_debug(true);
    std::cout <<
              "*****************************************\n"
              "** FUNCTIONAL TEST SET                 **\n"
              "*****************************************\n"
              " 0 - Sandbox                             \n"
              " 1 - TestIsCheck                         \n"
              " 2 - TestLegalMoves                      \n"
              " 3 - TestPlayUnplayMoves                 \n"
              " 4 - TestSearch                          \n"
              " 5 - TestExpectedBestMove                \n"
              " 6 - TestMates                           \n"
              " 7 - TestEloAdjustment                   \n"
              " 8 - Perft                               \n"
              " 9 - Evaluate                            \n"
              "*****************************************\n";
    int choice, duration, depth;

    std::cout <<  "Test:";
    cin >> choice;

    switch (choice)
    {
        case 0:
            Sandbox();
            break;
        case 1:
            TestIsCheck(noCheck,inCheck,inCheckmate);
            break;
        case 2:
            TestLegalMoves(legalMovesDatabase);
            break;
        case 3:
            TestPlayUnplayMoves(legalMovesDatabase, true);
            break;
        case 4:
            TestSearch();
            break;
        case 5:
            std::cout <<  "Duration (ms):";
            cin >> duration;
            TestExpectedBestMove(bestMovesDatabase, duration);
            break;
        case 6:
            std::cout <<  "Duration (ms):";
            cin >> duration;
            TestExpectedBestMove(matesDatabase, duration);
            break;
        case 7:
            std::cout <<  "Duration (ms):";
            cin >> duration;
            TestEloAdjustment(bestMovesDatabase, duration);
            break;
        case 8:
            std::cout <<  "Depth :";
            cin >> depth;
            BenchMoveGeneration(perftDatabase, depth);
            break;
        case 9:
            TestEvaluate(bestMovesDatabase);
            break;
        default:
            std::cout <<  "*********** TEST SECTION EXIT ************" << std::endl;
            break;
    }

}

// Perft ref positions.
// For more detail see:   https://chessprogramming.wikispaces.com/Perft+Results
void DebugTest::initPerftDatabase(map<string,string> & perftPosition)
{

    perftPosition["rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"]           = "001-start position:1 20 400 8902 197281 4865609";
    perftPosition["r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -"]   = "002-Pete McKenzie's Kiwipete:48 2039 97862 4085603 193690690";


}


void DebugTest::initMatesDatabase(map<string,string> & matesDatabase)
{
    matesDatabase["k7/8/K7/8/8/6Rr/8/8 w - - 10 10"] = "01-dilema gain or mate (white to move):g3g8"; //dilema gain or mate
    matesDatabase["K7/8/k7/8/8/6rR/8/8 b - - 10 10"] =  "02-dilema gain or mate (black to move):g3g8"; //dilema gain or mate
    matesDatabase["1k6/8/1K6/8/7R/8/8/8 w - - 10 10"] =  "03-obvious mate1 (white to play):h4h8"; //obvious mate1
    matesDatabase["1K6/8/1k6/8/7r/8/8/8 b - - 10 10"] =  "04-obvious mate1 (black to play):h4h8"; //obvious mate1
    matesDatabase["6k1/4pp2/3p2p1/2p4p/1pPqPPPP/1P1P4/3Q2K1/5r2 b - - 0 36"] = "05-back mates in 2 (Queen's rush):d4g1"; //mate in 2

    matesDatabase["8/P7/8/8/8/6K1/8/7k w - - 0 1"] = "07-mate in 2 with promotion:a7a8q";
    matesDatabase["8/1P6/8/8/8/6K1/8/7k w - - 1 1"] = "08-promotion with mate in 10:b7b8q";
    matesDatabase["8/5P1k/4BB1r/8/8/6q1/K7/8 w - - 0 1"] = "09-mate with knight promotion:f7f8n";

    // can highlight possible problems with PV display
    matesDatabase["7K/1r6/P7/8/8/8/8/7k w - - 0 1"] = "06-ultra simple position with mate in 10:a6b7";

    // tricky mate
    matesDatabase["7b/8/8/8/8/8/p1K2P1N/k7 w - - 0 1"] = "10-tricky zugzwang mate in 5:f2f4";

}

void DebugTest::initBestMovesDatabase(map<string,string> & bestMovesDatabase)
{

    // version 0.2.1 basic test positions
    bestMovesDatabase["r3q2k/pppppppp/8/1N6/8/8/PPPPPPPP/R3Q2K w - - 10 10"] = "01-minor fork (rook-a8 queen-e8):b5c7";


    bestMovesDatabase["r3k3/pppppppp/8/1N6/8/8/PPPPPPPP/R3K3 w - - 10 10"] ="02-fork (rook-a8 king-e8):b5c7";

    bestMovesDatabase["k7/8/K7/8/8/7r/7R/8 w - - 10 10"] = "03-obvious gain whites:h2h3";
    bestMovesDatabase["K7/8/k7/8/8/7R/7r/8 b - - 10 10"] = "04-obvious gain blacks:h2h3";

    bestMovesDatabase["7K/1r6/P7/8/8/8/8/7k w - - 0 1"] = "05-ultra simple position w (mate 10):a6b7";

    bestMovesDatabase["7k/8/8/8/8/p7/1R6/7K b - - 0 1"] = "06-ultra simple position b:a3b2";

    bestMovesDatabase["r3q2k/pppppppp/8/8/1n6/8/PPPPPPPP/R3Q2K b - - 10 10"] = "07-minor fork (rook-a1 queen-e1):b4c2";

    bestMovesDatabase["1k6/8/1K6/8/7R/8/8/8 w - - 10 10"] = "08-obvious mate1 (white to play):h4h8";

    bestMovesDatabase["8/4B3/2K5/8/8/6k1/7q/8 w - - 0 10"] = "09-bishop pin e7d6(-650)-****(-650) then d6h2(+300):e7d6";
    bestMovesDatabase["k7/8/K7/8/8/6Rr/8/8 w - - 10 10"] = "10-dilema gain or mate (white to move):g3g8";
    bestMovesDatabase["k7/8/K7/8/8/7r/7R/8 w - - 10 10"] = "11-obvious gain whites:h2h3";

    bestMovesDatabase["k7/7R/8/8/8/1K4N1/P3P2P/r3R2r w - - 10 10"] = "12-best gain (rook-a1 or rook-h1):e1h1";


    bestMovesDatabase["K7/8/k7/8/8/6rR/8/8 b - - 10 10"] = "13-dilema gain or mate (black to move):g3g8";

    bestMovesDatabase["8/1Q6/2K5/8/8/6k1/4b3/8 b - - 0 10"] = "14-bishop pin:e2f3";
    bestMovesDatabase["8/1q6/2k5/8/8/6K1/4B3/8 w - - 0 10"] = "14-bishop pin(mirror):e2f3";

    bestMovesDatabase["6k1/4pp2/3p2p1/2p4p/1pPqPPPP/1P1P4/3Q2K1/5r2 b - - 0 36"] = "15-back mates in 2 (Queen's rush):d4g1"; //mate in 2
    bestMovesDatabase["r3k3/pppppppp/8/1P6/1n6/8/PPPPPPPP/R3K3 b - - 10 10 "] ="16-fork (rook-a1 king-e1):b4c2";
    bestMovesDatabase["r1b1kbnr/ppq1pppp/2n5/8/3PPB2/5N2/PPP2PPP/R2QKB1R b KQkq - 1 7"] = "17-Queen free to take bishop:c7f4";

    bestMovesDatabase["1K6/8/1k6/8/7r/8/8/8 b - - 10 10"] ="18-obvious mate1 (black to play):h4h8";
    bestMovesDatabase["1r5k/P7/8/8/8/8/7r/K7 w - - 0 1"] = "19-White win or die:a7b8q";



    //// version 0.2.3 new test positions (long winning combinations and hard positions)
    //bestMovesDatabase["5nnr/P1N2k2/8/1P4Bp/7P/3K1P2/6p1/b6R w - - 0 34"] = "19-tactical 1- lateral move (Rxb):h1a1"; //rook takes bishop
    //bestMovesDatabase["6nr/P1n2k2/8/1P2K1Bp/7P/5P2/6p1/6R1 w - - 0 37"] = "20-tactical 2- Support pawn to ensure prom.:b5b6";
    // Study positions (mostly from the web)
    //bestMovesDatabase["8/8/7p/3KNN1k/2p4p/8/3P2p1/8 w - - 1 1"] = "Behting study (Bohemia magazine, 1906) - 1.Kc6! draws:d5c6";
    //bestMovesDatabase["8/8/8/5pp1/8/1K6/2pp1Q2/2k5 w - - 0 1"] = "Behting position (mate in 12):f2e3";
    //bestMovesDatabase["rnb1kb1r/pp1p1ppp/4p3/4q3/2NN4/2P1n3/PP3PPP/R2QKB1R w KQkq - 0 11"] ="19-getting back some material:f2e3";

}

void DebugTest::initLegalMovesDatabase(map<string,string> & testPositions)
{
    testPositions["7K/1P6/8/8/8/8/8/7k w - - 0 1"] = "007-kings final and promotion:b7b8q b7b8r b7b8b b7b8n h8h7 h8g8 h8g7";
    testPositions["r1bqkbnr/ppp1pppp/2n5/3pP3/8/8/PPPP1PPP/RNBQKBNR w KQkq d6 0 2"] = "00-en passant:e1e2 a2a3 a2a4 b2b3 b2b4 c2c3 c2c4 d2d3 d2d4 f2f3 f2f4 g2g3 g2g4 h2h3 h2h4 e5e6 e5d6 b1a3 b1c3 f1e2 f1d3 f1c4 f1b5 f1a6 g1f3 g1h3 g1e2 d1e2 d1f3 d1g4 d1h5";

    // promotion
    testPositions["7K/1r6/P7/8/8/8/8/7k w - - 0 1"] = "00-promotion moves:a6a7 a6b7 h8g8";
    testPositions["7b/8/8/8/8/8/p1K2P1N/k7 w - - 0 1"] = "10-mate in 5 (10ply):f2f4 f2f3 h2f1 h2f3 h2g4 c2c1 c2d1 c2d2 c2d3 c2b3";

    // quiet positions
    testPositions["k1K5/p7/8/8/8/8/8/8 b - - 1 1"] ="01-pawn + king:a7a6 a7a5"; //b basic pawn moves
    testPositions["r3k2r/8/8/8/8/p6p/P6P/R3K3 w KQkq - 1 1"] ="02-casteling moves:a1b1 a1c1 a1d1 e1d1 e1d2 e1e2 e1f2 e1f1 e1c1";

    // check positions
    testPositions["rnb1k2r/3pPp2/2B3pp/pp6/7B/b1P1PP1P/1P4P1/R3K1NR b - - 0 15"] ="03-no Black king legal moves detection:a8a7 a8a6 b8a6 b8c6 c8b7 c8a6 h8g8 h8f8 h8h7 d7c6 a3b2 b5b4 g6g5 f7f5 h6h5 f7f6 a5a4 a3e7 a3d6 a3c5 a3b4";
    testPositions["rnb1k2r/3p1p2/3b2pp/pp6/4n3/PPP1PPPP/PPP1K1PP/RB1B1BNR w - - 2 14"] ="04-avoid unlegal exposed square d2:e2d3 e2e1 a3a4 b3b4 c3c4 f3e4 f3f4 g3g4 h3h4";
    testPositions["k6K/8/8/8/8/8/PPPPPPPP/8 w - - 1 1"] ="05-pawn moves 1:a2a3 a2a4 b2b3 b2b4 c2c3 c2c4 d2d3 d2d4 e2e3 e2e4 f2f3 f2f4 g2g3 g2g4 h2h3 h2h4 h8g8 h8g7 h8h7"; //w pawn moves
    testPositions["K6k/8/8/8/8/8/8/8 b - - 0 1"] ="06-king moves:h8g8 h8g7 h8h7";

    testPositions["4kb1r/4b3/8/7B/8/8/8/6K1 b k - 0 19"] = "07-Blacks in check - escape or take(3 moves):h8h5 e8d8 e8d7";
    testPositions["K1k5/8/8/8/3R4/8/8/8 w - - 0 1 "] = "08-tower moves 1:d4d1 d4d2 d4d3 d4d5 d4d6 d4d7 d4d8 d4a4 d4b4 d4c4 d4e4 d4f4 d4g4 d4h4 a8a7";
    testPositions["K1k5/8/3p4/8/1P1R2p1/3P4/8/8 w - - 0 1 "] ="09-tower moves 2:d4d5 d4d6 d4c4 d4e4 d4f4 d4g4 a8a7 b4b5";
    testPositions["K1k5/8/8/8/8/3B4/8/8 w - - 0 1 "] = "10-bishop moves 1:d3b1 d3c2 d3e4 d3f5 d3g6 d3h7 d3a6 d3b5 d3c4 d3e2 d3f1 a8a7";
    testPositions["8/8/8/4N3/8/8/8/k6K w - - 10 10"] ="11-knight moves 1:e5f7 e5g6 e5g4 e5f3 e5d3 e5c4 e5c6 e5d7 h1h2 h1g2 h1g1"; // knight moves

    testPositions["k7/8/8/pP6/8/8/8/K7 w - a6 10 10"] ="12-en-passant + pawn push 1 (5 moves):a1a2 a1b2 a1b1 b5b6 b5a6"; // kings + en passant
    testPositions["k7/8/8/1Pp5/8/8/8/K7 w - c6 10 10"] ="13-en-passant + pawn push 2 (5 moves):a1a2 a1b2 a1b1 b5b6 b5c6"; // kings + en passant

    testPositions["rnbqkbnr/ppp4p/3pp1p1/8/2P1p3/5Q2/PP1P1PPP/RNB1KBNR w KQkq - 0 6 "] ="14-White queen in danger:b1a3 b1c3 e1d1 e1e2 f1e2 f1d3 g1e2 g1h3 a2a3 a2a4 b2b3 b2b4 c4c5 d2d3 d2d4 g2g3 g2g4 h2h3 h2h4 f3d1 f3e2 f3a3 f3b3 f3c3 f3d3 f3e3 f3g3 f3h3 f3e4 f3f4 f3f5 f3f6 f3f7 f3f8 f3g4 f3h5";
    testPositions["rnb1k2r/3p1p2/3b2pp/pp3n2/4nPb1/4P1P1/4P1P1/5K2 w - - 2 14"] ="15-White king alone:f1e1 f1g1";
    testPositions["rnb1k2r/3p1p2/2B3pp/pp6/7B/b1P1PP1P/1P4P1/R3K1NR b - - 0 15"] ="16-Black king unlegal moves detection(only 1):e8f8 a8a7 a8a6 b8a6 b8c6 c8b7 c8a6 h8g8 h8f8 h8h7 d7c6 a3b2 b5b4 g6g5 f7f5 h6h5 f7f6 a5a4 a3e7 a3d6 a3c5 a3f8 a3b4";
    testPositions["7k/1K1n4/8/1n6/8/8/8/8 w - - 0 15"] = "17-King vs knights(diags only):b7a8 b7c8 b7a6 b7c6";
    testPositions["rnb1k2r/3p1p2/5Bpp/pp6/8/b1P1PP1P/1P4P1/R1B1K1NR b - - 0 14"] ="18-King pb with e8d8:a3b2 e8f8 a5a4 b5b4 g6g5 h6h5 d7d6 d7d5 a3b4 a3c5 a3d6 a3e7 a3f8 b8c6 b8a6 c8b7 c8a6 a8a7 a8a6 h8g8 h8f8 h8h7";
    testPositions["k7/8/K7/8/8/7r/6R1/8 b - - 11 1"] = "19-Mate position but blacks can avoid:h3h8 h3h1 a8b8 h3h7 h3h2 h3g3 h3f3 h3e3 h3d3 h3c3 h3b3 h3a3 h3h4 h3h5 h3h6";
    testPositions["r3k1nr/3n1ppp/1p6/p5q1/P1P1Q3/2P1p1PP/8/R3K1NR b KQkq - 1 19"] ="20-Black king under fire(e8c8 forbidden):e8d8 e8f8 g5e7 g8e7 g5e5 d7e5";


    // check positions
    testPositions["r3k3/ppNppppp/8/8/8/8/PPPPPPPP/R3K3 b - - 0 1"]="0001-King-Rook fork:e8d8 e8f8";
    testPositions["rnbqkbnr/4pQ2/2pp3p/6B1/p1BPP3/2N2NP1/PPP2PP1/R3K2R b KQkq - 0 11"] ="0002-check - only one move 2:e8d7";
    testPositions["rn2kbnr/2Nbpppp/8/2pP4/3P1B2/P7/P1P2PPP/R2QKB1R b KQkq - 1 1"] ="0003-check - only one move 1:e8d8";
    testPositions["rB2kbbr/1p6/8/p6Q/2PPP3/8/PP3PPP/R4RK1 b - - 0 19"] ="0004-blacks in check - 5 moves:h8h5 g8f7 e8e7 e8d7 e8d8";

}

void DebugTest::initNoCheck(map<string,string> & noCheck)
{
    //testPositions["k6R/8/K7/8/8/8/8/8 b - - 10 10"] =" checkmate";
    //testPositions["k7/8/K7/8/8/8/8/7B b - - 10 10"] =" check";
    //testPositions["k7/8/K7/8/8/8/8/6B1 b - - 10 10"] =" nothing";
    noCheck["k7/8/K7/8/8/8/8/6B1 b - - 10 10"] = "1 - no check or checkmate";
    noCheck["k7/8/K7/8/8/6Rr/8/8 w - - 10 10"] = "2 - dilema gain or mate"; //dilema gain or mate
    noCheck["K7/8/k7/8/8/6rR/8/8 b - - 10 10"] =  "3 - dilema gain or mate"; //dilema gain or mate
    noCheck["1k6/8/1K6/8/7R/8/8/8 w - - 10 10"] =  "4 - obvious mate1"; //obvious mate1
    noCheck["1K6/8/1k6/8/7r/8/8/8 b - - 10 10"] =  "5 - obvious mate1";
}
void DebugTest::initInCheck(map<string,string> & inCheck)
{
    inCheck["k6R/8/K7/8/8/8/8/8 b - - 10 10"] ="1-Black is checkmate";
    inCheck["k7/8/K7/8/8/8/8/7B b - - 10 10"] ="2-check";
    //inCheck["k7/8/K7/8/8/8/8/6B1 b - - 10 10"] ="3-nothing";
    inCheck["K7/8/k7/8/8/8/8/7b w - - 10 10"] = "4-White is checkmate";
    inCheck["r3k1nr/3n1ppp/1p6/p5q1/P1P1Q3/2P1p1PP/8/R3K1NR b KQkq - 1 19"] ="Black king under fire(e8c8 forbidden):e8d8 e8f8 g5e7 g8e7 g5e5 d7e5";
}
void DebugTest::initInCheckmate(map<string,string> & inCheckmate)
{
    inCheckmate["k6R/8/K7/8/8/8/8/8 b - - 10 10"] ="1-Black is checkmate";
    //inCheckmate["k7/8/K7/8/8/8/8/7B b - - 10 10"] ="2-check";
    //inCheckmate["k7/8/K7/8/8/8/8/6B1 b - - 10 10"] ="3-nothing";
    inCheckmate["K6r/8/k7/8/8/8/8/8 w - - 10 10"] = "4-White is checkmate";
}


void DebugTest::TestSearch()
{
    //string fen = "rnbqkbnr/pppp1ppp/8/4p3/8/5N2/PPPPPPPP/RNBQKB1R b KQkq - 0 2";
    std::cout <<  "Starting Search test" << std::endl;
    //SetPosition(fen);
    set_option("Search Algorithm","PVSZWS") ;
    set_start_position();
    std::cout << GetCurrentPosition().pretty();
    SetDepthLevel(5);
    start_search();
}

void DebugTest::TestBitBoardLocalEval()
{
    /*
    Dictionary<string, BitBoardNode> hash1 = new Dictionary<string, BitBoardNode>();
    Dictionary<string, ArrayBoardNode> hash2 = new Dictionary<string, ArrayBoardNode>();

    for (int testNb = 0; testNb < 5; testNb++)
    {
    BitBoard bb = new BitBoard();
    ArrayBoard ab = new ArrayBoard();
    //bb.SetStartPosition();
    //ab.SetStartPosition();
    string fen = getRandomFEN();
    ab.SetFromFEN(fen);
    bb.SetFromFEN(fen);
    BitBoardNode bitNode = new BitBoardNode(Move.Null, bb, 0, hash1);
    ArrayBoardNode arrayNode = new ArrayBoardNode(Move.Null, ab, 0);

    std::cout <<  "FEN: " + fen);
    arrayNode.UpdateScore(1, ab.currentPosition.side_to_move());
    std::cout <<  "ArrayBoard score=" + arrayNode.GetScore());
    std::cout <<  "BitBoard score=" + bitNode.GetScore());
    if (arrayNode.GetScore() == bitNode.GetScore())
    {
    std::cout <<  " => ok");
    } else {
    std::cout <<  " => KO");
    }

    }* */
}

void DebugTest::TestPositionLoading()
{
    //TODO : this position is incorrectly initialysed
    // position startpos moves d2d4 f7f6 e2e4 f6f5 e4f5 g7g5 c1g5 e8f7 g5e7 f7e7 f2f4 e7e8 b1c3 e8e7 g1f3 e7e8 f1d3 e8e7 f5f6 e7f6 d3h7 f6e6 h7g8 h8g8 g2g4 g8g7 h2h4 e6e7 f4f5 g7g8 f5f6 e7e6 f6f7 g8g7 g4g5 e6e7 h4h5 g7f7 h1f1 f7f3 d1f3 e7d6 f3f8 d8f8 f1f8 d6c6 f8c8 c6b6 c8b8 a8b8 g5g6 c7c6 h5h6 b8f8 g6g7 f8f7 g7g8q b6c7 g8f7 c7d6 f7d7 d6d7 h6h7 d7d8 h7h8q d8d7 e1d2 d7c7 a1f1 c7b6 f1f7 a7a6 f7b7 b6a5 b7e7 c6c5 d4c5 a5b4 c5c6 b4a5 c6c7 a5b4 c7c8q b4a5 c8a6 a5a6 a2a4


}

void DebugTest::TestBitBoards()
{
    /*
    BitBoard bitBoard = new BitBoard();
    ArrayBoard position = new ArrayBoard();

    position.SetStartPosition();
    bitBoard.SetStartPosition();
    //position.SetPiece(WR, Square.A1);
    //bitBoard.SetPiece(WR, Square.A1);

    std::cout <<  "sq/ppiece/bbpiece/result");
    for(int i=0;i<64;i++)
    {
    System.Console.Write(Square.ToString(i)+": ");
    Piece pPos = position.GetPiece(i);
    Piece pBb = bitBoard.GetPiece(i);

    System.Console.Write(pPos.ToString() + "/" + pBb.ToString());

    if(pPos == pBb)
    {
    System.Console.Write(" => ok");
    } else
    {
    System.Console.Write(" => KO !!!");
    }
    std::cout <<  std::endl ;
    }
    * */

}

void DebugTest::TestEloAdjustment(map<string,string> & testPositions, int thinkTime)
{
    map<string, string> summary;
    SetVerboseMode(true);
    set_option("UCI_LimitStrength","true");

    int stepNb = 4;

    for(int eloStrength=MIN_ENGINE_ELO ; eloStrength<=MAX_ENGINE_ELO ; eloStrength+=((MAX_ENGINE_ELO-MIN_ENGINE_ELO)/stepNb))
    {
        std::ostringstream eloString;
        eloString << eloStrength;
        set_option("UCI_Elo",eloString.str());

        std::cout <<  "##############################" << std::endl;
        std::cout <<  "## Testing ELO = " << eloStrength << std::endl;
        std::cout <<  "##############################" << std::endl;

        for(map<string,string>::iterator item = testPositions.begin(); item != testPositions.end(); item++)
        {

            string fen = (*item).first;
            set_position(fen);
            string str = (*item).second;
            size_t pos = str.find(':');
            string expectedMove = str.substr(pos+1);
            SetTimeLevel(thinkTime);
            start_search();
            Sleep(thinkTime + 500);
            std::cout <<  "##############################" << std::endl;
            std::cout <<  (*item).second << std::endl;
            std::cout <<  "FEN: " << fen <<std::endl;
            std::cout <<  "Best move:     " << move_to_uci(bestMove.move, UCI_Chess960) << std::endl;
            std::cout <<  "Expected move: " << expectedMove << std::endl;

            std::ostringstream testName;
            testName << "ELO=" << (eloStrength<1000?"0":"") << eloStrength << " " << (*item).second;
            if (move_to_uci(bestMove.move, UCI_Chess960) == expectedMove)
            {
                std::cout <<  "=> OK" << std::endl;
                summary[testName.str()] = STR_RESULT_OK;
            }
            else
            {
                std::cout <<  "=> KO" << std::endl;
                summary[testName.str()] = STR_RESULT_KO;
            }
            std::cout <<  "##############################" << std::endl;
            std::cout <<  std::endl ;
        }
    }
    DisplayResults("TestEloAdjustment ", summary);

}

void DebugTest::TestExpectedBestMove(map<string,string> & testPositions, int thinkTime)
{
    SetVerboseMode(true);
    set_option("UCI_AnalyseMode","true");
    //SetOption("Blackbox", "true");
    set_option("Search Algorithm","NEGMAX") ;
    //SetOption("Search Algorithm","NEGASCOUT") ;
    //SetOption("Search Algorithm","ALPHABETA") ;
    //SetOption("Search Algorithm","PVSNULLWINDOW") ;

    //SetOption("Search Algorithm","PVSZWS") ;

    map<string, string> summary;
    for(map<string,string>::iterator item = testPositions.begin(); item != testPositions.end(); item++)
    {
        string fen = (*item).first;
        set_position(fen);
        string str = (*item).second;
        size_t pos = str.find(':');
        string expectedMove = str.substr(pos+1);
        SetTimeLevel(thinkTime);
        start_search();
        Sleep(thinkTime + 500);
        std::cout <<  "##############################" << std::endl;
        std::cout << currentPosition.pretty() << std::endl;
        std::cout <<  item->second << std::endl;
        std::cout <<  "FEN: " << fen <<std::endl;
        std::cout <<  "Best move:     " << move_to_uci(bestMove.move, UCI_Chess960) << std::endl;
        std::cout <<  "Expected move: " << expectedMove << std::endl;
        if (move_to_uci(bestMove.move, UCI_Chess960) == expectedMove)
        {
            std::cout <<  "=> OK" << std::endl;
            summary[item->second] = STR_RESULT_OK;
        }
        else
        {
            std::cout <<  "=> KO" << std::endl;
            summary[(*item).second] = STR_RESULT_KO;
        }
        std::cout <<  "##############################" << std::endl;
        std::cout <<  std::endl ;
    }

    DisplayResults("TestExpectedBestMove ", summary);

}

void DebugTest::TestEvaluate(map<string,string> & testPositions)
{
    SetVerboseMode(true);
    set_option("UCI_AnalyseMode","true");
    //SetOption("Blackbox", "true");
    set_option("Search Algorithm","NEGMAX") ;
    //SetOption("Search Algorithm","NEGASCOUT") ;
    //SetOption("Search Algorithm","ALPHABETA") ;
    //SetOption("Search Algorithm","PVSNULLWINDOW") ;

    //SetOption("Search Algorithm","PVSZWS") ;

    map<string, string> summary;
    for(map<string,string>::iterator item = testPositions.begin(); item != testPositions.end(); item++)
    {
        string fen = (*item).first;
        Position p;
        p.set(fen, false, NULL);

        Score psq = p.psq_score();
        Value statyc = evaluate_static_score(p);

        std::cout <<  "##############################" << std::endl;
        std::cout << p.pretty() << std::endl;
        std::cout <<  item->second << std::endl;
        std::cout <<  "FEN: " << fen <<std::endl;
        std::cout <<  "static:     " << statyc << std::endl;
        std::cout <<  "psq   :     " << psq << std::endl;
        std::cout <<  "##############################" << std::endl;
        std::cout <<  std::endl ;
    }
}

void DebugTest::DisplayResults(string title, map<string, string> summary)
{

    std::cout <<  "############################################################"<< std::endl;
    std::cout <<  title << std::endl;
    std::cout <<  "############################################################"<< std::endl;
    //foreach (KeyValuePair<string, string> result in summary)
    for(map<string,string>::iterator result = summary.begin(); result != summary.end(); result++)
    {
        size_t keySize = (*result).first.size();
        std::cout <<  (*result).second << " " << (*result).first.substr(0, min((size_t)53, keySize)) << std::endl;
    }
    std::cout <<  "############################################################"<< std::endl;

}

void DebugTest::TestIsCheck(map<string,string> & noCheck, map<string,string> & inCheck, map<string,string> & inCheckmate)
{

    Position pos;
    map<string, string> noCheckSummary;
    map<string, string> inCheckSummary;
    map<string, string> inCheckmateSummary;

    for(map<string,string>::iterator item = noCheck.begin() ; item != noCheck.end(); item++)
    {
        string fen = (*item).first;
        std::cout <<  "----------------------------" << std::endl;
        std::cout <<  "FEN : " << fen << std::endl;
        pos.set(fen, false, NULL);

        bool isCheck = pos.checkers();
        bool isCheckmate = IsCheckmate(pos);

        if (!isCheck && !isCheckmate)
        {
            std::cout <<  STR_RESULT_OK << std::endl;
            noCheckSummary[(*item).second] = STR_RESULT_OK;
        }
        else
        {
            std::cout <<  STR_RESULT_KO << std::endl;
            noCheckSummary[(*item).second] = STR_RESULT_KO;
        }
    }
    std::cout <<  std::endl ;

    for(map<string,string>::iterator item = inCheck.begin() ; item != inCheck.end(); item++)
    {
        string fen = (*item).first;
        std::cout <<  "----------------------------" << std::endl;
        std::cout <<  "FEN : " << fen << std::endl;
        pos.set(fen, false, NULL);
        bool isCheck = pos.checkers();
        if (isCheck)
        {
            std::cout <<  STR_RESULT_OK ;
            inCheckSummary[(*item).second] = STR_RESULT_OK;
        }
        else
        {
            std::cout <<  STR_RESULT_KO ;
            inCheckSummary[(*item).second] = STR_RESULT_KO;
        }
    }
    std::cout <<  std::endl ;


    for(map<string,string>::iterator item = inCheckmate.begin() ; item != inCheckmate.end(); item++)
    {
        string fen = (*item).first;
        std::cout <<  "----------------------------" << std::endl;
        std::cout <<  "FEN : " << fen << std::endl;
        pos.set(fen, false, NULL);
        bool isCheckmate = IsCheckmate(pos);

        if (isCheckmate)
        {
            std::cout <<  STR_RESULT_OK ;
            inCheckmateSummary[(*item).second] = STR_RESULT_OK;
        }
        else
        {
            std::cout <<  STR_RESULT_KO ;
            inCheckmateSummary[(*item).second] = STR_RESULT_KO;
        }

    }
    std::cout <<  std::endl ;

    DisplayResults("No check positions", noCheckSummary);
    DisplayResults("In check positions", inCheckSummary);
    DisplayResults("In checkmate positions", inCheckmateSummary);
}

bool DebugTest::TestPlayUnplayMoves(map<string,string> & testPositions, bool verbose)
{
    bool result = true;

    map<string, string> summary;

    for(map<string,string>::iterator item = testPositions.begin(); item != testPositions.end(); item++)
    {
        string fen = (*item).first;
        int pbCount = 0;
        long mvNb = 0;
        if (verbose)
            std::cout <<  "FEN: " << fen << std::endl;
        Position pos1;
        Position pos2;
        pos1.set(fen,false,NULL);
        pos2.set(fen, false,NULL);
        ExtMove tempMoves[MAX_MOVES];
        generate<LEGAL>(pos1, tempMoves);

        for(ExtMove* mv=tempMoves; mv->move != MOVE_NONE; mv++)
        {
            StateInfo newSt;
            Move m = mv->move;
            cout << "MOVE : " << move_to_uci(m, UCI_Chess960) << endl;
            cout << "BEFORE PLAY" << endl;
            cout << pos2.pretty() << endl;
            pos2.do_move(m, newSt);
            cout << "AFTER PLAY" << endl;
            cout << pos2.pretty() << endl;
            pos2.undo_move(m);
            cout << "AFTER UNPLAY" << endl;
            cout << pos2.pretty() << endl;
            if (pos2.fen() != pos1.fen())
            {
                result = false;
                pbCount++;
                if (verbose)
                {
                    std::cout <<  "->pb with move " << move_to_uci(m, UCI_Chess960) << std::endl;
                    std::cout <<  "static position (pos1)" << std::endl;
                    cout << pos1.pretty() << endl;
                    std::cout <<  "move/unmove position (pos2)" << std::endl;
                    cout << pos2.pretty() << endl;
                }
                pos2.set(fen, false, NULL);
            }
        }

        if (pbCount == 0)
        {
            if (verbose)
                std::cout <<  "==> OK" << std::endl;
            summary[(*item).second] = STR_RESULT_OK;
        }
        else
        {
            if (verbose)
                std::cout <<  "==> pb on " << pbCount << "/" << mvNb << " moves" << std::endl;
            summary[(*item).second] = STR_RESULT_KO;
        }
    }

    DisplayResults("TestPlayUnplayMoves ", summary);

    return result;
}


bool DebugTest::TestComparedSearch(map<string,string> & testPositions, int duration, string imp1, string imp2)
{
    bool result = true;
    int SLEEPTIME = duration + 500;
    //SetVerboseMode(true);
    map<string, string> summary;

    for(map<string,string>::iterator item = testPositions.begin(); item != testPositions.end(); item++)
    {
        string fen = (*item).first;

        std::cout <<  "*** Calling recusrive version ***" << std::endl;
        set_position(fen);
        set_option("Search Algorithm",imp1);
        SetMovetimeMode(duration);
        start_search();
        Sleep(SLEEPTIME);
        int score1 = bestMove.value;
        ExtMove bm1 = bestMove;

        std::cout <<  "*** Calling iterative version ***" << std::endl;
        set_position(fen);
        set_option("Search Algorithm",imp2);
        SetMovetimeMode(duration);
        start_search();
        Sleep(SLEEPTIME);
        int score2 = bestMove.value;
        ExtMove bm2 = bestMove;

        std::cout <<  "##############################" << std::endl;
        std::cout <<  ((*item).second) << std::endl;
        std::cout <<  "FEN: " << fen << std::endl;
        std::cout <<  imp1 << " search: best move= " << move_to_uci(bm1.move, UCI_Chess960) << "  score=" << score1 << std::endl;
        std::cout <<  imp2 << " saerch: best move= " << move_to_uci(bm2.move, UCI_Chess960) << "  score=" << score2 << std::endl;

        if ((bm1.move == bm2.move)
                && (score1 == score2))
        {
            std::cout <<  "=> OK" << std::endl;
            summary[(*item).second] = STR_RESULT_OK;
        }
        else
        {
            std::cout <<  "=> KO" << std::endl;
            summary[(*item).second] = STR_RESULT_KO;
        }
        std::cout <<  "##############################" << std::endl;
        std::cout <<  std::endl ;
    }

    std::ostringstream t;
    t << "TestComparedSearch : " << imp1 << " vs " << imp2;
    DisplayResults(t.str(), summary);

    return result;
}


void DebugTest::DisplayNonVerboseTestResult(string testFctName, bool result)
{
    if (result)
    {
        std::cout <<  STR_RESULT_OK;
    }
    else
    {
        std::cout <<  STR_RESULT_KO;
    }

    std::cout <<  " " << testFctName;
}

void DebugTest::TestLegalMoves(map<string,string> & testPositions)
{
    ExtMove foundMoves[MAX_MOVES];
    map<string,string> summary;

    for(map<string,string>::iterator item = testPositions.begin(); item != testPositions.end(); item++)
    {
        //string p = ((*item).second).Split(':');
        string str = (*item).second;
        size_t ind = str.find(':');
        string p0 = str.substr(0,ind);
        string p1 = str.substr(ind+1,str.size()-ind-1);

        // ACTIVATE THIS CODE IN CASE OF PB WITH THE TEST SET
        //std::cout <<  "str=" << str << std::endl
        //<< "p0=" << p0 << std::endl
        //<< "p1=" << p1 << std::endl;

        Position pos;
        string fen = (*item).first;
        pos.set(fen, false, NULL);
        cout << pos.pretty() << endl;
        ExtMove expectedMoves[MAX_MOVES];
        expectedMoves[0].move = MOVE_NONE;
        StringToMoves(p1, expectedMoves);

        /*
        		switch(movegenImplementation)
            {
              case MOVEGEN_IMPL_ESC_CHECK_OR_MOVE_NOT_PINNED:
                std::cout <<  "Testing MOVEGEN_IMPL_ESC_CHECK_OR_MOVE_NOT_PINNED" << std::endl;
                GenLegalMoves(pos, foundMoves);
                break;
              default:
                std::cout <<  "Testing MOVEGEN_IMPL_POSSIBLE_THEN_PRUNE_ILLEGAL" << std::endl;
                GetLegalMoves(pos, foundMoves);
                break;
            }
            */
        //GetLegalMoves(pos, foundMoves);
        generate<LEGAL>(pos, foundMoves);

        std::cout <<  "##############################" << std::endl;
        std::cout <<  p0 << std::endl;
        std::cout <<  "FEN: "<< fen << std::endl;

        std::cout <<  "Found moves:    ";
        DisplayMoves(foundMoves);
        std::cout <<  std::endl ;

        std::cout <<  "Expected moves: ";
        DisplayMoves(expectedMoves);
        std::cout <<  std::endl ;

        if (IsEqual(expectedMoves, foundMoves,true))
        {
            std::cout <<  STR_RESULT_OK << std::endl;
            summary[(*item).second] = STR_RESULT_OK;
        }
        else
        {
            std::cout <<  STR_RESULT_KO << std::endl;
            summary[(*item).second] = STR_RESULT_KO;
        }
        std::cout <<  "##############################" << std::endl;
        //std::cout <<  std::endl ;
    }

    DisplayResults("TestLegalMoves ", summary);

}

bool DebugTest::IsEqual(ExtMove*  expected, ExtMove* real, bool str_compare)
{
    bool result = true;
    int nb1 = 0;
    int nb2 = 0;

    for(ExtMove* m1=expected; m1->move!=MOVE_NONE; m1++)
    {
        bool found = false;
        for(ExtMove* m2=real; m2->move!=MOVE_NONE; m2++)
        {
            //if(m1 == m2) new parameter str_compare
            if(str_compare?(move_to_uci(m1->move, UCI_Chess960)==move_to_uci(m2->move, UCI_Chess960)):(m1->move == m2->move))
            {
                found = true;
                break;
            }
        }
        if (!found)
        {
            std::cout <<  "Move of the ref list not found: " << move_to_uci(m1->move, UCI_Chess960) << std::endl;
            result = false;
        }
        nb1++;
    }

    // second pass to detect errors
    for(ExtMove* m2=real; m2->move!=MOVE_NONE; m2++)
    {
        bool found = false;
        for(ExtMove* m1=expected; m1->move!=MOVE_NONE; m1++)
        {
            //if(m1 == m2) new parameter str_compare
            if(str_compare?(move_to_uci(m1->move, UCI_Chess960)==move_to_uci(m2->move, UCI_Chess960)):(m1->move == m2->move))
            {
                found = true;
                break;
            }
        }
        if (!found)
        {
            std::cout <<  "Move in the compared list not expected: " << move_to_uci(m2->move, UCI_Chess960) << std::endl;
            result = false;
        }
        nb2++;
    }

    result &=  (nb1 == nb2);

    return result;
}




void DebugTest::StringToMoves(string s, ExtMove* mvList )
{
    ExtMove* start = mvList;
    istringstream is(s);

    char buffer[6];

    while(is >> buffer)
    {

        //std::cout <<  "Move read: [" << buffer << "]" << std::endl;
        Square from = make_square((File)(buffer[0]-'a'),(Rank)(buffer[1]-'1'));
        Square to = make_square((File)(buffer[2]-'a'),(Rank)(buffer[3]-'1'));
        char promChar = (PieceType)buffer[4]; // can be an incorrect implementation

        PieceType prom = NO_PIECE_TYPE;
        if(promChar == 'q')
        {
            prom = QUEEN;
        }
        else if(promChar == 'r')
        {
            prom = ROOK;
        }
        else if(promChar == 'b')
        {
            prom = BISHOP;
        }
        else if(promChar == 'n')
        {
            prom = KNIGHT;
        }

        Move m ;
        if(prom == NO_PIECE_TYPE)
        {
            m = make_move(from,to);//new Move(smoves[i],pos);
        }
        else
        {
            m = make<PROMOTION>(from,to,prom);//new Move(smoves[i],pos);
        }

        mvList->move = m;
        mvList++;
    }
    mvList->move = MOVE_NONE;

    //std::cout <<  "DEBUG StringToMoves" << std::endl;
    //std::cout <<  "in:  " << s << std::endl;
    //std::cout <<  "out: ";
    for(ExtMove* mv=start; mv->move!=MOVE_NONE; mv++)
    {
        std::cout <<  move_to_uci(mv->move, UCI_Chess960) << ' ';
    }
    std::cout <<  std::endl;
}


void DebugTest::TestOptions()
{
    /*
    Engine engine = new Engine();

    engine.SetOption("PawnValue", "101");
    engine.SetOption("KnightValue", "251");
    engine.SetOption("BishopValue", "253");
    engine.SetOption("RookValue", "501");
    engine.SetOption("QueenValue", "901");
    engine.SetOption("QueekValue", "901");
    engine.SetOption("KingValue", "10001");

    std::cout <<  std::endl ;
    */
}

void DebugTest::TestHashCode()
{
    /*
    Position randomPosition = getRandomPosition();
    Position& pos1 = new Position(randomPosition);
    Position& pos2 = new Position(randomPosition);

    std::cout <<  "Testing hash keys on the same random position (different instances):");
    std::cout <<  "pos1 simpleHash : " + pos1.GetSimplifiedHashCode());
    std::cout <<  "pos2 simpleHash : " + pos2.GetSimplifiedHashCode());
    std::cout <<  "pos1 default hash : " + pos1.GetHashCode());
    std::cout <<  "pos2 default hash : " + pos2.GetHashCode());

    pos1.SetStartPosition();
    pos2.SetStartPosition();

    std::cout <<  "Testing hash keys on the same random position (different instances):");
    std::cout <<  "pos1 simpleHash : " + pos1.GetSimplifiedHashCode());
    std::cout <<  "pos2 simpleHash : " + pos2.GetSimplifiedHashCode());
    std::cout <<  "pos1 default hash : " + pos1.GetHashCode());
    std::cout <<  "pos2 default hash : " + pos2.GetHashCode());

    std::cout <<  std::endl ;
    * */
}


void DebugTest::DisplayMoves(ExtMove* mvList)
{
    for(ExtMove* mv=mvList; mv->move!=MOVE_NONE; mv++)
    {
        std::cout <<  move_to_uci(mv->move, UCI_Chess960) << " ";
    }
}


void DebugTest::DisplayCheckStatus(Position& pos)
{
    std::cout <<  "Position status: " << std::endl;
    if (IsCheckmate(pos))
    {
        std::cout <<  "checkmate !";
    }
    else if (pos.checkers())
    {
        std::cout <<  "check !";
    }
    else
    {
        std::cout <<  "no special status";
    }
    std::cout <<  std::endl ;
}


// typedef unsigned long long u64;

// u64 DebugTest::Perft(Position& pos, int depth)
// {
//     ExtMove perftMoves[MAX_MOVES];
//     u64 nodes = 0;

//     if (depth == 0) return 1;

//     //ExtMove* end = GetLegalMoves(pos,perftMoves);
//     ExtMove* end = generate<LEGAL>(pos, perftMoves);
//     end->move = MOVE_NONE;

//     for (ExtMove* mv=perftMoves; mv->move!=MOVE_NONE; mv++)
//     {
//         StateInfo newSt;
//         pos.do_move(mv->move, newSt);
//         nodes += Perft(pos, depth - 1);
//         pos.undo_move(mv->move);
//     }

//     return nodes;
// }

void DebugTest::BenchMoveGeneration(map<string,string> & positionsDB, int depth)
{
    // Perft - "Kiwipete" position by Peter McKenzie
    //pos.SetFromFEN("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -");

    // initial board
    Position pos;
    Watch watch4;



    std::cout <<  "********************************************"<< std::endl;
    std::cout <<  "* Perft                                    *"<< std::endl;
    std::cout <<  "********************************************"<< std::endl;

    for(map<string,string>::iterator item = positionsDB.begin(); item != positionsDB.end(); item++)
    {
        //string p = ((*item).second).Split(':');
        string str = (*item).second;
        size_t ind = str.find(':');
        string p0 = str.substr(0,ind);
        string p1 = str.substr(ind+1,str.size()-ind-1);

        // ACTIVATE THIS CODE IN CASE OF PB WITH THE TEST SET
        //std::cout <<  "str=" << str << std::endl
        //<< "p0=" << p0 << std::endl
        //<< "p1=" << p1 << std::endl;

        string fen = (*item).first;
        pos.set(fen, false, NULL);
        cout << pos.pretty() << endl;

        //ExtMove expectedMoves[MAX_MOVES];
        //expectedMoves[0].move = MOVE_NONE;
        //StringToMoves(p1, expectedMoves);

        for(int d=1; d<=depth; d++)
        {
            watch4.start();
            unsigned long result = recursive_perft_search(pos, d);
            watch4.stop();

            std::cout << "depth=" << d
                      << "  nodes=" << result
                      << "  time=" << watch4.get_elapsed_time() << " ms"
                      << std::endl;
        }

        /*
                std::cout <<  "##############################" << std::endl;
                std::cout <<  p0 << std::endl;
                std::cout <<  "FEN: "<< fen << std::endl;



                if (IsEqual(expectedMoves, foundMoves,true))
                {
                    std::cout <<  STR_RESULT_OK << std::endl;
                    summary[(*item).second] = STR_RESULT_OK;
                }
                else
                {
                    std::cout <<  STR_RESULT_KO << std::endl;
                    summary[(*item).second] = STR_RESULT_KO;
                }
                std::cout <<  "##############################" << std::endl;
                //std::cout <<  std::endl ;
                */
    }

}
