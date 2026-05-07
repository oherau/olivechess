#include <assert.h>
//#include "zobrist.h"
#include "transpose.h"


TranspositionTable::TranspositionTable()
{
    resize(1); // 1 Mo by default
}

TranspositionTable::~TranspositionTable()
{
}

/**
 * @brief Resize the transposition table
 * 
 * @param sizeInMB new size in MB
 */
void TranspositionTable::resize(int sizeInMB)
{
    assert(sizeInMB>0);
    //std::cout <<  "Resizing Transposition Table to " << sizeInMB "Mb" << std::endl;

    // flush table content
    this->clear();
    // convert MB in bytes
    this->maxSize = (1048576L * sizeInMB) / (sizeof(TTElem)+sizeof(Key));
}


void TranspositionTable::clear()
{
    if(!table.empty())
        table.erase ( table.begin(), table.end() );
    hits = 0;
}


bool TranspositionTable::lookup(const Position& p, /*int depth,*/ int height, Value* alpha, Value* beta, Value* hashvalue)
{
    map<Key, TTElem>::iterator result;
    result = table.find(p.key());
    if(result==table.end())
        return false; // no entry found
    if(result->second.height < height)
        return false;

    // entry found, update values
    // Useless reverse
    if(false/*((p.side_to_move()==WHITE?1:0)+depth+result->second.height+height)%2*/)
    {
        //std::cout <<  "WARNING !!! - odd hashtable hit" << std::endl;
        (*alpha)      = Value(-result->second.beta);
        (*beta)       = Value(-result->second.alpha);
        (*hashvalue)  = Value(-result->second.bestvalue);
    }
    else
    {

        //std::cout <<  "regular hashtable hit" << std::endl;
        (*alpha)      = Value(result->second.alpha);
        (*beta)       = Value(result->second.beta);
        (*hashvalue)  = Value(result->second.bestvalue);
    }

    hits++;
    return true;
}


void TranspositionTable::store(const Position& p, /*int depth,*/ int height, Value bestvalue, Value alpha, Value beta)
{
    Key key = p.key();
    map<Key, TTElem>::iterator result;
    result = table.find(key);
    if(result==table.end())
    {
        // no entry, always store
        //TTElem elem = {depth,bestvalue,alpha,beta};
        TTElem elem = {height,bestvalue,alpha,beta};
        table[key] = elem;
        if(table.size() > maxSize) table.erase(table.begin());
    }
    else
    {
        // entry found, update only deeper results
        if(result->second.height < height)
        {
            result->second.height = height;
            // Useless reverse
            if(false /*((p.side_to_move()==WHITE?1:0)+depth+height)%2*/)
            {
                result->second.alpha = -beta;
                result->second.beta  = -alpha;
                result->second.bestvalue = -bestvalue;
            }
            else
            {
                result->second.alpha = alpha;
                result->second.beta  = beta;
                result->second.bestvalue = bestvalue;
            }
        }
    }
}


unsigned int TranspositionTable::GetUsagePerMil()
{
    return  (1000L * (unsigned long)table.size()) / maxSize;
}


unsigned long TranspositionTable::GetHits()
{
    return hits;
}





















