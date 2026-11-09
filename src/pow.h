#ifndef NAKACORE_POW_H
#define NAKACORE_POW_H

#include "block.h"
#include <openssl/sha.h>
#include <vector>
#include <array>
#include <cstdint>
#include <cstring>

namespace NakaCore {

class ProofOfWork {
public:
    // Doble hashing SHA-256 estándar para la cabecera del bloque (80 bytes)
    static std::array<uint8_t, 32> HashHeader(const BlockHeader& header) {
        std::vector<uint8_t> serialized(80);
        std::memcpy(serialized.data(), &header.version, 4);
        std::memcpy(serialized.data() + 4, header.hash_prev_block.data(), 32);
        std::memcpy(serialized.data() + 36, header.hash_merkle_root.data(), 32);
        std::memcpy(serialized.data() + 68, &header.timestamp, 4);
        std::memcpy(serialized.data() + 72, &header.bits, 4);
        std::memcpy(serialized.data() + 76, &header.nonce, 4);

        std::array<uint8_t, 32> hash1;
        std::array<uint8_t, 32> hash2;

        SHA256(serialized.data(), serialized.size(), hash1.data());
        SHA256(hash1.data(), hash1.size(), hash2.data());

        return hash2;
    }

    // Validación de la prueba de trabajo según los bits de dificultad
    static bool CheckProofOfWork(const BlockHeader& header) {
        std::array<uint8_t, 32> hash = HashHeader(header);
        uint32_t target_prefix = header.bits >> 24;
        for (size_t i = 0; i < target_prefix && i < 32; ++i) {
            if (hash[i] != 0) return false;
        }
        return true;
    }

    // Rutina de minería iterativa para encontrar un nonce válido
    static bool MineBlock(Block& block, uint32_t max_iterations = 5000000) {
        for (uint32_t i = 0; i < max_iterations; ++i) {
            if (CheckProofOfWork(block.header)) {
                return true;
            }
            block.header.nonce++;
        }
        return false;
    }
};

} // namespace NakaCore

#endif // NAKACORE_POW_H
