#ifndef UCI_H
#define UCI_H

#include <iostream>
#include <sstream>
#include "engine.h"

void uci_loop();
void uci_set_position(istringstream& is);
void uci_set_option(istringstream& is);
void uci_go(istringstream& is);

#endif
