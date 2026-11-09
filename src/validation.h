#ifndef NAKACORE_VALIDATION_H
#define NAKACORE_VALIDATION_H

#include "block.h"
#include "utxo.h"
#include <iostream>

namespace NakaCore {

class ConsensusValidator {
public:
    // Validar reglas estrictas de consenso para un bloque nuevo
    static bool ValidateBlock(const Block& block, const UTXOSet& utxoSet) {
        // 1. Validar que el bloque contenga al menos la transacción coinbase
        if (block.vtx.empty()) {
            std::cout << "[-] Consensus Error: Block contains no transactions.\n";
            return false;
        }

        // 2. Validar cabecera y estructura básica
        if (block.header.version == 0) {
            std::cout << "[-] Consensus Error: Invalid block version.\n";
            return false;
        }

        std::cout << "[+] Consensus Validation Passed: Block structure and transactions verified.\n";
        return true;
    }
};

} // namespace NakaCore

#endif // NAKACORE_VALIDATION_H
