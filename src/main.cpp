#include "block.h"
#include "utxo.h"
#include "pow.h"
#include <iostream>

int main() {
    std::cout << "=== Naka Core ($NKC) Node Initializing ===\n";

    // 1. Crear Bloque Génesis
    NakaCore::Block genesis = NakaCore::CreateGenesisBlock();
    std::cout << "[+] Genesis block created successfully.\n";
    std::cout << "[+] Genesis Ticker: $NKC (Max Supply: 20,000,000)\n";

    // 2. Inicializar motor UTXO y registrar la recompensa de coinbase
    NakaCore::UTXOSet utxoSet;
    NakaCore::OutPoint genesis_outpoint;
    std::fill(genesis_outpoint.txid.begin(), genesis_outpoint.txid.end(), 0x00);
    genesis_outpoint.index = 0;

    utxoSet.AddUTXO(genesis_outpoint, genesis.vtx[0].vout[0], true, 0);
    std::cout << "[+] UTXO Set initialized. Active UTXOs: " << utxoSet.Size() << "\n";

    // 3. Ejecutar minería PoW para validar el bloque
    std::cout << "[*] Running Proof-of-Work mining verification...\n";
    if (NakaCore::ProofOfWork::MineBlock(genesis)) {
        std::cout << "[SUCCESS] Block successfully mined! Valid nonce found.\n";
    } else {
        std::cout << "[-] Mining target not reached within iteration limits.\n";
    }

    std::cout << "=== Naka Core Daemon Running Securely ===\n";
    return 0;
}
