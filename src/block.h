#ifndef NAKACORE_BLOCK_H
#define NAKACORE_BLOCK_H

#include <cstdint>
#include <vector>
#include <string>
#include <array>

namespace NakaCore {

// Parámetros monetarios globales
constexpr uint64_t MAX_SUPPLY = 20'000'000 * 100'000'000ULL; // 20M NKC en nakas (8 decimales)
constexpr uint32_t TARGET_BLOCK_TIME = 600; // 10 minutos por bloque

struct TxOut {
    uint64_t value; // Valor en unidades atómicas (nakas)
    std::vector<uint8_t> pk_script;
};

struct Transaction {
    int32_t version;
    std::vector<TxOut> vout;
    uint32_t lock_time;
};

struct BlockHeader {
    int32_t version;
    std::array<uint8_t, 32> hash_prev_block;
    std::array<uint8_t, 32> hash_merkle_root;
    uint32_t timestamp;
    uint32_t bits;
    uint32_t nonce;
};

struct Block {
    BlockHeader header;
    std::vector<Transaction> vtx;
};

Block CreateGenesisBlock();

} // namespace NakaCore

#endif // NAKACORE_BLOCK_H
