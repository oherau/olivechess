#ifndef TRANSPOSITION_H
#define TRANSPOSITION_H

#include <unordered_map>
#include <cstdint>
#include "position.h"
#include "types.h"

// Utilisation d'une structure compacte pour limiter l'empreinte mémoire
struct TTElem {
    int32_t height;
    int32_t bestvalue;
    int32_t alpha;
    int32_t beta;
};

class TranspositionTable {
public:
    TranspositionTable();
    ~TranspositionTable() = default;

    void resize(int sizeInMB);
    uint32_t GetUsagePerMil() const;
    void clear();

    void store(const Position& p, int height, Value bestvalue, Value alpha, Value beta);
    bool lookup(const Position& p, int height, Value* alpha, Value* beta, Value* hashvalue);

    uint64_t GetHits() const { return hits; }

private:
    size_t maxSize; 
    // unordered_map est bien plus adapté pour une table de hachage de moteur d'échecs
    std::unordered_map<Key, TTElem> table;
    uint64_t hits{0};
};

extern TranspositionTable TTable; // Déclaration globale

#endif