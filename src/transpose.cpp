#include "transpose.h"
#include <cassert>

TranspositionTable::TranspositionTable() {
    resize(1); // 1 Mo par défaut
}

void TranspositionTable::resize(int sizeInMB) {
    assert(sizeInMB > 0);
    clear();
    
    // Calcul du nombre maximum d'éléments basé sur la taille mémoire
    // C++20: Utilisation de size_t pour les calculs de mémoire
    maxSize = (1048576ULL * sizeInMB) / (sizeof(Key) + sizeof(TTElem));
    
    // Pré-allocation pour éviter les re-hachages fréquents
    table.reserve(maxSize);
}

void TranspositionTable::clear() {
    table.clear();
    hits = 0;
}

bool TranspositionTable::lookup(const Position& p, int height, Value* alpha, Value* beta, Value* hashvalue) {
    auto it = table.find(p.key());
    
    if (it == table.end()) {
        return false;
    }

    const TTElem& entry = it->second;

    // On ne retourne une valeur que si la profondeur enregistrée est suffisante
    if (entry.height < height) {
        return false;
    }

    *alpha = static_cast<Value>(entry.alpha);
    *beta = static_cast<Value>(entry.beta);
    *hashvalue = static_cast<Value>(entry.bestvalue);

    hits++;
    return true;
}

void TranspositionTable::store(const Position& p, int height, Value bestvalue, Value alpha, Value beta) {
    Key key = p.key();
    auto it = table.find(key);

    if (it == table.end()) {
        // Nouvelle entrée
        if (table.size() >= maxSize) {
            // Stratégie simple de remplacement : on vide si plein (ou on pourrait effacer une entrée aléatoire)
            // Dans un moteur pro, on utilise souvent un index fixe au lieu d'un map.
            table.clear(); 
        }
        table[key] = {height, static_cast<int>(bestvalue), static_cast<int>(alpha), static_cast<int>(beta)};
    } else {
        // Mise à jour si la nouvelle recherche est plus profonde
        if (it->second.height < height) {
            it->second = {height, static_cast<int>(bestvalue), static_cast<int>(alpha), static_cast<int>(beta)};
        }
    }
}

uint32_t TranspositionTable::GetUsagePerMil() const {
    if (maxSize == 0) return 0;
    return static_cast<uint32_t>((1000ULL * table.size()) / maxSize);
}