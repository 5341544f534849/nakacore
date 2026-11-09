#ifndef NAKACORE_UTXO_H
#define NAKACORE_UTXO_H

#include "block.h"
#include <unordered_map>
#include <array>
#include <cstdint>

namespace NakaCore {

struct OutPoint {
    std::array<uint8_t, 32> txid;
    uint32_t index;

    bool operator==(const OutPoint& other) const {
        return txid == other.txid && index == other.index;
    }
};

struct OutPointHash {
    size_t operator()(const OutPoint& op) const {
        size_t hash = 0;
        for (uint8_t b : op.txid) {
            hash ^= std::hash<uint8_t>{}(b) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        }
        hash ^= std::hash<uint32_t>{}(op.index) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        return hash;
    }
};

struct UTXOEntry {
    TxOut output;
    bool is_coinbase;
    uint32_t height;
};

class UTXOSet {
private:
    std::unordered_map<OutPoint, UTXOEntry, OutPointHash> utxo_pool;

public:
    bool GetUTXO(const OutPoint& outpoint, UTXOEntry& entry) const {
        auto it = utxo_pool.find(outpoint);
        if (it != utxo_pool.end()) {
            entry = it->second;
            return true;
        }
        return false;
    }

    void AddUTXO(const OutPoint& outpoint, const TxOut& output, bool is_coinbase, uint32_t height) {
        utxo_pool[outpoint] = UTXOEntry{output, is_coinbase, height};
    }

    bool SpendUTXO(const OutPoint& outpoint) {
        return utxo_pool.erase(outpoint) > 0;
    }

    size_t Size() const {
        return utxo_pool.size();
    }
};

} // namespace NakaCore

#endif // NAKACORE_UTXO_H
