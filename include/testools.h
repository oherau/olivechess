#ifndef TESTOOLS_H
#define TESTOOLS_H

#include <map>
#include <string>
using namespace std;

typedef unsigned long long u64;
class DebugTest
{

public:
    void LaunchTests();
    void Sandbox();
    void TestLegalMoves(map<string,string> & legalMovesDatabase);
    void TestEvaluate(map<string,string> & evalDatabase);
    bool TestPlayUnplayMoves(map<string,string> & legalMovesDatabase,  bool verbose);
    void TestSearch();
    void TestEloAdjustment(map<string,string> & testPositions, int thinkTime);
    void TestExpectedBestMove(map<string,string> & bestMovesDatabase, int duration);
    bool TestComparedSearch(map<string,string> & testPositions, int duration, string imp1, string imp2);

    void TestAlphaBetaIter();
    void TestBitboards();
    void DisplayResults(string title, map<string, string> summary);

    void initLegalMovesDatabase(map<string,string> & testPositions);
    void initBestMovesDatabase(map<string,string> & bestMovesDatabase);
    void initMatesDatabase(map<string,string> & matesDatabase);
    void initNoCheck(map<string,string> & noCheck);
    void initInCheck(map<string,string> & inCheck);
    void initInCheckmate(map<string,string> & inCheckmate);

    void TestBitBoardLocalEval();
    void TestPositionLoading();
    void TestBitBoards();
    void TestIsCheck(map<string,string> & noCheck, map<string,string> & inCheck, map<string,string> & inCheckmate);
    void DisplayNonVerboseTestResult(string testFctName, bool result);
    bool IsEqual(ExtMove* expected, ExtMove* list, bool str_compare = false);
    void TestOptions();
    void TestHashCode();
    int GetNbPieces(Position& position);
    void DisplayMoves(ExtMove* moves);

    void DisplayCheckStatus(Position& position);
    void StringToMoves(string s, ExtMove* moves);

    void BenchMoveGeneration(map<string,string> & positionsDB, int depth);
    void initPerftDatabase(map<string,string> & perftPosition);
};

#endif
