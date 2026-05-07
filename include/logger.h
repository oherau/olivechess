#ifndef LOGGER_H
#define LOGGER_H

#include <fstream>
#include <string>

struct Log : public std::ofstream
{
    Log(const std::string& f = "olivechess.log") : std::ofstream(f.c_str(), std::ios::out | std::ios::app) {}
    ~Log() { if (is_open()) close(); }
};

#endif

