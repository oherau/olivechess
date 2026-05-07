#include <iostream>
#include <assert.h>
#include "engine.h"
#include "search.h"
//#include "zobrist.h"
#include "transpose.h"

#ifdef DEBUG
bool debug = true;
#else
bool debug = false;
#endif

void pv_copy(const Move* src, Move* dst, int depth, int maxdepth)
{
    for(int i=depth; i<=(maxdepth+1); i++) dst[i] = src[i];
}

void pv_copy(ExtMove* src, ExtMove* dst)
{
    ExtMove* mvd = dst;
    ExtMove* mvs = src;
    while(mvs->move != MOVE_NONE)
    {
        mvd->move = mvs->move;
        mvd->value = mvs->value;
        mvd++;
        mvs++;
    }
    mvd->move = MOVE_NONE;
}

void pv_copy(Move* src, Move* dst)
{
    Move* mvd = dst;
    Move* mvs = src;
    while((*mvs) != MOVE_NONE)
    {
        (*mvd) = (*mvs);
        mvd++;
        mvs++;
    }
    (*mvd) = MOVE_NONE;
}

void pv_copy(ExtMove* src, ExtMove* dst, int depth, int maxdepth)
{
    int d = 0;
    ExtMove* mvd = dst;
    for(ExtMove* mvs = src; mvs->move != MOVE_NONE; mvs++)
    {
        d++;
        if(d>=maxdepth)
        {
            break;
        }
        else if(d>=depth)
        {
            mvd->move = mvs->move;
            mvd->value = mvs->value;
            mvd++;
        }
    }
    mvd->move = MOVE_NONE;
}

unsigned long recursive_perft_search(Position& pos, int depth) {
    if(depth==0) return 1;
    unsigned long  count=0;
    ExtMove mlist[MAX_MOVES];
    for(int i=0; i<MAX_MOVES; i++) mlist[i].move = MOVE_NONE;
    ExtMove* end = generate<LEGAL>(pos, mlist);
    end->move=MOVE_NONE;
    StateInfo st;
    for(ExtMove* mv = mlist; mv->move!=MOVE_NONE; mv++)
    {
        assert(pos.piece_on(from_sq(mv->move)) != NO_PIECE);
        pos.do_move(mv->move, st);
        count+=recursive_perft_search(pos,depth-1);
        pos.undo_move(mv->move);
    }
    return count;
}

void perft_search(Position& pos, int depth) {
    unsigned long totalCount = 0;
    ExtMove mlist[MAX_MOVES];
    for(int i=0; i<MAX_MOVES; i++) mlist[i].move = MOVE_NONE;
    ExtMove* end = generate<LEGAL>(pos, mlist);
    end->move=MOVE_NONE;
    StateInfo st;
    for(ExtMove* mv = mlist; mv->move!=MOVE_NONE; mv++)
    {
        assert(pos.piece_on(from_sq(mv->move)) != NO_PIECE);
        pos.do_move(mv->move, st);
        int count=recursive_perft_search(pos, depth-1);
        pos.undo_move(mv->move);
        totalCount+=count;
        cout << move_to_uci(mv->move,pos.is_chess960()) << ": " << count << endl;
    }

    cout << endl << "Nodes searched: " << totalCount << endl;
} 

void search(Position& pos, int depth)
{
    Value value, best_value;
    selectiveDepth = depth;
    //selectiveDepth = (int)(depth/1.5) ;//arbitrary selective depth;
    nbProcessedNodes++;
    nbCurrSearchTotalGenNodes=0;
    nbCurrSearchProcessedNodes=0;

    // TODO : global variables should be declared as local for multithreading
    Value alpha = Value(-VALUE_INFINITE);
    Value beta  = Value(+VALUE_INFINITE);
    Value old_alpha = alpha;
    best_value = Value(VALUE_NONE);
    bestPv = NULL;

    int mIndex=0;
    pvhash.clear();
    bool isOkToShowCurrmoves = !isPondering && watch.get_elapsed_time() > 1000;
    for(ExtMove* mv = moves; mv->move!=MOVE_NONE; mv++)
    {
        pvhash.insert(make_pair(mv->move, pvbuffer[mIndex]));;
        mIndex++;
        Move* pv  = pvhash[mv->move];
        pv[0] = mv->move;
        if(isOkToShowCurrmoves) cout << "info depth " << depth <<
            " currmove " << move_to_uci(mv->move,UCI_Chess960) <<
            " currmovenumber " << mIndex
            << endl;
    }

    for(ExtMove* currmove = moves; currmove->move!=MOVE_NONE; currmove++)
    {
        Move* pv    = pvhash[currmove->move];
        int scDelta;

        // Set aspiration window default width
        if (depth >= 5 && abs(currmove->value) < VALUE_KNOWN_WIN)
        {
            scDelta = 16;
            alpha = currmove->value - scDelta;
            beta  = currmove->value + scDelta;
        }
        else
        {
            alpha = Value(-VALUE_INFINITE);
            beta  = Value(+VALUE_INFINITE);
        }

        StateInfo newSt;
        assert(pos.piece_on(from_sq(currmove->move)) != NO_PIECE);
        pos.do_move(currmove->move, newSt);
        nbCurrSearchProcessedNodes++;

        // FIRST LEVEL OF SEARCH - ALWAYS A FULL SEARCH IF TTABLE IN USE (NO ZERO-WINDOW SEARCH)
        // zero-window search narrow the serach window with range base on the previous searches
        // needs at least a few searches not to get locked on a too small or incorrect window
        if(best_value == VALUE_NONE || depth > minZeroWindowDepth)
        {
            // first move, full-window search
            value = -full_search(pos, -beta, -alpha, 1, depth, pv+1);
        }
        else
        {
            // zero-window search
            value = -full_search(pos, -alpha-1, -alpha, 1, depth, pv+1);
            if(value > alpha)
            {
                //.. && score < beta
                // need to perform a full-window re-search
                value = -full_search(pos, -beta, -alpha, 1, depth, pv+1);
            }
        }

        pos.undo_move(currmove->move);
        currmove->value = value;

        if(value <= alpha) // upper bound
        {
            best_value = old_alpha;
        }
        else if(value >= beta) // lower bound
        {
            best_value = beta;
        }
        else // alpha < value < beta (exact value)
        {
            best_value = value;
        }

        if(value>best_value && (best_value == VALUE_NONE || value > alpha))
        {
            best_value = currmove->value;
        }

        if(value > best_value)
        {
            if(value>alpha)
            {
                //if(search_type == SearchNormal)
                alpha = value;
                if(value >= beta) break;
            }

        }
    }

	// If leaving the full scan might be incomplete
	// but at least the previous best move with eval at depth D
	// should have its pv updated properly at depth D+1
    if(brainStatus != THINKING) return;

    // At this point all the moves are properly evaluated
    // we can sort moves by score
    sort(moves);
    //if(debug) DisplayScores(moves);

    // bestMove and bestPv are updated only after all the moves have been studied
    // the previous best move can turn into a very bad move at depth d+1 (trap combination, sacrifice, etc.)
    // TODO: implement UCI_LimitStrength directly in the eval function to induce some erroneous estimation
    // no limitation: best score at index 0
    bestMove = moves[0];
    bestPv    = pvhash[bestMove.move];
    ponderMove = bestPv[1];

    display_best_pv(depth, nbProcessedNodes, watch.get_elapsed_time(), bestMove.value, bestPv);
}


void display_move_buffer(Move* moveBuff, int depth)
{
    std::cout <<  "MOVE BUFFER: ";
    for(int i=0; i<depth; i++)
    {
        std::cout <<  move_to_uci(moveBuff[i], currentPosition.is_chess960()) << " ";
    }
    std::cout <<  std::endl;
}

// Self-called: iso-AlphaBeta (2 fct implementation in one)
// uses Hash table
Value full_search(Position& pos, Value alpha, Value beta, int depth, int maxDepth, Move* pv)
{
    //int height = maxDepth - depth;
    bool useFutility = false;

    Value value     = VALUE_NONE;
    Value bestValue = VALUE_NONE;

    if (brainStatus != THINKING) return VALUE_NONE; // quick exit

    //

    // if(useTrans)
    // {
    //     Value hashvalue;
    //     if(TTable.lookup(pos, depth, height, &alpha, &beta, &hashvalue))
    //     {
    //         return hashvalue;
    //     }
    // }

    // count a process node only if not taken from the TTable
    nbProcessedNodes++;

    if(depth >= maxDepth)
    {
        // last move
        return evaluate_board(pos,currentPosition.side_to_move());
    }


	ExtMove localmoves[MAX_MOVES];
    //ExtMove* end = localmoves;
    //for(int i=0;i<MAX_MOVES;i++) {localmoves[i].move=MOVE_NONE;} // TODO init with memcpy
    //ExtMove* end = localmoves;
    // beyond depth 5 we consider only captures
    // if(depth<selectiveDepth)    end = generate<LEGAL>(pos, localmoves);
    // else                        end = generate<CAPTURES>(pos, localmoves); // TODO: check if not too restrictive
    ExtMove* end = generate<LEGAL>(pos, localmoves);
    end->move=MOVE_NONE;

    int nbMoves = end - localmoves;
    nbCurrSearchTotalGenNodes += nbMoves;
    if (nbMoves == 0)
    {
        // last move
        if (pos.checkers())
        {
            return (pos.side_to_move() != currentPosition.side_to_move()) ? mate_in(depth) : mated_in(depth);
        }
        else
        {
            return VALUE_DRAW;
        }
    }

    /*
    // Selectivity pruning
    if(depth > maxDepth)
    {
      SelectivityPruning(pos, localmoves);
    }
    // TODO: in case of empty list return the correct result
    */

    bestValue = VALUE_NONE;

    // optimistic value for futility pruning
    Value opt_value = Value(+VALUE_INFINITE);

    for(ExtMove* mv = localmoves; mv!=end /*mv->move!=MOVE_NONE*/; mv++)
    {
        // extensions (augment depth if needed)
        // ...

        // history pruning
        // ...

        // futility pruning
        int FutilityMargin = 100;
        // TODO: check if 'futile' move is not a pawn pushed on the 7th rank
        if(useFutility && depth == (maxDepth-1))
        {
            // TODO: apply alpha-beta on pre-leave nodes using futilityMargin
            if(opt_value == +VALUE_INFINITE)
            {
                opt_value = evaluate_board(pos,currentPosition.side_to_move())+FutilityMargin;
            }
            value = opt_value;
            // pruning
            if(value<=alpha)
            {
                if(value>bestValue)
                {
                    bestValue = value;
                }
                // jump to the next move
                continue;
            }
        }

        // TODO: fix issue happening HERE : sometime assertion fails
        //       => check the piece_on() function and board content
        StateInfo newSt;
        assert(pos.piece_on(from_sq(mv->move)) != NO_PIECE);
        pos.do_move(mv->move, newSt);
        nbCurrSearchProcessedNodes++;
        if(bestValue == VALUE_NONE)
        {
        	// first move, always search with full window
            value = -full_search(pos, -beta, -alpha, depth + 1, maxDepth, pv+1);
        }
        else
        {
        	// not the first move, zero window search first
            value = -full_search(pos, -alpha-1, -alpha, depth + 1, maxDepth, pv+1);
            // search failed, re-search with full window
			if(value > alpha)
                value = -full_search(pos, -beta, -alpha, depth + 1, maxDepth, pv+1);
        }
        pos.undo_move(mv->move);

        if(value >= beta)
        {
            // fail hard beta-cutoff - i.e. no pv move detected
            //if( useTrans ) TTable.store(pos, depth, height, value, alpha, beta);
            return beta;
        }
        if(value > alpha)
        {
            // alpha acts like max in MiniMax
            alpha = value;
            pv[0] = mv->move;
        }
    }

    //if(useTrans /*&& height > minTransDepth*/ && brainStatus == THINKING) TTable.store(pos, depth, height, value, alpha, beta);

    return alpha;
}

