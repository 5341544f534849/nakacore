#include "blockchain.h"
#include <iostream>

int main() {
    std::cout << "=== Naka Core ($NKC) Node Initializing ===\n";

    // Inicializar la cadena de bloques con el Génesis hardcodeado
    NakaCore::Blockchain nakachain;
    std::cout << "[+] Blockchain active. Current Height: " << nakachain.GetHeight() << "\n";

    // Simular la creación y minería de un segundo bloque en la red
    NakaCore::Block block2;
    block2.header.version = 1;
    block2.header.timestamp = 1762700700;
    block2.header.bits = 0x1d00ffff;
    block2.header.nonce = 0;

    std::cout << "[*] Mining Block #1 (Proof-of-Work SHA-256)...\n";
    if (NakaCore::ProofOfWork::MineBlock(block2)) {
        std::cout << "[SUCCESS] Block #1 mined successfully!\n";
        nakachain.AddBlock(block2);
    } else {
        std::cout << "[-] Mining failed to find valid nonce.\n";
    }

    std::cout << "=== Naka Core Daemon Executing Securely ===\n";
    return 0;
}
