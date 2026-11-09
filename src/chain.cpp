#include "block.h"

namespace NakaCore {

Block CreateGenesisBlock() {
    Block genesis;
    genesis.header.version = 1;
    genesis.header.timestamp = 1762700400; // Time anchor
    genesis.header.bits = 0x1d00ffff;      // Dificultad inicial PoW
    genesis.header.nonce = 534154;         // Nonce inicial personalizado

    // Transacción Coinbase del Bloque Génesis
    Transaction coinbase;
    coinbase.version = 1;
    
    TxOut genesis_output;
    genesis_output.value = 50 * 100'000'000ULL; // Emisión inicial del bloque génesis
    
    // Mensaje cypherpunk incrustado en el bloque génesis
    std::string genesis_message = "Naka Core $NKC: Autonomous P2P Electronic Cash - 5341544f534849";
    genesis_output.pk_script.assign(genesis_message.begin(), genesis_message.end());
    
    coinbase.vout.push_back(genesis_output);
    genesis.vtx.push_back(coinbase);

    return genesis;
}

} // namespace NakaCore
