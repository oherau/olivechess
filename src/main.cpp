#include <iostream>
#include "logger.h"
#include "bitboard.h"
#include "thread.h"
#include "uci.h"
#include "engine.h"
using namespace std;

int main ()
{
    std::cout <<  GetEngineInfo(false) << std::endl;
    Bitboards::init();
    Position::init();
    Threads.init();
    engine_init();
    uci_loop();
    return 0;
}

